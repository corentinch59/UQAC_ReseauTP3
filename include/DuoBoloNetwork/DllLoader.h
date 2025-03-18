#pragma once

#include <string>

#include <spdlog/spdlog.h>

#if defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
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
#endif
    }

    bool IsDllLoaded() const;

private:
#if defined(_WIN32) || defined(__WIN32__)
  HMODULE mDll;
#else
#error "MacOS and Linux not yet implemented!!"
#endif
};