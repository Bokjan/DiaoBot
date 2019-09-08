#include "Job.hpp"
#include <DiaoBot/Log.hpp>
#include <DiaoBot/BotEngine.hpp>
#include <DiaoBot/WeworkMessage.hpp>

namespace Sample
{

void CronSample::Run(void)
{
    DiaoBot::WeworkImageMessage wim;
    wim.SetHttpImage("https://avatars0.githubusercontent.com/u/8068621?s=460&v=4");
    DiaoBot::BotEngine::GetInstance().SendMessage(wim);
}

}