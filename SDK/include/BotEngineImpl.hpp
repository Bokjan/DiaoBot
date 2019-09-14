#pragma once

#include <list>
#include <mutex>
#include "Runnable.hpp"
#include "Replyable.hpp"
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

struct ReplyMaker
{
    unsigned int LibID;
    ReplyablePtr Maker;
    ReplyMaker(unsigned int id, ReplyablePtr &rptr):
        LibID(id), Maker(rptr) { }
};

class BotEngineImpl
{
public:
    string                  WebhookUrl;
    std::mutex              CronJobListMutex;
    std::mutex              ReplyMakerListMutex;
    std::list<CronJob>      CronJobList;
    std::list<ReplyMaker>   ReplyMakerList;
};

}