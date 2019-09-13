#include "DaemonHeader.hpp"

namespace DiaoBot
{

using std::string;

std::string* CallbackHandler(SimpleHttpMessage *msg)
{
    auto ret = new string;
    if (msg->Method == "GET") // Verify URL
    {

    }
    else if (msg->Method == "POST") // Callback 
    {
        
    }
    return ret;
}

}