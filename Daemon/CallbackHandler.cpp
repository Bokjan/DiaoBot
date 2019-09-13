#include <map>
#include "Log.hpp"
#include "DaemonHeader.hpp"
#include "TfcConfigCodec.hpp"
#include "../ThirdParty/WXBizMsgCrypt/WXBizMsgCrypt.hpp"

namespace DiaoBot
{

using std::string;

Tencent::WXBizMsgCrypt *Cryptor; // won't deconstruct

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
    delete echostr;
    if (retcode != 0)
        LOG("VerifyURL failed, retcode=%d", retcode);
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
        
    }
    return ret;
}

}