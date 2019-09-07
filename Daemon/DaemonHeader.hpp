#pragma once

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

}