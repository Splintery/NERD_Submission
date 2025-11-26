#ifndef REC2659_H
#define REC2659_H
#include "context.h"
#include <vector>
#include <queue>

namespace REC2659
{
static std::vector<std::queue<Message>> thread_mailboxes;
void Initialize(size_t thread_count);
void Cleanup();

void HandleMessages(size_t thread_index, MessageHandler& message_handler);

} // namespace REC2659

#endif // REC2659_H
