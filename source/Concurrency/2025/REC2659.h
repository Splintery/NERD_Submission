#ifndef REC2659_H
#define REC2659_H
#include "context.h"
#include <queue>
#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include <atomic>


namespace REC2659
{
// Multiple messages could be addressed to the same tread, hence the use of a queue
struct MailBox
{
    std::condition_variable cv;
    std::mutex idle_mu;
    std::mutex mail_mu;
    std::queue<Message> mail;
};
static std::unordered_map<size_t, MailBox> thread_mailboxes;
static std::atomic_flag work_done{};

void Initialize(size_t thread_count);
void Cleanup();

void HandleMessages(size_t thread_index, MessageHandler& message_handler);

} // namespace REC2659

#endif // REC2659_H
