#include <ctime>
#include <chrono>
#include <thread>
#include "Log.hpp"
#include "DaemonHeader.hpp"
#include "BotEngine.hpp"
#include "BotEngineImpl.hpp"

namespace DiaoBot
{

static void DoLoop(std::list<CronJobPair> &list, tm *now)
{
    for (const auto &i : list)
    {
        bool willExecute = false;
        do
        {
            // LOG("%lx, %x, %x, %x, %x", 
            //     i.Config.GetMinute(),
            //     i.Config.GetHour(),
            //     i.Config.GetMonth(),
            //     i.Config.GetDay(),
            //     i.Config.GetDayOfWeek());
            // LOG("NOW: %d, %d, %d, %d, %d",
            //     now->tm_min,
            //     now->tm_hour,
            //     now->tm_mon,
            //     now->tm_mday,
            //     now->tm_wday);
            if (!i.Config.IsSetMinute(now->tm_min))
                break;
            if (!i.Config.IsSetHour(now->tm_hour))
                break;
            if (!i.Config.IsSetMonth(now->tm_mon + 1))
                break;
            if (!i.Config.IsSetDayOfWeek((now->tm_wday + 7) % 7) && !i.Config.IsSetDay(now->tm_mday))
                break;
            willExecute = true;
        }
        while (false);
        if (!willExecute)
            continue;
        std::thread task(&Runnable::Run, i.Task);
        task.detach();
    }
}

void DoCronThread(void)
{
    LOG("%s", "CronThread started");
    while (!IsKilled)
    {
        // Get current time
        time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        tm *tmptr = localtime(&t);
        // Emit jobs
        DoLoop(BotEngine::GetInstance().PImpl->CronJobList, tmptr);
        // Set to next minute
        ++tmptr->tm_min;
        tmptr->tm_sec = 0;
        std::this_thread::sleep_until(std::chrono::system_clock::from_time_t(mktime(tmptr)));
    }
    LOG("%s", "CronThread terminated");
}

}
