#include <windows.h>
#include "encrypt.h"
#include "hwbp.h"

//#pragma comment(linker, "/export:EmptyWorkingSet=PSAPI.EmptyWorkingSet")
//#pragma comment(linker, "/export:EnumDeviceDrivers=PSAPI.EnumDeviceDrivers")
//#pragma comment(linker, "/export:EnumPageFilesA=PSAPI.EnumPageFilesA")
//#pragma comment(linker, "/export:EnumPageFilesW=PSAPI.EnumPageFilesW")
//#pragma comment(linker, "/export:EnumProcesses=PSAPI.EnumProcesses")
//#pragma comment(linker, "/export:EnumProcessModules=PSAPI.EnumProcessModules")
//#pragma comment(linker, "/export:GetDeviceDriverBaseNameA=PSAPI.GetDeviceDriverBaseNameA")
//#pragma comment(linker, "/export:GetDeviceDriverBaseNameW=PSAPI.GetDeviceDriverBaseNameW")
//#pragma comment(linker, "/export:GetDeviceDriverFileNameA=PSAPI.GetDeviceDriverFileNameA")
//#pragma comment(linker, "/export:GetDeviceDriverFileNameW=PSAPI.GetDeviceDriverFileNameW")
//#pragma comment(linker, "/export:GetMappedFileNameA=PSAPI.GetMappedFileNameA")
//#pragma comment(linker, "/export:GetMappedFileNameW=PSAPI.GetMappedFileNameW")
//#pragma comment(linker, "/export:GetModuleBaseNameA=PSAPI.GetModuleBaseNameA")
//#pragma comment(linker, "/export:GetModuleBaseNameW=PSAPI.GetModuleBaseNameW")
//#pragma comment(linker, "/export:GetModuleFileNameExA=PSAPI.GetModuleFileNameExA")
//#pragma comment(linker, "/export:GetModuleFileNameExW=PSAPI.GetModuleFileNameExW")
//#pragma comment(linker, "/export:GetModuleInformation=PSAPI.GetModuleInformation")
//#pragma comment(linker, "/export:GetPerformanceInfo=PSAPI.GetPerformanceInfo")
//#pragma comment(linker, "/export:GetProcessImageFileNameA=PSAPI.GetProcessImageFileNameA")
//#pragma comment(linker, "/export:GetProcessImageFileNameW=PSAPI.GetProcessImageFileNameW")
#pragma comment(linker, "/export:GetProcessMemoryInfo=PSAPI.GetProcessMemoryInfo")
//#pragma comment(linker, "/export:GetWsChanges=PSAPI.GetWsChanges")
//#pragma comment(linker, "/export:InitializeProcessForWsWatch=PSAPI.InitializeProcessForWsWatch")
//#pragma comment(linker, "/export:QueryWorkingSet=PSAPI.QueryWorkingSet")

static ULONG_PTR hwidAddr = 0;
static wchar_t tempPath[MAX_PATH] = L"";
static int tempPathLen;

static bool match(const wchar_t* a, const wchar_t* b, size_t s)
{
    for(size_t i = 0; i < s; i++)
        if(a[i] != b[i])
            return false;
    return true;
}

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
#ifdef _WIN64
            auto arg = (wchar_t*)ExceptionInfo->ContextRecord->Rcx;
#else
            auto arg = *(wchar_t**)(ExceptionInfo->ContextRecord->Esp + 4);
#endif //_WIN64
            if(match(arg, tempPath, tempPathLen))
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

        DWORD old;
        VirtualProtect(lol, sizeof(void*), PAGE_READWRITE, &old);
        *lol = FindResourceExW_hook;
        VirtualProtect(lol, sizeof(void*), old, &old);

        tempPathLen = GetTempPathW(MAX_PATH, tempPath);
        AddVectoredExceptionHandler(1, VectoredHandler);
        hwidAddr = ULONG_PTR(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW"));
        hwbpSet(GetCurrentThread(), hwidAddr, 0, TYPE_EXECUTE, SIZE_1);
    }
    return TRUE;
}