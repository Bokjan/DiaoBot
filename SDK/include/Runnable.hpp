#pragma once

#include <memory>

namespace DiaoBot {

class Runnable {
 public:
  virtual void Run(void) = 0;
  virtual ~Runnable(void) = default;
};

using RunnablePtr = std::shared_ptr<Runnable>;

}  // namespace DiaoBot
