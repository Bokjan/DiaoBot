#include "dll.h"
#include "Job.hpp"
#include "BotEngine.hpp"

const char * DB_DllName(void)
{
    return "Sample Plugin";
}

void DB_BindEnginePtr(DiaoBot::BotEngine *engine)
{
    auto task = new Sample::CronSample;
    DiaoBot::CronConfig cc;
    cc.SetMonth(DiaoBot::CronConfig::EVERY);
    cc.SetDay(DiaoBot::CronConfig::EVERY);
    cc.SetHour(DiaoBot::CronConfig::EVERY);
    cc.SetMinute(DiaoBot::CronConfig::EVERY);
    engine->RegisterCronJob(cc, task);
}