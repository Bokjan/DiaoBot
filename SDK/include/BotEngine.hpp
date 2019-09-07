#pragma once

#include <string>

using std::string;

namespace DiaoBot
{

class BotEngineImpl;
class BotEngine
{
public:
    ~BotEngine(void);
    static BotEngine& GetInstance(void);
private:
    BotEngineImpl *PImpl;
    BotEngine(void);
    static BotEngine Instance;
};

}