#include <list>
#include "BotEngine.hpp"
#include "BotEngineImpl.hpp"

namespace DiaoBot
{

BotEngine BotEngine::Instance;

BotEngine::BotEngine(void)
{
    PImpl = new BotEngineImpl;
}

BotEngine::~BotEngine(void)
{
    delete PImpl;
}

BotEngine& BotEngine::GetInstance(void)
{
    return BotEngine::Instance;
}

void BotEngine::RegisterCronJob(const CronConfig &config, Runnable *runnable)
{
    PImpl->CronJobList.push_back(CronJobPair(config, runnable));
}

void BotEngine::DestroyCronJobs(Runnable *runnable)
{
    for (auto it = PImpl->CronJobList.begin(); it != PImpl->CronJobList.end(); ++it)
    {
        if (it->Task == runnable)
        {
            PImpl->CronJobList.erase(it++); // 处理迭代器失效问题
        }
    }
    delete runnable;
}

Runnable::~Runnable(void)
{
    
}

}