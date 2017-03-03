#include <windows.h>
#include "encrypt.h"
#include "hwbp.h"

static ULONG_PTR hwidAddr = 0;
static wchar_t tempPath[MAX_PATH] = L"";
static int tempPathLen;

static LONG CALLBACK VectoredHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
    static bool bRestoreHardwareBreakpoint = false;
    static ULONG_PTR dr7backup = 0;
    DWORD ExceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
    if(ExceptionCode == DBG_PRINTEXCEPTION_C)
        return EXCEPTION_CONTINUE_SEARCH;
    if(ExceptionCode == EXCEPTION_SINGLE_STEP)
    {
        if(bRestoreHardwareBreakpoint)
        {
            bRestoreHardwareBreakpoint = false;
            ExceptionInfo->ContextRecord->Dr7 = dr7backup;
            return EXCEPTION_CONTINUE_EXECUTION;
        }
        if(ULONG_PTR(ExceptionInfo->ExceptionRecord->ExceptionAddress) == hwidAddr)
        {
            auto arg = *(wchar_t**)(ExceptionInfo->ContextRecord->Esp + 4);
            if(!memcmp(arg, tempPath, tempPathLen * sizeof(wchar_t)))
                *arg = L'\0';
            else
            {
                dr7backup = ExceptionInfo->ContextRecord->Dr7;
                ExceptionInfo->ContextRecord->Dr7 = 0;
                bRestoreHardwareBreakpoint = true;
                ExceptionInfo->ContextRecord->EFlags |= 0x100; //TRAP_FLAG
            }
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

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
    if(!memcmp(lpName, L"SCRIPT", sizeof(L"SCRIPT")) && !decrypted)
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

        tempPathLen = GetTempPathW(MAX_PATH, tempPath);
        AddVectoredExceptionHandler(1, VectoredHandler);
        hwidAddr = ULONG_PTR(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW"));
        hwbpSet(GetCurrentThread(), hwidAddr, 0, TYPE_EXECUTE, SIZE_1);
    }
    return TRUE;
}