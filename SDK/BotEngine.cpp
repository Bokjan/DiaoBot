#include <list>
#include <cstdlib>
#include <curl/curl.h>
#include <rapidjson/document.h>
#include "Log.hpp"
#include "BotEngine.hpp"
#include "BotEngineImpl.hpp"
#include "WeworkMessage.hpp"

namespace DiaoBot
{

static size_t CurlBodyWriteFunction(void *ptr, size_t size, size_t nmemb, string *data)
{
    data->append((char *)ptr, size * nmemb);
    return size * nmemb;
}

static string GetHostInUrl(const string &url)
{
    int start;
    int slash_count = 0;
    decltype(url.length()) i;
    for (i = 0; i < url.length(); ++i)
    {
        if (url[i] == '/')
            ++slash_count;
        if (slash_count == 2) // protocol
            break;
    }
    start = ++i;
    for ( ; i < url.length(); ++i)
        if (url[i] == '/')
            break;
    return string(url.c_str() + start, i - start);
}

static void CurlHttpRequest(long &response_code, string &retbody, const string &url, bool proxy = true, const char *data = nullptr, size_t data_len = 0)
{
    string env_no_proxy;
    auto curl = curl_easy_init();
    if (curl == nullptr)
    {
        LOG("%s", "Could not get a cURL handle!");
        return;
    }
    if (!proxy)
    {
        // LOG("host=%s", GetHostInUrl(url).c_str());
        string host = GetHostInUrl(url);
        auto origin_no_proxy = getenv("no_proxy");
        if (origin_no_proxy != nullptr)
            env_no_proxy = origin_no_proxy;
        if (origin_no_proxy == nullptr)
        {
            setenv("no_proxy", host.c_str(), 1);
        }
        else
        {
            string t = env_no_proxy;
            t.append(",").append(host);
            setenv("no_proxy", t.c_str(), 1);
        }
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if (data != nullptr)
    {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        if (data_len != 0)
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_len);
    }
    string header;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlBodyWriteFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &retbody);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header);
    auto retcode = curl_easy_perform(curl);
    if (retcode != CURLE_OK)
    {
        LOG("cURL request failed on: %s", url.c_str());
        goto RETURN;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
RETURN:
    if (!proxy)
    {
        if (env_no_proxy.empty())
            unsetenv("no_proxy");
        else
            setenv("no_proxy", env_no_proxy.c_str(), 1);
    }
    curl_easy_cleanup(curl);
}

BotEngine BotEngine::Instance;

BotEngine::BotEngine(void)
{
    PImpl = new BotEngineImpl;
}

BotEngine::~BotEngine(void)
{
    delete PImpl;
}

BotEngine& BotEngine::GetInstance(void)
{
    return BotEngine::Instance;
}

void BotEngine::RegisterCronJob(const CronConfig &config, Runnable *runnable)
{
    PImpl->CronJobList.push_back(CronJobPair(config, runnable));
}

void BotEngine::DestroyCronJobs(Runnable *runnable)
{
    for (auto it = PImpl->CronJobList.begin(); it != PImpl->CronJobList.end(); ++it)
    {
        if (it->Task == runnable)
        {
            PImpl->CronJobList.erase(it++); // 处理迭代器失效问题
        }
    }
    delete runnable;
}

HttpResponse BotEngine::HttpGetRequest(const string &url, bool proxy)
{
    HttpResponse ret;
    CurlHttpRequest(ret.ResponseCode, ret.BodyData, url, proxy);
    // LOG("ret.len = %lu, data:\n%s", ret.BodyData.length(), ret.BodyData.data());
    return ret;
}

HttpResponse BotEngine::HttpPostRequest(const string &url, const string &body, bool proxy)
{
    HttpResponse ret;
    CurlHttpRequest(ret.ResponseCode, ret.BodyData, url, proxy, body.data(), body.length());
    return ret;
}

void BotEngine::SetWebhookUrl(const string &url)
{
    PImpl->WebhookUrl = url;
}

int BotEngine::SendMessage(const WeworkMessage &message)
{
    auto response = this->HttpPostRequest(PImpl->WebhookUrl, message.GetJson());
    if (response.ResponseCode != 200)
        return static_cast<int>(-response.ResponseCode);
    rapidjson::Document doc;
    doc.Parse(response.BodyData.c_str());
    int ret;
    do
    {
        if (doc.HasParseError())
        {
            LOG("Cannot parse: %s", response.BodyData.c_str());
            ret = -8964;
            break;
        }
        if (!doc.HasMember("errcode"))
        {
            LOG("`errcode` not found in: %s", response.BodyData.c_str());
            ret = -8965;
            break;
        }
        ret = doc["errcode"].GetInt();
        if (ret == 0)
            break;
        if (!doc.HasMember("errmsg"))
        {
            LOG("`errmsg` not found in: %s", response.BodyData.c_str());
            ret = -8966;
            break;
        }
        LOG("Error message: %s", doc["errmsg"].GetString());
    }
    while (false);
    return ret;
}

Runnable::~Runnable(void) { }

}