#pragma once

#include <string>
#include "Runnable.hpp"
#include "CronConfig.hpp"

using std::string;

namespace DiaoBot
{

class BotEngineImpl;
class BotEngine
{
public:
    ~BotEngine(void);
    static BotEngine& GetInstance(void);

    void RegisterCronJob(const CronConfig &config, Runnable *runnable);
    void DestroyCronJobs(Runnable *runnable); // 销毁全部对应Runnable的任务

private:
    BotEngineImpl *PImpl;
    BotEngine(void);
    static BotEngine Instance;
    friend void DoCronThread(void);
};

}