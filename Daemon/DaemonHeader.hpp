#pragma once

#include <string>
#include <condition_variable>

struct SimpleHttpMessage
{
    std::string Method;
    std::string URI;
    std::string QueryString;
    std::string Body;
};

namespace DiaoBot
{

class TfcConfigCodec;
extern TfcConfigCodec MainConf;

extern volatile bool IsKilled;
extern std::condition_variable ThreadSleepCV;
extern std::mutex ThreadSleepCVMutex;

void PrintUsage(const char *argv0);
void LoadSharedObjects(void);
void InitializeBotEngine(void);

void DoCronThread(void);
void DoHttpThread(void);

void KillDiaoBotDaemon(void);

void GracefulSignalHandler(int signal);

void SetupCryptor(void);

using HttpRequestHandler = std::string*(*)(SimpleHttpMessage*);

std::string* CallbackHandler(SimpleHttpMessage *msg);

}

// for 3rd-party `URLDecode`
extern "C" char *urlDecode(const char *str);