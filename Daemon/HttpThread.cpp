#include "Log.hpp"
#include "DaemonHeader.hpp"

namespace DiaoBot
{

void DoHttpThread(void)
{
    LOG("%s", "HttpThread started");
    
    LOG("%s", "HttpThread terminated");
}

}