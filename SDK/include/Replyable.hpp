#pragma once

#include <functional>
#include <memory>

namespace DiaoBot {

class WeworkMessage;
class CallbackMessage;

class Replyable {
 public:
  virtual bool WillReply(const std::shared_ptr<CallbackMessage> &) = 0;
  virtual std::shared_ptr<WeworkMessage> Reply(std::shared_ptr<CallbackMessage>) = 0;
  virtual ~Replyable(void) = default;
};

using ReplyablePtr = std::shared_ptr<Replyable>;
using CreateReplyableFunc = std::function<ReplyablePtr(void)>;

}  // namespace DiaoBot
