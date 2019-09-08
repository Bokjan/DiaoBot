#pragma once

#include <list>
#include "Runnable.hpp"
#include "CronConfig.hpp"

namespace DiaoBot
{

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
    friend void DoCronThread(void);

    string WebhookUrl;
    std::list<CronJobPair> CronJobList;
};

}