#pragma once

#include <DiaoBot/Runnable.hpp>

namespace Sample
{

class CronSample : public DiaoBot::Runnable
{
public:
    void Run(void) override;
};

}