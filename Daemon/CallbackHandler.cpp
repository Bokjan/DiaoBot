#include <map>
#include <memory>
#include "Log.hpp"
#include "BotEngineImpl.hpp"
#include "DaemonHeader.hpp"
#include "TfcConfigCodec.hpp"
#include "WeworkMessage.hpp"
#include "CallbackMessage.hpp"
#include "../ThirdParty/WXBizMsgCrypt/WXBizMsgCrypt.hpp"

namespace DiaoBot
{

using std::string;

static Tencent::WXBizMsgCrypt *Cryptor; // no need to be deleted

void SetupCryptor(void)
{
    Cryptor = new Tencent::WXBizMsgCrypt(MainConf.GetKV("root", "token"), MainConf.GetKV("root", "encoding_aes_key"), "");
}

static void DoVerifyURL(SimpleHttpMessage *msg, string *ret)
{
    ret->clear();
    std::map<string, string> qsmap;
    for (auto &i : SplitString(msg->QueryString, '&'))
    {
        auto pos = i.find_first_of('=');
        if (pos == string::npos)
            continue;
        qsmap.insert(std::make_pair(string(i.c_str(), pos), string(i.c_str() + pos + 1, i.length() - pos - 1)));
    }
    auto echostr = urlDecode(qsmap["echostr"].c_str());
    int retcode = Cryptor->VerifyURL(qsmap["msg_signature"], qsmap["timestamp"], qsmap["nonce"], echostr, *ret);
    free(echostr);
    if (retcode != 0)
        LOG("VerifyURL failed, retcode=%d", retcode);
}


void OnMessageReceived(SimpleHttpMessage *msg, string *ret)
{
    ret->clear();
    std::map<string, string> qsmap;
    for (auto &i : SplitString(msg->QueryString, '&'))
    {
        auto pos = i.find_first_of('=');
        if (pos == string::npos)
            continue;
        qsmap.insert(std::make_pair(string(i.c_str(), pos), string(i.c_str() + pos + 1, i.length() - pos - 1)));
    }
    string decrypted;
    int retcode = Cryptor->DecryptMsg(qsmap["msg_signature"], qsmap["timestamp"], qsmap["nonce"], msg->Body, decrypted);
    if (retcode != 0)
    {
        *ret = "Decrypt failed!";
        // LOG("%s%d", "Decrypt failed, retcode=", retcode);
        return;
    }
    // LOG("decrypted:\n%s", decrypted.c_str());
    tinyxml2::XMLDocument doc;
    if (doc.Parse(decrypted.c_str(), decrypted.length()) != 0)
    {
        *ret = "XML parse failed!";
        return;
    }
    auto root = doc.FirstChildElement("xml");
    if (root == nullptr)
    {
        *ret = "Invalid request";
        return;
    }
    auto msgtype = root->FirstChildElement("MsgType");
    if (msgtype == nullptr)
    {
        *ret = "Invalid request";
        return;
    }
    std::shared_ptr<CallbackMessage> cbmsg;
    if (strcmp(msgtype->GetText(), "text") == 0)
        cbmsg = std::make_shared<CallbackTextMessage>(root);
    else if (strcmp(msgtype->GetText(), "event") == 0)
        cbmsg = std::make_shared<CallbackEventMessage>(root);
    else if (strcmp(msgtype->GetText(), "attachment") == 0)
        cbmsg = std::make_shared<CallbackAttachmentMessage>(root);
    if (cbmsg == nullptr || cbmsg->HasExtractError)
    {
        *ret = "Invalid request";
        return;
    }
    // Select a reply maker
    std::shared_ptr<WeworkMessage> rmsg = nullptr;
    for (auto &i : GetEngineImpl()->ReplyMakerList)
    {
        auto logicptr = i.CreateReplyable();
        if (!logicptr->WillReply(cbmsg))
            continue;
        rmsg = logicptr->Reply(cbmsg);
        break;
    }
    if (rmsg == nullptr)
    {
        ret->clear();
        return;
    }
    Cryptor->EncryptMsg(rmsg->GetXml(), qsmap["timestamp"], qsmap["nonce"], *ret);
}

string* CallbackHandler(SimpleHttpMessage *msg)
{
    auto ret = new string;
    if (msg->Method == "GET") // Verify URL
    {
        DoVerifyURL(msg, ret);
    }
    else if (msg->Method == "POST") // Callback 
    {
        OnMessageReceived(msg, ret);
    }
    return ret;
}

}