/*-----------------------------------------------------------------------*
Project: Recruitment
File: ConcurrencyExercise/candidate_main.cpp

Copyright (C) 2025 Nintendo, All rights reserved.

These coded instructions, statements, and computer programs contain proprietary
information of Nintendo and/or its licensed developers and are protected by
national and international copyright laws. They may not be disclosed to third
parties or copied or duplicated in any form, in whole or in part, without the
prior written consent of Nintendo.

The content herein is highly confidential and should be handled accordingly.
*-----------------------------------------------------------------------*/

#include "all.h"
#include <random>
#include <thread>
#include <ostream>
#include <iostream>

#define CANDIDATE REC2659

namespace test
{

static constexpr size_t main_thread_index = std::numeric_limits<size_t>::max();
static constexpr size_t invalid_thread_index = std::numeric_limits<size_t>::max() - 1;
thread_local size_t tl_own_thread_index = invalid_thread_index;
class MessagePassingMessageHandler final : public MessageHandler
{
    size_t m_thread_index = 0;
    size_t m_thread_count = 0;

    std::mt19937_64 rng;

    size_t GetRandomTargetThreadIndex()
    {
        size_t target_thread = std::uniform_int_distribution<size_t>(0, m_thread_count - 2)(rng);
        if (target_thread >= m_thread_index)
            target_thread++;
        return target_thread;
    }

    size_t m_message_count = 0;

    std::vector<Message> m_next_messages;

    void SetNextMessage(uint64_t message_index)
    {
        if (message_index >= m_message_count)
            return;

        uint64_t target_thread = GetRandomTargetThreadIndex();
        MessageData data = (message_index << 32) + (target_thread << 16) + (m_thread_index << 0);
        Message message = Message{ .target_thread_index = target_thread, .message_data = data };

        m_tx_count++;
        m_tx_checksum ^= data;

        m_next_messages.push_back(message);
    }

    uint32_t ReceiveMessage(MessageData const& data)
    {
        uint32_t message_index = uint32_t((data >> 32) & 0xFFFFFFFF);
        size_t source_thread = (data >> 0) & 0xFFFF;
        size_t target_thread = (data >> 16) & 0xFFFF;
        (void)source_thread;

        if (target_thread != m_thread_index)
            m_error.emplace("Message sent to the wrong thread!");

        m_rx_count++;
        m_rx_checksum ^= data;
        return message_index;
    }

public:
    uint64_t m_tx_count = 0;
    uint64_t m_rx_count = 0;
    uint64_t m_tx_checksum = 0;
    uint64_t m_rx_checksum = 0;
    mutable std::optional<std::string> m_error;

    MessagePassingMessageHandler(size_t thread_index, size_t thread_count, size_t message_count) 
        : m_thread_index(thread_index), m_thread_count(thread_count), rng(thread_index * 532809411), m_message_count(message_count)
    {
            SetNextMessage(0);
    }

    bool HasMessageToSend() const override
    {
        if (tl_own_thread_index != m_thread_index && tl_own_thread_index != main_thread_index)
            m_error.emplace("MessageHandler was accessed by the wrong thread!");
        return m_next_messages.size() > 0;
    }
    std::optional<Message> GetMessageToSend() override
    {
        if (tl_own_thread_index != m_thread_index)
            m_error.emplace("MessageHandler was accessed by the wrong thread!");
        if (m_next_messages.empty())
            return {};

        auto message = m_next_messages.back();
        m_next_messages.pop_back();
        return message;
    }

    void ReceivedMessage(MessageData const& message) override
    {
        if (tl_own_thread_index != m_thread_index)
            m_error.emplace("MessageHandler was accessed by the wrong thread!");
        size_t message_index = ReceiveMessage(message);
        SetNextMessage(message_index + 1);
    }
};

}

#if defined(IS_CANDIDATE)
int main()
#else
int candidate_main()
#endif
{
    try
    {
        static constexpr size_t thread_count = 10;
        static constexpr size_t repetitions = 10;

        for (size_t b = 0; b < repetitions; b++)
        {
            std::vector<std::unique_ptr<test::MessagePassingMessageHandler>> message_handlers;
            for (size_t a = 0; a < thread_count; a++)
            {
                message_handlers.push_back(std::make_unique<test::MessagePassingMessageHandler>(a, thread_count, 1000));
            }

            std::vector<std::thread> threads;

            test::tl_own_thread_index = test::main_thread_index;

            CANDIDATE::Initialize(thread_count);
            for (size_t a = 0; a < thread_count; a++)
            {
                threads.emplace_back([&, a]()
                    {
                        test::tl_own_thread_index = a;
                        CANDIDATE::HandleMessages(a, *message_handlers[a]);
                    });
            }
            for (size_t a = 0; a < thread_count; a++)
            {
                threads[a].join();
            }
            CANDIDATE::Cleanup();

            size_t total_tx_count = 0;
            size_t total_rx_count = 0;
            size_t total_tx_checksum = 0;
            size_t total_rx_checksum = 0;
            bool has_messages_left_to_send = false;
            std::optional<std::string> error_message;
            for (auto const& handler : message_handlers)
            {
                total_tx_count += handler->m_tx_count;
                total_rx_count += handler->m_rx_count;
                total_tx_checksum ^= handler->m_tx_checksum;
                total_rx_checksum ^= handler->m_rx_checksum;
                has_messages_left_to_send |= handler->HasMessageToSend();
                if (handler->m_error.has_value())
                    error_message = handler->m_error;
            }
            if (error_message.has_value())
            {
                throw std::runtime_error(error_message.value());
            }
            if (has_messages_left_to_send)
            {
                throw std::runtime_error("There are messages left to be sent!");
            }
            else if (total_tx_count > total_rx_count)
            {
                std::cout << "total_tx_count[" << total_tx_count << "] && total_rx_count[" << total_rx_count << "]" << std::endl;
                throw std::runtime_error("Some messages were not received!");
            }
            else if (total_tx_count < total_rx_count)
            {
                throw std::runtime_error("Some messages were received multiple times!");
            }
            else if (total_tx_checksum != total_rx_checksum)
            {
                throw std::runtime_error("Some messages were not correct!");
            }
        }
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "An exception was thrown: %s\n", e.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        fprintf(stderr, "An exception was thrown\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
