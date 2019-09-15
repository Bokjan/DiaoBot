#pragma once

#include <string>
#include "Runnable.hpp"
#include "Replyable.hpp"
#include "CronConfig.hpp"

using std::string;

struct SimpleHttpMessage;

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

    void RegisterCronJob(unsigned int libid, const CronConfig &config, RunnablePtr runnable);
    void DestroyCronJobs(RunnablePtr runnable); // 销毁全部对应Runnable的任务
    void DestroyCronJobs(unsigned int libid); // 销毁某动态库下的任务

    void RegisterReplyMaker(unsigned int libid, ReplyablePtr replyable);
    void DestroyReplyMakers(unsigned int libid); // 销毁某动态库下的回调处理器

    HttpResponse HttpGetRequest(const string &url, bool proxy = true);
    HttpResponse HttpPostRequest(const string &url, const string &body, bool proxy = true);

private:
    BotEngineImpl *PImpl;
    BotEngine(void);
    static BotEngine Instance;
    friend BotEngineImpl* GetEngineImpl(void);
};

}