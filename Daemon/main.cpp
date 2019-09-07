#include <thread>
#include <csignal>
#include "Log.hpp"
#include "DaemonHeader.hpp"
#include "TfcConfigCodec.hpp"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        DiaoBot::PrintUsage(argv[0]);
        exit(EXIT_SUCCESS);
    }
    int ret;
    ret = DiaoBot::MainConf.ParseFile(argv[1]);
    if (ret != DiaoBot::TfcConfigCodec::SUCCESS)
    {
        LOG("Parse %s failed, retcode: %d", argv[1], ret);
        exit(EXIT_FAILURE);
    }

    DiaoBot::LoadSharedObjects();
    DiaoBot::InitializeBotEngine();

    signal(SIGTERM, DiaoBot::GracefulSignalHandler);

    std::thread CronThread(DiaoBot::DoCronThread);
    std::thread HttpThread(DiaoBot::DoHttpThread);
    CronThread.join();
    HttpThread.join();
}