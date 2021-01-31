#include <chrono>
#include <ctime>
#include <string>
#include <thread>
#include "BotEngineImpl.hpp"
#include "DaemonHeader.hpp"
#include "Log.hpp"

using std::string;

namespace DiaoBot {

static void DoLoop(std::list<CronJob> &list, tm *now) {
  for (const auto &i : list) {
    bool willExecute = false;
    do {
      if (!i.Config.IsSetMinute(now->tm_min)) break;
      if (!i.Config.IsSetHour(now->tm_hour)) break;
      if (!i.Config.IsSetMonth(now->tm_mon + 1)) break;
      if (!i.Config.IsSetDayOfWeek((now->tm_wday + 7) % 7) && !i.Config.IsSetDay(now->tm_mday))
        break;
      willExecute = true;
    } while (false);
    if (!willExecute) continue;
    std::thread task(&Runnable::Run, i.Task);
    task.detach();
  }
}

void DoCronThread(void) {
  LOG("%s", "CronThread started");
  std::unique_lock<std::mutex> lk(ThreadSleepCVMutex);
  decltype(std::chrono::system_clock::now()) time_point;
  do {
    if (IsKilled) {
      break;
    }
    // Get current time
    time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm *tmptr = localtime(&t);
    // Emit jobs
    DoLoop(GetEngineImpl()->CronJobList, tmptr);
    // Set to next minute
    ++tmptr->tm_min;
    tmptr->tm_sec = 0;
    time_point = std::chrono::system_clock::from_time_t(mktime(tmptr));
  } while (ThreadSleepCV.wait_until(lk, time_point) == std::cv_status::timeout);
  LOG("%s", "CronThread terminated");
}
}  // namespace DiaoBot
