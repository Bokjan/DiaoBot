#include "Impl.hpp"
#include <DiaoBot/Log.hpp>
#include <DiaoBot/BotEngine.hpp>
#include <DiaoBot/WeworkMessage.hpp>
#include <DiaoBot/CallbackMessage.hpp>

namespace Sample
{

void CronSample::Run(void)
{
    DiaoBot::WeworkTextMessage wtm;
    wtm.SetContent("早上好");
    DiaoBot::BotEngine::GetInstance().SendMessage(wtm);
}

bool CallbackEchoSample::WillReply(const std::shared_ptr<DiaoBot::CallbackMessage> &msg)
{
    return msg->MsgType == DiaoBot::CallbackMessage::TEXT;
}

std::shared_ptr<DiaoBot::WeworkMessage> CallbackEchoSample::Reply(std::shared_ptr<DiaoBot::CallbackMessage> _msg)
{
    auto msg = std::dynamic_pointer_cast<DiaoBot::CallbackTextMessage>(_msg);
    auto ret = std::make_shared<DiaoBot::WeworkTextMessage>();
    ret->SetContent(msg->Content);
    return ret;
}

}