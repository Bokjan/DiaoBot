#include <map>
#include <dlfcn.h>
#include "DylibManager.hpp"

namespace DiaoBot
{

DylibManager DylibManager::Instance;

class DylibManagerImpl
{
    friend class DylibManager;
    std::map<string, void*> Map;
};

DylibManager::DylibManager(void)
{
    PImpl = new DylibManagerImpl;
}

DylibManager::~DylibManager(void)
{
    delete PImpl;
}

DylibManager& DylibManager::GetInstance(void)
{
    return DylibManager::Instance;
}

void DylibManager::LoadLibrary(const string &path, const string &name) throw (std::runtime_error)
{
    if (PImpl->Map.find(name) != PImpl->Map.end())
        throw std::runtime_error(name + " library already loaded");
    dlerror(); // clear
    void *handle = dlopen(path.c_str(), RTLD_NOW);
    auto error = dlerror();
    if (error != nullptr)
        throw std::runtime_error(error);
    PImpl->Map.insert(std::make_pair(name, handle));
}

void DylibManager::UnloadLibrary(const string &name) throw (std::runtime_error)
{
    auto f = PImpl->Map.find(name);
    if (f == PImpl->Map.end())
        throw std::runtime_error(name + " library not found");
    dlerror(); // clear
    dlclose(f->second);
    auto error = dlerror();
    if (error != nullptr)
        throw std::runtime_error(error);
}

void * DylibManager::GetSymbolImpl(const string &name, const string &symbol)
{
    auto f = PImpl->Map.find(name);
    if (f == PImpl->Map.end())
        return nullptr;
    return dlsym(f->second, symbol.c_str());
}

}