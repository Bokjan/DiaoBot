#pragma once

#include <string>
#include <memory>

using std::string;

namespace DiaoBot
{

class WeworkMessage
{
public:
    const static string ALL;
    const static string ALL_GROUP;
    const static string ALL_SUBSCRIBER;

    virtual ~WeworkMessage(void) = 0;
    virtual string GetJson(void) = 0;

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

    string GetJson(void) override;

private:
    std::shared_ptr<WeworkTextMessageImpl> PImpl;
};



}