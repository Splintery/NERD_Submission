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
    std::mutex idle_thread_mu;
    std::queue<Message> mail;
};
static std::unordered_map<size_t, MailBox> thread_mailboxes;
static std::atomic<bool> work_done{false};

static std::atomic<unsigned long> idle_threads{0};
static std::atomic<unsigned long> message_in_flight{0}; 
static unsigned long global_thread_count;

enum ThreadState
{
    REDIRECTING,
    ACCEPTING,
    WAITING,
    FINISHED
};

void Initialize(size_t thread_count);
void Cleanup();

void SignalAll();

void HandleMessageFromHandler(size_t thread_index, MessageHandler& message_handler, MailBox& mailbox);
void HandleMessagesFromThreads(size_t thread_index, MessageHandler& message_handler, MailBox& mailbox);
void HandleMessages(size_t thread_index, MessageHandler& message_handler);

} // namespace REC2659

#endif // REC2659_H
