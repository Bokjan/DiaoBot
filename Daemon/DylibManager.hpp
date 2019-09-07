#pragma once

#include <string>
#include <stdexcept>

using std::string;

namespace DiaoBot
{

class DylibManagerImpl;
class DylibManager
{
public:
    ~DylibManager(void);
    static DylibManager& GetInstance(void);

    void   LoadLibrary(const string &path, const string &name) throw (std::runtime_error);
    void   UnloadLibrary(const string &name) throw (std::runtime_error);
    void * GetSymbolImpl(const string &name, const string &symbol);
    template <typename T = void*>
    inline T GetSymbol(const string &name, const string &symbol)
    {
        return reinterpret_cast<T>(this->GetSymbolImpl(name, symbol));
    }
private:
    DylibManagerImpl *PImpl;
    DylibManager(void);
    static DylibManager Instance;
};

}