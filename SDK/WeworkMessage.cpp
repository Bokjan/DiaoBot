#include <vector>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include "WeworkMessage.hpp"

namespace DiaoBot
{

const string WeworkMessage::ALL             = "@all";
const string WeworkMessage::ALL_GROUP       = "@all_group";
const string WeworkMessage::ALL_SUBSCRIBER  = "@all_subscriber";

WeworkMessage::~WeworkMessage(void) { }

void WeworkMessage::SetChatID(const string &chatid)
{
    this->ChatID = chatid;
}

class WeworkTextMessageImpl
{
    friend class WeworkTextMessage;
    string              Content;
    std::vector<string> MentionedList;
    std::vector<string> MentionedMobileList;
};

WeworkTextMessage::WeworkTextMessage(void)
{
    PImpl = std::make_shared<WeworkTextMessageImpl>();
}

bool WeworkTextMessage::SetContent(const string &c)
{
    if (c.length() > 2048)
        return false;
    PImpl->Content = c;
    return true;
}

void WeworkTextMessage::AddMentioned(const string &m)
{
    PImpl->MentionedList.emplace_back(m);
}

void WeworkTextMessage::AddMentionedMobile(const string &m)
{
    PImpl->MentionedMobileList.emplace_back(m);
}

string WeworkTextMessage::GetJson(void)
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    w.StartObject();
    w.Key("chatid");
    w.String(this->ChatID.c_str());
    w.Key("msgtype");
    w.String("text");
    w.Key("text");
    w.StartObject();
    w.Key("content");
    w.String(PImpl->Content.c_str());
    if (!PImpl->MentionedList.empty())
    {
        w.Key("mentioned_list");
        w.StartArray();
        for (const auto &i : PImpl->MentionedList)
            w.String(i.c_str());
        w.EndArray();
    }
    if (!PImpl->MentionedMobileList.empty())
    {
        w.Key("mentioned_mobile_list");
        w.StartArray();
        for (const auto &i : PImpl->MentionedMobileList)
            w.String(i.c_str());
        w.EndArray();
    }
    w.EndObject();
    w.EndObject();
    return string(sb.GetString(), sb.GetSize());
}

}