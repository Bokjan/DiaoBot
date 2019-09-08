#pragma once

#include <string>
#include "Runnable.hpp"
#include "CronConfig.hpp"

using std::string;

namespace DiaoBot
{

struct HttpResponse
{
    long   ResponseCode;
    string BodyData;
};

class WeworkMessage;
class BotEngineImpl;
class BotEngine
{
public:
    ~BotEngine(void);
    static BotEngine& GetInstance(void);

    void SetWebhookUrl(const string &url);

    int  SendMessage(const WeworkMessage &message);

    void RegisterCronJob(const CronConfig &config, Runnable *runnable);
    void DestroyCronJobs(Runnable *runnable); // 销毁全部对应Runnable的任务

    HttpResponse HttpGetRequest(const string &url, bool proxy = true);
    HttpResponse HttpPostRequest(const string &url, const string &body, bool proxy = true);

private:
    BotEngineImpl *PImpl;
    BotEngine(void);
    static BotEngine Instance;
    friend void DoCronThread(void);
};

}