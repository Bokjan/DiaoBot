#pragma once

#include "Runnable.hpp"

namespace Sample
{

class CronSample : public DiaoBot::Runnable
{
public:
    void Run(void) override;
};

}