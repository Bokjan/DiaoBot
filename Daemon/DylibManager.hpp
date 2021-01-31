#pragma once

#include <stdexcept>
#include <string>

using std::string;

namespace DiaoBot {

class DylibManagerImpl;
class DylibManager {
 public:
  ~DylibManager(void);
  static DylibManager &GetInstance(void);

  unsigned int GetLibraryID(const string &name);
  void LoadLibrary(const string &path, const string &name);
  void ReloadLibrary(const string &path, const string &name);
  void UnloadLibrary(const string &name);
  void *GetSymbolImpl(const string &name, const string &symbol);
  template <typename T = void *>
  inline T GetSymbol(const string &name, const string &symbol) {
    return reinterpret_cast<T>(this->GetSymbolImpl(name, symbol));
  }

 private:
  DylibManagerImpl *PImpl;
  DylibManager(void);
  static DylibManager Instance;
};

}  // namespace DiaoBot