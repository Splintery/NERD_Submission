#include "REC2659.h"
#include <Helpers.h>
#include <ostream>
#include <iostream>

namespace REC2659
{

void Initialize(size_t thread_count)
{
    std::cout << "Initialising thread mail boxes" << std::endl;
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
}

/**
 * @brief
 * This function is used to either receive a message directly from the handler or to redirect it to the target thread in a thread-safe way
 * 
 * @param thread_index the index of the current thread
 * @param message_handler the message handler of the current thread
 */
void HandleMessageFromHandler(size_t thread_index, MessageHandler& message_handler)
{
    std::optional<Message> curr_message = message_handler.GetMessageToSend();
    if (curr_message.has_value())
    {
        if (curr_message.value().target_thread_index  == thread_index)
        {
            message_handler.ReceivedMessage(curr_message.value().message_data);
        }
        else
        { //! A lock guard mutex is used to place a lock on the mutex, it will be released by exciting the else block
            std::lock_guard lock(thread_mailboxes.at(curr_message.value().target_thread_index).idle_mu);

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
 * 
 * @param thread_index the index of the current thread
 * @param message_handler the message handler of the current thread
 */
void HandleMessageFromThreads(size_t thread_index, MessageHandler& message_handler)
{
    //TODO implement
    UNUSED_PARAM(thread_index);
    UNUSED_PARAM(message_handler);
}

/**
 * @brief
 * This function is called once for each thread. This is where you need to write the main loop for each of the threads.
 * 
 * @param thread_index the index of the current thread
 * @param message_handler the message handler of the current thread
 */
void HandleMessages(size_t thread_index, MessageHandler& message_handler)
{
    MailBox &mb = thread_mailboxes.at(thread_index);
    while (!work_done.test())
    {
        while (message_handler.HasMessageToSend())
        {
            HandleMessageFromHandler(thread_index, message_handler);
        }
                
        HandleMessageFromThreads(thread_index, message_handler);

        std::unique_lock lock(mb.idle_mu);
        mb.cv.wait(lock);
    }
}

} // namespace REC2659
