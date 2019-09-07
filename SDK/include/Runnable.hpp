#pragma once

namespace DiaoBot
{

class Runnable
{
public:
    virtual ~Runnable(void) = 0;
    virtual void Run(void) = 0;
};

}