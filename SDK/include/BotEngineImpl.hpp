#pragma once

#include <list>
#include <mutex>
#include <string>
#include "Runnable.hpp"
#include "Replyable.hpp"
#include "CronConfig.hpp"

using std::string;

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
    CreateReplyableFunc CreateReplyable;
    ReplyMaker(unsigned int id, CreateReplyableFunc func):
        LibID(id), CreateReplyable(func) { }
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

BotEngineImpl* GetEngineImpl(void);

}