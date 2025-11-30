#include "REC2659.h"
#include <Helpers.h>
#include <ostream>
#include <iostream>

namespace REC2659
{

void Initialize(size_t thread_count)
{
    std::cout << "Initialising thread mail boxes" << std::endl;
    global_thread_count = thread_count;
    for (size_t thread_id = 0; thread_id < thread_count; thread_id++)
    {
        thread_mailboxes.try_emplace(thread_id); //* Mutexes can't be moved so they need to be built in place
    }
    if (thread_mailboxes.size() != thread_count)
    {
        //TODO Handle error if a MailBox couldn't be emplaced
    }
}

void Cleanup()
{
    std::cout << "Cleaning up" << std::endl;
    work_done.store(false);
    idle_threads.store(0);
    message_in_flight.store(0);
}

void SignalAll()
{
    std::cout << "SignalAll()" << std::endl;
    for (auto &i : thread_mailboxes)
    {
        i.second.cv.notify_all();
    }
}

/**
 * @brief
 * This function is used to either receive a message directly from the handler or to redirect it to the target thread in a thread-safe way
 * 
 * @param thread_index the index of the current thread
 * @param message_handler the message handler of the current thread
 */
void HandleMessageFromHandler(size_t thread_index, MessageHandler& message_handler, MailBox& mailbox)
{
    std::optional<Message> curr_message = message_handler.GetMessageToSend();
    if (!curr_message.has_value()) return;

    Message m = curr_message.value();

    if (m.target_thread_index == thread_index)
    {
        // std::cout << "Thread[" << thread_index << "] is receiving a message by its handler" << std::endl;
        message_handler.ReceivedMessage(m.message_data);
        return;
    }

    //? increment message_in_flight before pushing to recipients mailbox
    message_in_flight.fetch_add(1, std::memory_order_acq_rel);

    { //! A lock guard mutex is used to place a lock on the mutex, it will be released by exiting the else block
        MailBox &target_mb = thread_mailboxes.at(m.target_thread_index);

        std::lock_guard lock(target_mb.idle_thread_mu);

        // std::cout << "Thread[" << thread_index << "] is sending a message to " << m.target_thread_index << std::endl;
        target_mb.mail.push(std::move(m));
        target_mb.cv.notify_one();
    } //! The lock on the idle mutex is released and the thread concerned will be unblocked
}

/**
 * @brief
 * This function is used to receive a message from another thread that notified us on our threads idle condition_variable
 * It is important that the lock is placed on the resource before this method.
 * @param thread_index the index of the current thread
 * @param message_handler the message handler of the current thread
 */
void HandleMessagesFromThreads(size_t thread_index, MessageHandler& message_handler, MailBox& mailbox)
{
    while (!mailbox.mail.empty())
    {
        Message received_message = std::move(mailbox.mail.front());
        mailbox.mail.pop();

        if (received_message.target_thread_index == thread_index) // Simple paranoia
        {
            // std::cout << "Thread[" << thread_index << "] is receiving a message from another thread" << std::endl;
            message_handler.ReceivedMessage(received_message.message_data);
        }
        
        //? One message that was previously in flight has been received
        message_in_flight.fetch_sub(1, std::memory_order_acq_rel);
    }
}

/**
 * @brief
 * This function is called once for each thread. 
 * I Designed a Petri net (Réseau de pétri) to isolate the different states a thread could be in.
 * 
 * @param thread_index the index of the current thread
 * @param message_handler the message handler of the current thread
 */
void HandleMessages(size_t thread_index, MessageHandler& message_handler)
{
    MailBox &mb = thread_mailboxes.at(thread_index);
    ThreadState curr_state = REDIRECTING;

    while (!work_done.load())
    {
        switch (curr_state)
        {
        case REDIRECTING:
            if (message_handler.HasMessageToSend())
            {
                HandleMessageFromHandler(thread_index, message_handler, mb);
            }
            else
            {
                std::unique_lock<std::mutex> lock(mb.idle_thread_mu); //! tries to lock the thread mutex

                if (mb.mail.empty())
                    curr_state = WAITING;
                else
                    curr_state = ACCEPTING;
            }
            break;
        case ACCEPTING:    
        {
            std::unique_lock<std::mutex> lock(mb.idle_thread_mu);
            HandleMessagesFromThreads(thread_index, message_handler, mb); // Handles all messages received until now

            curr_state = REDIRECTING; // If we accepted messages, new ones might need to be redirected
            break;
        }
        case WAITING:
        {
            std::unique_lock<std::mutex> lock(mb.idle_thread_mu);

            unsigned long prev_idle = idle_threads.fetch_add(1, std::memory_order_acq_rel);

            if ((prev_idle + 1 == global_thread_count) && (message_in_flight.load(std::memory_order_acquire) == 0)) //? Check if I am the last to go to sleep
            {
                work_done.store(true, std::memory_order_release);
                idle_threads.fetch_sub(1, std::memory_order_acq_rel); // Only a single thread will notify the others

                // Notify all threads so they see work_done
                SignalAll();
                lock.unlock();
                curr_state = FINISHED;
            }
            else
            {
                // std::cout << "Going to sleep[" << thread_index << "]" << std::endl;

                mb.cv.wait(lock, [&]{
                    return work_done.load(std::memory_order_acquire) || !mb.mail.empty();
                });//! releases the lock while waiting, lock is placed when woken up
                
                idle_threads.fetch_sub(1, std::memory_order_acq_rel);
                // std::cout << "Waking up[" << thread_index << "]" << std::endl;

                if (work_done.load(std::memory_order_acquire))
                {
                    lock.unlock();
                    curr_state = FINISHED;
                }
                else if (!mb.mail.empty())
                {
                    lock.unlock();
                    curr_state = ACCEPTING;
                }
                else // spurious wake up -> go back and check if messages need redirecting
                {
                    lock.unlock();
                    curr_state = REDIRECTING;
                }
            }
            break;
        }
        case FINISHED: //? No lock needed for this state
            work_done.store(true, std::memory_order_release);
            break;
        default:
            break;
        }
    }
}

} // namespace REC2659
