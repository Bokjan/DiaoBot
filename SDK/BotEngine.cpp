#include "BotEngine.hpp"

namespace DiaoBot
{

BotEngine BotEngine::Instance;

class BotEngineImpl
{
    friend class BotEngine;
};

BotEngine::BotEngine(void)
{
    PImpl = new BotEngineImpl;
}

BotEngine::~BotEngine(void)
{
    delete PImpl;
}

BotEngine& BotEngine::GetInstance(void)
{
    return BotEngine::Instance;
}

}