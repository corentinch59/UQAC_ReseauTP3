#include <DuoBoloNetwork/DllLoader.h>

#if defined(_WIN32) || defined(__WIN32__)

DllLoader::~DllLoader() {
    if (mDll)
    {
        FreeLibrary(mDll);
    }
}

DllLoader::DllLoader(const std::string &dllPath, bool appendPlatformExtension) {
    std::string finalPath = dllPath;
    if (appendPlatformExtension) finalPath.append(".dll");
    
    mDll = LoadLibrary(finalPath.c_str());

    if (!mDll)
    {
        DWORD errorCode = GetLastError();

        LPSTR errorMsg = nullptr;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&errorMsg,
            0, NULL
        );

        spdlog::error("Failed to load library {}, error: {}", finalPath, errorMsg);
    }
}

bool DllLoader::IsDllLoaded() const {
    return mDll != nullptr;
}

#else
#error "MacOS and Linux not yet implemented!!"
#endif