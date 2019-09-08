#include "Log.hpp"
#include "DaemonHeader.hpp"
#include "../ThirdParty/mongoose/mongoose.h"

namespace DiaoBot
{

void DoHttpThread(void)
{
    LOG("%s", "HttpThread started");
    
    LOG("%s", "HttpThread terminated");
}

}