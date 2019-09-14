#include <vector>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include "BotEngine.hpp"
#include "WeworkMessage.hpp"
#include "../ThirdParty/md5/src/md5.h"
#include "../ThirdParty/cpp-base64/base64.h"
#include "../ThirdParty/tinyxml2/tinyxml2.h"

namespace DiaoBot
{

const string WeworkMessage::ALL             = "@all";
const string WeworkMessage::ALL_GROUP       = "@all_group";
const string WeworkMessage::ALL_SUBSCRIBER  = "@all_subscriber";

WeworkMessage::~WeworkMessage(void) { }

string WeworkMessage::GetXml(void) const 
{
    return string();
}

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

class WeworkNewsMessageImpl
{
    friend class WeworkNewsMessage;
    std::vector<WeworkNewsMessage::Article> Articles;
};

class WeworkMarkdownMessageImpl
{
    friend class WeworkMarkdownMessage;
    string Content;
    std::vector<std::shared_ptr<WeworkMarkdownMessage::Attachment>> AttachmentPtrs;
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

// tinyxml2 is shit.
string WeworkTextMessage::GetXml(void) const
{
    tinyxml2::XMLDocument doc;
    auto root = doc.NewElement("xml");
    doc.InsertFirstChild(root);
    auto msgtype = doc.NewElement("MsgType");
    msgtype->SetText("text");
    root->InsertFirstChild(msgtype);
    auto text = doc.NewElement("Text");
    text->SetName("Text");
    root->InsertEndChild(text);
    auto content = doc.NewElement("Content");
    auto content_cdata = doc.NewText(this->PImpl->Content.c_str());
    content_cdata->SetCData(true);
    content->InsertFirstChild(content_cdata);
    text->InsertEndChild(content);
    if (!PImpl->MentionedList.empty())
    {
        auto list = doc.NewElement("MentionedList");
        for (const auto &i : PImpl->MentionedList)
        {
            auto item = doc.NewElement("Item");
            auto item_cdata = doc.NewText(i.c_str());
            item_cdata->SetCData(true);
            item->InsertEndChild(item_cdata);
            list->InsertEndChild(item);
        }
        root->InsertEndChild(list);
    }
    if (!PImpl->MentionedMobileList.empty())
    {
        auto list = doc.NewElement("MentionedMobileList");
        for (const auto &i : PImpl->MentionedMobileList)
        {
            auto item = doc.NewElement("Item");
            auto item_cdata = doc.NewText(i.c_str());
            item_cdata->SetCData(true);
            item->InsertEndChild(item_cdata);
            list->InsertEndChild(item);
        }
        root->InsertEndChild(list);
    }
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    return string(printer.CStr(), printer.CStrSize());
}

string WeworkTextMessage::GetJson(void) const
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    w.StartObject();
    if (!this->ChatID.empty())
    {
        w.Key("chatid");
        w.String(this->ChatID.c_str());
    }
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

WeworkNewsMessage::WeworkNewsMessage(void)
{
    PImpl = std::make_shared<WeworkNewsMessageImpl>();
}

WeworkNewsMessage::Article* WeworkNewsMessage::AddArticle(void)
{
    if (PImpl->Articles.size() >= 8)
        return nullptr;
    PImpl->Articles.emplace_back(Article());
    return &(PImpl->Articles.back());
}

bool WeworkNewsMessage::AddArticle(const Article &article)
{
    if (PImpl->Articles.size() >= 8)
        return false;
    PImpl->Articles.emplace_back(article);
    return true;
}

string WeworkNewsMessage::GetJson(void) const
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    w.StartObject();
    if (!this->ChatID.empty())
    {
        w.Key("chatid");
        w.String(this->ChatID.c_str());
    }
    w.Key("msgtype");
    w.String("news");
    w.Key("news");
    w.StartObject();
    w.Key("articles");
    w.StartArray();
    for (const auto &i : PImpl->Articles)
    {
        w.StartObject();
        w.Key("title");
        w.String(i.Title.c_str());
        if (!i.Description.empty())
        {
            w.Key("description");
            w.String(i.Description.c_str());
        }
        w.Key("url");
        w.String(i.URL.c_str());
        if (!i.PictureURL.empty())
        {
            w.Key("picurl");
            w.String(i.PictureURL.c_str());
        }
        w.EndObject();
    }
    w.EndArray();
    w.EndObject();
    w.EndObject();
    return string(sb.GetString(), sb.GetSize());
}

WeworkMarkdownMessage::ButtonAction::ButtonAction(void)
{
    Type = "button";
    TextColor = "2EAB49";
    BorderColor = "2EAB49";
}

struct WeworkMarkdownMessage::AttachmentImpl
{
    std::vector<std::shared_ptr<WeworkMarkdownMessage::Action>> ActionPtrs;
};

WeworkMarkdownMessage::Attachment::Attachment(void)
{
    PImpl = std::make_shared<WeworkMarkdownMessage::AttachmentImpl>();
}

std::shared_ptr<WeworkMarkdownMessage::ButtonAction> WeworkMarkdownMessage::Attachment::AddButtonAction(void)
{
    auto ptr = std::make_shared<WeworkMarkdownMessage::ButtonAction>();
    PImpl->ActionPtrs.emplace_back(ptr);
    return ptr;
}

void WeworkMarkdownMessage::ButtonAction::WriteJsonObject(void *_w) const
{
    auto w = static_cast<rapidjson::Writer<rapidjson::StringBuffer>*>(_w);
    w->StartObject();
    w->Key("type");
    w->String(Type.c_str());
    w->Key("name");
    w->String(Name.c_str());
    w->Key("text");
    w->String(Text.c_str());
    w->Key("border_color");
    w->String(BorderColor.c_str());
    w->Key("text_color");
    w->String(TextColor.c_str());
    w->Key("value");
    w->String(Value.c_str());
    if (!ReplaceText.empty())
    {
        w->Key("replace_text");
        w->String(ReplaceText.c_str());
    }
    w->EndObject();
}

WeworkMarkdownMessage::WeworkMarkdownMessage(void)
{
    PImpl = std::make_shared<WeworkMarkdownMessageImpl>();
}

bool WeworkMarkdownMessage::SetContent(const string &c)
{
    if (c.length() > 2048)
        return false;
    PImpl->Content = c;
    return true;
}

std::shared_ptr<WeworkMarkdownMessage::Attachment> WeworkMarkdownMessage::AddAttachment(void)
{
    auto ptr = std::make_shared<WeworkMarkdownMessage::Attachment>();
    PImpl->AttachmentPtrs.emplace_back(ptr);
    return ptr;
}

string WeworkMarkdownMessage::GetXml(void) const
{
    tinyxml2::XMLDocument doc;
    auto root = doc.NewElement("xml");
    doc.InsertFirstChild(root);
    auto msgtype = doc.NewElement("MsgType");
    msgtype->SetText("markdown");
    root->InsertFirstChild(msgtype);
    auto markdown = doc.NewElement("Markdown");
    markdown->SetName("Markdown");
    root->InsertEndChild(markdown);
    auto content = doc.NewElement("Content");
    auto content_cdata = doc.NewText(this->PImpl->Content.c_str());
    content_cdata->SetCData(true);
    content->InsertFirstChild(content_cdata);
    markdown->InsertEndChild(content);
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    return string(printer.CStr(), printer.CStrSize());
}

string WeworkMarkdownMessage::GetJson(void) const
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    w.StartObject();
    if (!this->ChatID.empty())
    {
        w.Key("chatid");
        w.String(this->ChatID.c_str());
    }
    w.Key("msgtype");
    w.String("markdown");
    w.Key("content");
    w.String(PImpl->Content.c_str());
    w.Key("attachments");
    w.StartArray();
    for (const auto &i : PImpl->AttachmentPtrs)
    {
        w.Key("callback_id");
        w.String(i->CallbackID.c_str());
        w.Key("actions");
        w.StartArray();
        for (const auto &p : i->PImpl->ActionPtrs)
        {
            p->WriteJsonObject(&w);
        }
        w.EndArray();
    }
    w.EndArray();
    w.EndObject();
    return string(sb.GetString(), sb.GetSize());
}

void WeworkImageMessage::SetRawImage(const string &buf)
{
    RawImageBuffer = buf;
}

string WeworkImageMessage::GetJson(void) const
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    w.StartObject();
    if (!this->ChatID.empty())
    {
        w.Key("chatid");
        w.String(this->ChatID.c_str());
    }
    w.Key("msgtype");
    w.String("image");
    w.Key("image");
    w.StartObject();
    w.Key("base64");
    w.String(base64_encode(reinterpret_cast<const unsigned char *>(RawImageBuffer.data()), RawImageBuffer.length()).c_str());
    w.Key("md5");
    w.String(MD5(RawImageBuffer).toStr().c_str());
    w.EndObject();
    w.EndObject();
    return string(sb.GetString(), sb.GetSize());
}

void WeworkImageMessage::SetHttpImage(const string &url, bool proxy)
{
    this->SetRawImage(BotEngine::GetInstance().HttpGetRequest(url, proxy).BodyData);
}

}