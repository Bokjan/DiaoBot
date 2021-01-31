#include <stdexcept>
#include "BotEngine.hpp"
#include "DaemonHeader.hpp"
#include "DylibManager.hpp"
#include "Log.hpp"
#include "TfcConfigCodec.hpp"
#include "dll.h"

namespace DiaoBot {

TfcConfigCodec MainConf;
volatile bool IsKilled = false;
std::condition_variable ThreadSleepCV;
std::mutex ThreadSleepCVMutex;

void PrintUsage(const char *argv0) { fprintf(stderr, "USAGE:\n\t%s <conf_file>\n", argv0); }

void KillDiaoBotDaemon(void) {
  do {
    std::lock_guard<std::mutex> lock(ThreadSleepCVMutex);
    IsKilled = true;
  } while (false);
  ThreadSleepCV.notify_all();
}

void GracefulSignalHandler(int signal) {
  LOG("Signal %d received, terminating...", signal);
  KillDiaoBotDaemon();
}

void LoadSharedObjects(void) {
  if (!MainConf.HasSection("root\\libs")) {
    LOG("%s", "Config file does not have `root\\libs` section!");
    return;
  }
  auto libs = MainConf.GetSection("root\\libs");
  for (const auto &i : libs.Pairs) {
    try {
      DiaoBot::DylibManager::GetInstance().LoadLibrary(i.Value, i.Key);
    } catch (const std::runtime_error &e) {
      LOG("Exception caught: %s", e.what());
      continue;
    }
    auto getname =
        DiaoBot::DylibManager::GetInstance().GetSymbol<decltype(&DB_DllName)>(i.Key, "DB_DllName");
    if (getname == nullptr) {
      LOG("%s doesn't have symbol `DB_DllName`!", i.Value.c_str());
      continue;
    }
    LOG("%s loaded, self description: %s", i.Value.c_str(), getname());
    auto bind =
        DiaoBot::DylibManager::GetInstance().GetSymbol<decltype(&DB_Bind)>(i.Key, "DB_Bind");
    if (bind == nullptr) {
      LOG("%s doesn't have symbol `DB_Bind`!", i.Value.c_str());
      continue;
    }
    bind(DiaoBot::DylibManager::GetInstance().GetLibraryID(i.Key));
  }
}

void InitializeBotEngine(void) {
  if (!MainConf.HasKV("root", "webhook")) {
    throw std::runtime_error("Webhook URL not configured!");
  }
  BotEngine::GetInstance().SetWebhookUrl(MainConf.GetKV("root", "webhook"));
}

}  // namespace DiaoBot
