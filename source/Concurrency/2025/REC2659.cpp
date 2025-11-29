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
    work_done.clear(); //* Sets the loop flag to false
}

void Cleanup()
{
    // Nothing was allocated Dynamically by Initialize
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
    if (curr_message.has_value())
    {
        if (curr_message.value().target_thread_index  == thread_index)
        {
            std::cout << "Thread[" << thread_index << "] is receiving a message by its handler" << std::endl;
            message_handler.ReceivedMessage(curr_message.value().message_data);
        }
        else
        { //! A lock guard mutex is used to place a lock on the mutex, it will be released by exiting the else block
            std::lock_guard lock(thread_mailboxes.at(curr_message.value().target_thread_index).idle_thread_mu);
            mailbox.previous_pen_pal = curr_message.value().target_thread_index;

            std::cout << "Thread[" << thread_index << "] is sending a message to " << curr_message.value().target_thread_index << std::endl;
            thread_mailboxes.at(curr_message.value().target_thread_index).mail.push(
                curr_message.value()
            );
            thread_mailboxes.at(curr_message.value().target_thread_index).cv.notify_one();
        } //! The lock on the idle mutex is released and the thread concerned will be unblocked
    }
}

/**
 * @brief
 * This function is used to receive a message from another thread that notified us on our threads idle condition_variable
 * It is important that the lock is placed on the resource before this method.
 * @param thread_index the index of the current thread
 * @param message_handler the message handler of the current thread
 */
void HandleMessageFromThreads(size_t thread_index, MessageHandler& message_handler, MailBox& mailbox)
{
    while (!mailbox.mail.empty())
    {
        Message received_message = mailbox.mail.front();
        if (received_message.target_thread_index == thread_index) // Simple paranoia
        {
            std::cout << "Thread[" << thread_index << "] is receiving a message from another thread" << std::endl;
            message_handler.ReceivedMessage(received_message.message_data);
        }
        mailbox.mail.pop();
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
    std::unique_lock<std::mutex> *mail_lock;
    ThreadState curr_state = REDIRECTING;
    while (!work_done.test())
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
                mail_lock = new std::unique_lock(mb.idle_thread_mu); //! tries to lock the thread mutex

                if (mb.mail.empty())
                    curr_state = WAITING;
                else
                    curr_state = ACCEPTING;
            }
            break;
        case ACCEPTING: //* Lock allready aquired before enterring this state
            HandleMessageFromThreads(thread_index, message_handler, mb);

            mail_lock->unlock(); //! releases the lock on thread mutex
            curr_state = REDIRECTING; // If we accepted messages, new ones might need to be redirected

            break;
        case WAITING: //* Lock allready aquired before enterring this state
            if (mb.previous_pen_pal != (size_t) -1)
                thread_mailboxes.at(mb.previous_pen_pal).idle_thread_mu.lock(); //! Needed to prevent race condition
            
            idle_couter_mutex.lock();
            idle_counter++;

            if (mb.previous_pen_pal != (size_t) -1)
                thread_mailboxes.at(mb.previous_pen_pal).idle_thread_mu.unlock(); //! Needed to prevent race condition

            if (idle_counter == global_thread_count) //? Check if I am the last to go to sleep
            {
                idle_counter--; // Only a single thread will notify the others
                idle_couter_mutex.unlock(); // Unlock before signaling so threads can finish without having to wait that every thread is notified
                SignalAll();

                curr_state = FINISHED;
            }
            else
            {
                idle_couter_mutex.unlock();

                mb.cv.wait(*mail_lock);//! releases the lock while waiting, lock is placed when woken up
                curr_state = WAKING_UP;

                idle_couter_mutex.lock();
                idle_counter--;
                idle_couter_mutex.unlock();

                mail_lock->unlock(); //! Necessary to avoid a race condition
            }
            break;
        case WAKING_UP:
            mail_lock->lock();
            if (mb.mail.empty())
            {
                curr_state = FINISHED; //? If we got notified with no work to do, it means we are finished with working
                mail_lock->unlock();
            }
            else
            {
                curr_state = ACCEPTING;
            }
            break;
        case FINISHED: //? No lock needed for this state
            work_done.test_and_set();
            break;
        default:
            break;
        }
    }
}

} // namespace REC2659
