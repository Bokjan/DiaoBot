#include <DiaoBot/dll.h>
#include <DiaoBot/BotEngine.hpp>
#include "Impl.hpp"

unsigned int LibraryID;

const char *DB_DllName(void) { return "业务逻辑示范库"; }

void DB_Bind(unsigned int libid) {
  // 保存下来自己的 LoadID
  LibraryID = libid;
  // 注册一个示范定时任务
  do {
    auto task = std::make_shared<Sample::CronSample>();
    DiaoBot::CronConfig cc;
    cc.SetMonth(DiaoBot::CronConfig::EVERY);
    cc.SetDay(DiaoBot::CronConfig::EVERY);
    cc.SetHour(10);
    cc.SetMinute(0);
    DiaoBot::BotEngine::GetInstance().RegisterCronJob(LibraryID, cc, task);
  } while (false);
  // 注册一个回调处理器
  DiaoBot::BotEngine::GetInstance().RegisterReplyMaker(
      LibraryID, []() { return std::make_shared<Sample::CallbackEchoSample>(); });
}