#include "Job.hpp"
#include "Log.hpp"
#include "WeworkMessage.hpp"

namespace Sample
{

void CronSample::Run(void)
{
    // DiaoBot::WeworkTextMessage wtm;
    // wtm.SetChatID(DiaoBot::WeworkTextMessage::ALL);
    // wtm.SetContent("Hello!");

    // DiaoBot::WeworkNewsMessage wnm;
    // wnm.AddArticle(DiaoBot::WeworkNewsMessage::Article("标题", "描述", "http://tiyan.oa.com", ""));
    // LOG("%s", wnm.GetJson().c_str());

    DiaoBot::WeworkMarkdownMessage wmm;
    wmm.SetContent("Hello!");
    auto p = wmm.AddAttachment();
    p->CallbackID = "test_callback_id";
    auto b = p->AddButtonAction();
    b->Name = "callback_fallthrough_name";
    b->Text = "Test";
    b->Value = "callback_fallthrough_value";
    LOG("%s", wmm.GetJson().c_str());
}

}