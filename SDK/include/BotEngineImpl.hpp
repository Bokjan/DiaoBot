#pragma once

#include <list>
#include <mutex>
#include "Runnable.hpp"
#include "CronConfig.hpp"

namespace DiaoBot
{

struct CronJob
{
    unsigned int LibID;
    CronConfig   Config;
    RunnablePtr  Task;
    CronJob(unsigned int id, const CronConfig &config, RunnablePtr runnable):
        LibID(id), Config(config), Task(runnable) { }
};

class BotEngineImpl
{
    friend class    BotEngine;
    friend void     DoCronThread(void);

    string              WebhookUrl;
    std::mutex          CronJobListMutex;
    std::list<CronJob>  CronJobList;
};

}