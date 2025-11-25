/*-----------------------------------------------------------------------*
Project: Recruitment
File: ConcurrencyExercise/context.h

Copyright (C) 2025 Nintendo, All rights reserved.

These coded instructions, statements, and computer programs contain proprietary
information of Nintendo and/or its licensed developers and are protected by
national and international copyright laws. They may not be disclosed to third
parties or copied or duplicated in any form, in whole or in part, without the
prior written consent of Nintendo.

The content herein is highly confidential and should be handled accordingly.
*-----------------------------------------------------------------------*/

#pragma once
#include <optional>
#include <cstdint>
#include <cstddef>

using MessageData = std::uint64_t;

struct Message
{
    std::size_t target_thread_index = 0;
    MessageData message_data;
};

class MessageHandler
{
public:
    virtual bool HasMessageToSend() const = 0;
    virtual std::optional<Message> GetMessageToSend() = 0;
    virtual void ReceivedMessage(MessageData const& message) = 0;
};
