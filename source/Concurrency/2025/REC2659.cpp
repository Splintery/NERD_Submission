#include "REC2659.h"
#include <Helpers.h>
#include <ostream>
#include <iostream>

namespace REC2659
{

void Initialize(size_t thread_count)
{
    UNUSED_PARAM(thread_count);
    std::cout << "Initialising thread mail boxes" << std::endl;
    // while (thread_count --> 0)
    // {
    //     thread_mailboxes.at(thread_count).push(Message());
    //     std::cout << "Pushed messqge on queue of thread[" << thread_count << "]" << std::endl;
    // }
}

void Cleanup()
{
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
    /* pseudo-code phase
    while (message_handler.HasMessageToSend()) // Work until there is no work !
    {
        
        if (mutex.try_lock())
        {
            std::optional<Message> message = message_handler.GetMessageToSend();
            if (it is my message)
            {
                message_handler.ReceivedMessage(message.value())
            }
            else
            {
                pass it to the identified thread queue
            }
            mutex.unlock()
            // Check if the message we got concerns our thread
        }
        else // This means another thread got the message
        {
            // Wait until another thread passes the message onto us
        }
    }
    */

    UNUSED_PARAM(thread_index);
    UNUSED_PARAM(message_handler);
}

} // namespace REC2659
