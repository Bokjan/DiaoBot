#include "Job.hpp"
#include <DiaoBot/dll.h>
#include <DiaoBot/BotEngine.hpp>

unsigned int LibraryID;

const char * DB_DllName(void)
{
    return "Sample Plugin";
}

void DB_Bind(unsigned int libid)
{
    LibraryID = libid;
    auto task = std::make_shared<Sample::CronSample>();
    DiaoBot::CronConfig cc;
    cc.SetMonth(DiaoBot::CronConfig::EVERY);
    cc.SetDay(DiaoBot::CronConfig::EVERY);
    cc.SetHour(DiaoBot::CronConfig::EVERY);
    cc.SetMinute(DiaoBot::CronConfig::EVERY);
    DiaoBot::BotEngine::GetInstance().RegisterCronJob(LibraryID, cc, task);
}