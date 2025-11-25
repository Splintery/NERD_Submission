#include "REC2659.h"
#include <Helpers.h>

namespace REC2659
{

void Initialize(size_t thread_count)
{
    UNUSED_PARAM(thread_count);
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
    UNUSED_PARAM(thread_index);
    UNUSED_PARAM(message_handler);
}

} // namespace REC2659
