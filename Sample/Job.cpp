#include "Job.hpp"
#include "Log.hpp"
#include "WeworkMessage.hpp"

namespace Sample
{

void CronSample::Run(void)
{
    DiaoBot::WeworkTextMessage wtm;
    wtm.SetChatID(DiaoBot::WeworkTextMessage::ALL);
    wtm.SetContent("Hello!");
}

}