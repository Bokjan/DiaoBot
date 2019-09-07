#include <stdexcept>
#include "dll.h"
#include "Log.hpp"
#include "BotEngine.hpp"
#include "DylibManager.hpp"
#include "DaemonHeader.hpp"
#include "TfcConfigCodec.hpp"

namespace DiaoBot
{

TfcConfigCodec MainConf;
volatile bool IsKilled = false;

void PrintUsage(const char *argv0)
{
    fprintf(stderr, "USAGE:\n\t%s <conf_file>\n", argv0);
}

void KillDiaoBotDaemon(void)
{
    IsKilled = true;
}

void GracefulSignalHandler(int signal)
{
    LOG("Signal %d received, terminating...", signal);
    KillDiaoBotDaemon();
}

void LoadSharedObjects(void)
{
    if (!MainConf.HasSection("root\\libs"))
    {
        LOG("%s", "Config file does not have `root\\libs` section!");
        return;
    }
    auto libs = MainConf.GetSection("root\\libs");
    for (const auto &i : libs.Pairs)
    {
        try
        {
            DiaoBot::DylibManager::GetInstance().LoadLibrary(i.Value, i.Key);
        }
        catch (const std::runtime_error &e)
        {
            LOG("Exception caught when loading %s: %s", i.Value.c_str(), e.what());
            continue;
        }
        auto getname = DiaoBot::DylibManager::GetInstance().GetSymbol<decltype(&DB_DllName)>(i.Key, "DB_DllName");
        if (getname == nullptr)
        {
            LOG("%s doesn't have symbol `DB_DllName`!", i.Value.c_str());
            continue;
        }
        LOG("%s loaded, self description: %s", i.Value.c_str(), getname());
        auto bindengine = DiaoBot::DylibManager::GetInstance().GetSymbol<decltype(&DB_BindEnginePtr)>(i.Key, "DB_BindEnginePtr");
        if (bindengine == nullptr)
        {
            LOG("%s doesn't have symbol `DB_BindEnginePtr`!", i.Value.c_str());
            continue;
        }
        bindengine(&DiaoBot::BotEngine::GetInstance());
    }
}

void InitializeBotEngine(void)
{

}

}