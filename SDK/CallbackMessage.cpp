#include "CallbackMessage.hpp"
#include "../ThirdParty/tinyxml2/tinyxml2.h"
#include "Log.hpp"

namespace DiaoBot {

CallbackMessage::~CallbackMessage(void) {}

CallbackMessage::CallbackMessage(tinyxml2::XMLElement *xml)
    : ChatType(UNKNOWN_CHAT_TYPE), MsgType(UNKNOWN_MSG_TYPE), HasExtractError(true) {
  do {
    // WebhookUrl
    auto p = xml->FirstChildElement("WebhookUrl");
    if (p == nullptr) {
      break;
    }
    WebhookUrl = p->GetText();
    // ChatId
    p = xml->FirstChildElement("ChatId");
    if (p == nullptr) {
      break;
    }
    ChatId = p->GetText();
    // GetChatInfoUrl
    p = xml->FirstChildElement("GetChatInfoUrl");
    if (p != nullptr) {
      GetChatInfoUrl = p->GetText();
    }
    // MsgId
    p = xml->FirstChildElement("MsgId");
    if (p == nullptr) {
      break;
    }
    MsgId = p->GetText();
    // ChatType
    p = xml->FirstChildElement("ChatType");
    if (p == nullptr) {
      break;
    }
    if (strcmp(p->GetText(), "single") == 0) {
      ChatType = SINGLE;
    } else if (strcmp(p->GetText(), "group") == 0) {
      ChatType = GROUP;
    } else {
      break;
    }  // MsgType
    p = xml->FirstChildElement("MsgType");
    if (p == nullptr) {
      break;
    }
    if (strcmp(p->GetText(), "text") == 0) {
      MsgType = TEXT;
    } else if (strcmp(p->GetText(), "event") == 0) {
      MsgType = EVENT;
    } else if (strcmp(p->GetText(), "attachment") == 0) {
      MsgType = ATTACHMENT;
    } else {
      break;
    }  
    // From
    p = xml->FirstChildElement("From");
    auto q = p->FirstChildElement("UserId");
    if (q == nullptr) {
      break;
    }
    From.UserId = q->GetText();
    q = p->FirstChildElement("Name");
    if (q == nullptr) {
      break;
    }
    From.Name = q->GetText();
    q = p->FirstChildElement("Alias");
    if (q == nullptr) {
      break;
    }
    From.Alias = q->GetText();
    HasExtractError = false;
  } while (false);
}

CallbackTextMessage::CallbackTextMessage(tinyxml2::XMLElement *xml) : CallbackMessage(xml) {
  if (HasExtractError) {
    return;
  }
  HasExtractError = true;
  do {
    auto p = xml->FirstChildElement("Text");
    if (p == nullptr) {
      break;
    }
    p = p->FirstChildElement("Content");
    if (p == nullptr) {
      break;
    }
    Content = p->GetText();
    HasExtractError = false;
  } while (false);
}

CallbackEventMessage::CallbackEventMessage(tinyxml2::XMLElement *xml)
    : CallbackMessage(xml), EventType(UNKNOWN_EVENT) {
  if (HasExtractError) {
    return;
  }
  HasExtractError = true;
  do {
    auto p = xml->FirstChildElement("AppVersion");
    if (p == nullptr) {
      break;
    }
    AppVersion = p->GetText();
    p = xml->FirstChildElement("Event");
    if (p == nullptr) {
      break;
    }
    p = p->FirstChildElement("EventType");
    if (p == nullptr) {
      break;
    }
    string type = p->GetText();
    if (type == "add_to_chat") {
      EventType = ADD_TO_CHAT;
    } else if (type == "delete_from_chat") {
      EventType = DELETE_FROM_CHAT;
    } else if (type == "enter_chat") {
      EventType = ENTER_CHAT;
    } else {
      break;
    }
    HasExtractError = false;
  } while (false);
}

CallbackAttachmentMessage::CallbackAttachmentMessage(tinyxml2::XMLElement *xml)
    : CallbackMessage(xml) {
  if (HasExtractError) {
    return;
  }
  HasExtractError = true;
  do {
    auto r = xml->FirstChildElement("Attachment");
    if (r == nullptr) {
      break;
    }
    auto p = r->FirstChildElement("CallbackId");
    if (p == nullptr) {
      break;
    }
    CallbackId = p->GetText();
    p = r->FirstChildElement("Actions");
    if (p == nullptr) {
      break;
    }
    auto q = p->FirstChildElement("Name");
    if (q == nullptr) {
      break;
    }
    ActionName = q->GetText();
    q = p->FirstChildElement("Value");
    if (q == nullptr) {
      break;
    }
    ActionValue = q->GetText();
    HasExtractError = false;
  } while (false);
}

}  // namespace DiaoBot
