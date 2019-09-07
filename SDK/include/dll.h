#pragma once

namespace DiaoBot
{
    class BotEngine;
}

extern "C" 
{

const char * DB_DllName(void);
void         DB_BindEnginePtr(DiaoBot::BotEngine*);

} 
