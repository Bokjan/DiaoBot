#pragma once

#include <string>

using std::string;

namespace tinyxml2
{

class XMLElement;

}

namespace DiaoBot
{

struct FromUser
{
    string UserId;
    string Name;
    string Alias;
};

class CallbackMessage
{
public:
    CallbackMessage(void) = delete;
    CallbackMessage(tinyxml2::XMLElement *xml);
    virtual ~CallbackMessage(void) = 0;

    enum ChatTypeEnum
    {
        UNKNOWN_CHAT_TYPE,
        SINGLE,
        GROUP
    };

    enum MsgTypeEnum
    {
        UNKNOWN_MSG_TYPE,
        TEXT,
        EVENT,
        ATTACHMENT
    };

    ChatTypeEnum    ChatType;
    MsgTypeEnum     MsgType;
    string          WebhookUrl;
    string          ChatId;
    string          GetChatInfoUrl;
    string          MsgId;
    FromUser        From;
    bool            HasExtractError;
};

class CallbackTextMessage : public CallbackMessage
{
public:
    CallbackTextMessage(void) = delete;
    CallbackTextMessage(tinyxml2::XMLElement *xml);
    
    string Content;
};

class CallbackEventMessage : public CallbackMessage
{
public:
    CallbackEventMessage(void) = delete;
    CallbackEventMessage(tinyxml2::XMLElement *xml);
    
    enum EventTypeEnum
    {
        UNKNOWN_EVENT,
        ADD_TO_CHAT,
        DELETE_FROM_CHAT,
        ENTER_CHAT
    };

    EventTypeEnum   EventType;
    string          AppVersion;
};

class CallbackAttachmentMessage : public CallbackMessage
{
public:
    CallbackAttachmentMessage(void) = delete;
    CallbackAttachmentMessage(tinyxml2::XMLElement *xml);
    
    string CallbackId;
    string ActionName;
    string ActionValue;
};

}