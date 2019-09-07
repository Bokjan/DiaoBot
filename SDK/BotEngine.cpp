#include <list>
#include "BotEngine.hpp"

namespace DiaoBot
{

BotEngine BotEngine::Instance;

struct CronJobPair
{
    CronConfig Config;
    Runnable*  Task;
    CronJobPair(const CronConfig &config, Runnable *runnable):
        Config(config), Task(runnable) { }
};

class BotEngineImpl
{
    friend class BotEngine;
    std::list<CronJobPair> CronJobList;
};

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

}