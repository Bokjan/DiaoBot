#include <map>
#include <dlfcn.h>
#include <cstring>
#include "BotEngine.hpp"
#include "DylibManager.hpp"
#include "TfcConfigCodec.hpp"
#include "../ThirdParty/md5/src/md5.h"

namespace DiaoBot
{

DylibManager DylibManager::Instance;

struct DylibInfo
{
    unsigned int    LibID;
    void *          Handle;
    unsigned char   Md5Data[16];
};

class DylibManagerImpl
{
    friend class DylibManager;
    unsigned int AI_LibID;
    std::map<string, DylibInfo> Map;
    DylibManagerImpl(void):
        AI_LibID(1) { }
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

unsigned int DylibManager::GetLibraryID(const string &name)
{
    auto f = PImpl->Map.find(name);
    if (f == PImpl->Map.end())
        return 0;
    return f->second.LibID;
}

void DylibManager::LoadLibrary(const string &path, const string &name)
{
    if (PImpl->Map.find(name) != PImpl->Map.end())
        throw std::runtime_error(name + " library already loaded");
    dlerror(); // clear
    void *handle = dlopen(path.c_str(), RTLD_NOW);
    auto error = dlerror();
    if (error != nullptr)
        throw std::runtime_error(error);
    MD5 somd5(ReadFile(path.c_str()));
    DylibInfo di;
    di.Handle = handle;
    di.LibID = PImpl->AI_LibID++;
    memcpy(di.Md5Data, somd5.getDigest(), 16);
    PImpl->Map.insert(std::make_pair(name, di));
}

void DylibManager::UnloadLibrary(const string &name)
{
    auto f = PImpl->Map.find(name);
    if (f == PImpl->Map.end())
        throw std::runtime_error(name + " library not found");
    BotEngine::GetInstance().DestroyCronJobs(f->second.LibID);
    dlerror(); // clear
    dlclose(f->second.Handle);
    auto error = dlerror();
    if (error != nullptr)
        throw std::runtime_error(error);
}

void DylibManager::ReloadLibrary(const string &path, const string &name)
{
    auto f = PImpl->Map.find(name);
    if (f == PImpl->Map.end())
        goto LOAD;
    this->UnloadLibrary(name);
LOAD:
    this->LoadLibrary(path, name);
}

void * DylibManager::GetSymbolImpl(const string &name, const string &symbol)
{
    auto f = PImpl->Map.find(name);
    if (f == PImpl->Map.end())
        return nullptr;
    return dlsym(f->second.Handle, symbol.c_str());
}

}