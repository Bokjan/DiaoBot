#pragma once

#include <string>

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

void PrintUsage(const char *argv0);
void LoadSharedObjects(void);
void InitializeBotEngine(void);

void DoCronThread(void);
void DoHttpThread(void);

extern volatile bool IsKilled;
void KillDiaoBotDaemon(void);

void GracefulSignalHandler(int signal);

using HttpRequestHandler = std::string*(*)(SimpleHttpMessage*);

std::string* CallbackHandler(SimpleHttpMessage *msg);

}

// for 3rd-party `URLDecode`
extern "C" char *urlDecode(const char *str);