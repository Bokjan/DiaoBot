#pragma once

#include <string>
#include <memory>

using std::string;

constexpr bool NO_PROXY = false;

namespace DiaoBot
{

class WeworkMessage
{
public:
    const static string ALL;
    const static string ALL_GROUP;
    const static string ALL_SUBSCRIBER;

    virtual ~WeworkMessage(void) = 0;
    virtual string GetJson(void) const = 0;

    void SetChatID(const string &chatid);

protected:
    string ChatID;
};

class WeworkTextMessageImpl;
class WeworkTextMessage : public WeworkMessage
{
public:
    WeworkTextMessage(void);
    
    bool SetContent(const string &c); // Cannot exceed 2048 bytes
    void AddMentioned(const string &m);
    void AddMentionedMobile(const string &m);

    string GetJson(void) const override;

private:
    std::shared_ptr<WeworkTextMessageImpl> PImpl;
};

class WeworkNewsMessageImpl;
class WeworkNewsMessage : public WeworkMessage
{
public:
    struct Article
    {
        string Title;
        string Description;
        string URL;
        string PictureURL; // JPG, PNG

        Article(void) = default;
        Article(const char *title, const char *desc, const char *url, const char *purl):
            Title(title), Description(desc), URL(url), PictureURL(purl) { }
        Article(const string &title, const string &desc, const string &url, const string &purl):
            Title(title), Description(desc), URL(url), PictureURL(purl) { }
    };

    WeworkNewsMessage(void);

    Article*    AddArticle(void);
    bool        AddArticle(const Article &article); // Cannot exceed 8

    string GetJson(void) const override;

private:
    std::shared_ptr<WeworkNewsMessageImpl> PImpl;
};

class WeworkMarkdownMessageImpl;
class WeworkMarkdownMessage : public WeworkMessage
{
public:
    struct Action
    {
        string Type;    // auto set
        string Name;    // callback related
        string Value;   // callback related

        virtual void WriteJsonObject(void*) const = 0; // param is rapidjson::Writer<rapidjson::StringBuffer>*
    };
    struct ButtonAction : public Action
    {
        string Text;
        string TextColor;
        string BorderColor;
        string ReplaceText;

        ButtonAction(void);
        void WriteJsonObject(void*) const override;
    };
    struct AttachmentImpl;
    struct Attachment
    {
        string CallbackID;
        std::shared_ptr<AttachmentImpl> PImpl;
        Attachment(void);
        std::shared_ptr<ButtonAction> AddButtonAction(void);
    };

    WeworkMarkdownMessage(void);

    bool SetContent(const string &c);
    std::shared_ptr<Attachment> AddAttachment(void);

    string GetJson(void) const override;

private:
    std::shared_ptr<WeworkMarkdownMessageImpl> PImpl;
};

class WeworkImageMessage : public WeworkMessage
{
public:
    void SetRawImage(const string &buf);
    void SetHttpImage(const string &url, bool proxy = true);

    string GetJson(void) const override;

private:
    string RawImageBuffer;
};

}