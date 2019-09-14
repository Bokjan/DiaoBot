#pragma once

#include <DiaoBot/Runnable.hpp>
#include <DiaoBot/Replyable.hpp>

namespace Sample
{

class CronSample : public DiaoBot::Runnable
{
public:
    void Run(void) override;
};

class CallbackEchoSample : public DiaoBot::Replyable
{
public:
    bool WillReply(const std::shared_ptr<DiaoBot::CallbackMessage> &);
    std::shared_ptr<DiaoBot::WeworkMessage> Reply(std::shared_ptr<DiaoBot::CallbackMessage>);
};

}