#pragma once

#include <string>

#include <spdlog/spdlog.h>

#if defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

class DllLoader {
public:
    ~DllLoader();

    DllLoader(const std::string &dllPath, bool appendPlatformExtension = true);

    template<typename Signature>
    Signature LoadFunction(const std::string& name) {
#if defined(_WIN32) || defined(__WIN32__)
        Signature func = (Signature) GetProcAddress(mDll, name.c_str());

        if (!func)
        {
            spdlog::error("Failed to load function: {}", name);
            return nullptr;
        }

        return func;
#else
        Signature func = (Signature) dlsym(mLib, name.c_str());
        const char* error = dlerror();

        if (!func)
        {
            spdlog::error("Failed to load function {}: ", name, error);
            return nullptr;
        }

        return func;
#endif
    }

    bool IsDllLoaded() const;

private:
#if defined(_WIN32) || defined(__WIN32__)
  HMODULE mDll;
#else
  void* mLib;
#endif
};