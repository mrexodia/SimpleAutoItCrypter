#include <windows.h>
#include "encrypt.h"

extern "C"
_Ret_maybenull_
HRSRC
WINAPI
FindResourceExW_hook(
_In_opt_ HMODULE hModule,
_In_ LPCWSTR lpType,
_In_ LPCWSTR lpName,
_In_ WORD wLanguage
)
{
    static auto decrypted = false;
    auto hResInfo = FindResourceExW(hModule, lpType, lpName, wLanguage);
    if(!wcscmp(lpName, L"SCRIPT") && !decrypted)
    {
        decrypted = true;
        auto size = SizeofResource(hModule, hResInfo);
        auto data = LockResource(LoadResource(hModule, hResInfo));
        DWORD old;
        VirtualProtect(data, size, PAGE_READWRITE, &old);
        decrypt((unsigned char*)data, size);
        VirtualProtect(data, size, old, &old);
    }
    return hResInfo;
}

BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
    )
{
    if(fdwReason == DLL_PROCESS_ATTACH)
    {
        void* frex = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "FindResourceExW");
        auto lol = (void**)GetModuleHandleW(nullptr);
        while(*lol != frex)
            lol++;
        *lol = FindResourceExW_hook;
    }
    return TRUE;
}