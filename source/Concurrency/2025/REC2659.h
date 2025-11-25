#ifndef REC2659_H
#define REC2659_H
#include "context.h"

namespace REC2659
{
	
void Initialize(size_t thread_count);
void Cleanup();

void HandleMessages(size_t thread_index, MessageHandler& message_handler);

} // namespace REC2659

#endif // REC2659_H
