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
    size_t previous_pen_pal = -1;
};
static std::unordered_map<size_t, MailBox> thread_mailboxes;
static std::atomic_flag work_done{};
static std::mutex idle_couter_mutex; // This mutex is the only bottle neck of this approach
static size_t idle_counter = 0;
static size_t global_thread_count;

enum ThreadState
{
    REDIRECTING,
    ACCEPTING,
    WAITING,
    WAKING_UP,
    FINISHED
};

void Initialize(size_t thread_count);
void Cleanup();

void SignalAll();

void HandleMessageFromHandler(size_t thread_index, MessageHandler& message_handler, MailBox& mailbox);
void HandleMessageFromThreads(size_t thread_index, MessageHandler& message_handler, MailBox& mailbox);
void HandleMessages(size_t thread_index, MessageHandler& message_handler);

} // namespace REC2659

#endif // REC2659_H
