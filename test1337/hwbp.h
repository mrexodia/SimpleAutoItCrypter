#pragma once

#include <windows.h>

enum HWBP_MODE
{
    MODE_DISABLED = 0, //00
    MODE_LOCAL = 1, //01
    MODE_GLOBAL = 2 //10
};

enum HWBP_TYPE
{
    TYPE_EXECUTE = 0, //00
    TYPE_WRITE = 1, //01
    TYPE_READWRITE = 3 //11
};

enum HWBP_SIZE
{
    SIZE_1 = 0, //00
    SIZE_2 = 1, //01
    SIZE_8 = 2, //10
    SIZE_4 = 3 //11
};

#pragma pack(1)
struct DR7
{
    BYTE HWBP_MODE[4];
    BYTE HWBP_TYPE[4];
    BYTE HWBP_SIZE[4];
};

#define BITSET(a,x) (a|=1<<x)
#define BITCLEAR(a,x) (a&=~(1<<x))
#define BITTOGGLE(a,x) (a^=1<<x)
#define BITGET(a,x) (a&(1<<x))

inline ULONG_PTR dr7uint(DR7* dr7)
{
    ULONG_PTR ret = 0;
    if(BITGET(dr7->HWBP_MODE[0], 0))
        BITSET(ret, 0);
    if(BITGET(dr7->HWBP_MODE[0], 1))
        BITSET(ret, 1);
    if(BITGET(dr7->HWBP_MODE[1], 0))
        BITSET(ret, 2);
    if(BITGET(dr7->HWBP_MODE[1], 1))
        BITSET(ret, 3);
    if(BITGET(dr7->HWBP_MODE[2], 0))
        BITSET(ret, 4);
    if(BITGET(dr7->HWBP_MODE[2], 1))
        BITSET(ret, 5);
    if(BITGET(dr7->HWBP_MODE[3], 0))
        BITSET(ret, 6);
    if(BITGET(dr7->HWBP_MODE[3], 1))
        BITSET(ret, 7);
    if(BITGET(dr7->HWBP_TYPE[0], 0))
        BITSET(ret, 16);
    if(BITGET(dr7->HWBP_TYPE[0], 1))
        BITSET(ret, 17);
    if(BITGET(dr7->HWBP_SIZE[0], 0))
        BITSET(ret, 18);
    if(BITGET(dr7->HWBP_SIZE[0], 1))
        BITSET(ret, 19);
    if(BITGET(dr7->HWBP_TYPE[1], 0))
        BITSET(ret, 20);
    if(BITGET(dr7->HWBP_TYPE[1], 1))
        BITSET(ret, 21);
    if(BITGET(dr7->HWBP_SIZE[1], 0))
        BITSET(ret, 22);
    if(BITGET(dr7->HWBP_SIZE[1], 1))
        BITSET(ret, 23);
    if(BITGET(dr7->HWBP_TYPE[2], 0))
        BITSET(ret, 24);
    if(BITGET(dr7->HWBP_TYPE[2], 1))
        BITSET(ret, 25);
    if(BITGET(dr7->HWBP_SIZE[2], 0))
        BITSET(ret, 26);
    if(BITGET(dr7->HWBP_SIZE[2], 1))
        BITSET(ret, 27);
    if(BITGET(dr7->HWBP_TYPE[3], 0))
        BITSET(ret, 28);
    if(BITGET(dr7->HWBP_TYPE[3], 1))
        BITSET(ret, 29);
    if(BITGET(dr7->HWBP_SIZE[3], 0))
        BITSET(ret, 30);
    if(BITGET(dr7->HWBP_SIZE[3], 1))
        BITSET(ret, 31);
    return ret;
}

inline void uintdr7(ULONG_PTR dr7, DR7* ret)
{
    memset(ret, 0, sizeof(DR7));
    if(BITGET(dr7, 0))
        BITSET(ret->HWBP_MODE[0], 0);
    if(BITGET(dr7, 1))
        BITSET(ret->HWBP_MODE[0], 1);
    if(BITGET(dr7, 2))
        BITSET(ret->HWBP_MODE[1], 0);
    if(BITGET(dr7, 3))
        BITSET(ret->HWBP_MODE[1], 1);
    if(BITGET(dr7, 4))
        BITSET(ret->HWBP_MODE[2], 0);
    if(BITGET(dr7, 5))
        BITSET(ret->HWBP_MODE[2], 1);
    if(BITGET(dr7, 6))
        BITSET(ret->HWBP_MODE[3], 0);
    if(BITGET(dr7, 7))
        BITSET(ret->HWBP_MODE[3], 1);
    if(BITGET(dr7, 16))
        BITSET(ret->HWBP_TYPE[0], 0);
    if(BITGET(dr7, 17))
        BITSET(ret->HWBP_TYPE[0], 1);
    if(BITGET(dr7, 18))
        BITSET(ret->HWBP_SIZE[0], 0);
    if(BITGET(dr7, 19))
        BITSET(ret->HWBP_SIZE[0], 1);
    if(BITGET(dr7, 20))
        BITSET(ret->HWBP_TYPE[1], 0);
    if(BITGET(dr7, 21))
        BITSET(ret->HWBP_TYPE[1], 1);
    if(BITGET(dr7, 22))
        BITSET(ret->HWBP_SIZE[1], 0);
    if(BITGET(dr7, 23))
        BITSET(ret->HWBP_SIZE[1], 1);
    if(BITGET(dr7, 24))
        BITSET(ret->HWBP_TYPE[2], 0);
    if(BITGET(dr7, 25))
        BITSET(ret->HWBP_TYPE[2], 1);
    if(BITGET(dr7, 26))
        BITSET(ret->HWBP_SIZE[2], 0);
    if(BITGET(dr7, 27))
        BITSET(ret->HWBP_SIZE[2], 1);
    if(BITGET(dr7, 28))
        BITSET(ret->HWBP_TYPE[3], 0);
    if(BITGET(dr7, 29))
        BITSET(ret->HWBP_TYPE[3], 1);
    if(BITGET(dr7, 30))
        BITSET(ret->HWBP_SIZE[3], 0);
    if(BITGET(dr7, 31))
        BITSET(ret->HWBP_SIZE[3], 1);
}

inline bool hwbpSet(HANDLE hThread, ULONG_PTR hwbpAddr, int hwbpIndex, HWBP_TYPE hwbpType, HWBP_SIZE hwbpSize)
{
    switch(hwbpSize)
    {
    case SIZE_1:
        break;
    case SIZE_2:
        if((hwbpAddr % 2) != 0)
            return false;
        break;
    case SIZE_4:
        if((hwbpAddr % 4) != 0)
            return false;
        break;
    case SIZE_8:
        if((hwbpAddr % 8) != 0)
            return false;
        break;
    default:
        return false;
    }

    CONTEXT DBGContext;
    memset(&DBGContext, 0, sizeof(CONTEXT));
    DBGContext.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if(!GetThreadContext(hThread, &DBGContext))
        return false;

    DR7 dr7;
    uintdr7((ULONG_PTR)DBGContext.Dr7, &dr7);

    dr7.HWBP_MODE[hwbpIndex] = MODE_LOCAL;
    dr7.HWBP_SIZE[hwbpIndex] = hwbpSize;
    dr7.HWBP_TYPE[hwbpIndex] = hwbpType;

    DBGContext.Dr7 = (ULONG_PTR)dr7uint(&dr7);

    switch(hwbpIndex)
    {
    case 0:
        DBGContext.Dr0 = (ULONG_PTR)hwbpAddr;
        break;
    case 1:
        DBGContext.Dr1 = (ULONG_PTR)hwbpAddr;
        break;
    case 2:
        DBGContext.Dr2 = (ULONG_PTR)hwbpAddr;
        break;
    case 3:
        DBGContext.Dr3 = (ULONG_PTR)hwbpAddr;
        break;
    default:
        return false;
    }

    if(!SetThreadContext(hThread, &DBGContext))
        return false;
    return true;
}

inline bool hwbpDel(HANDLE hThread, int hwbpIndex)
{
    CONTEXT DBGContext;
    memset(&DBGContext, 0, sizeof(CONTEXT));
    DBGContext.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if(!GetThreadContext(hThread, &DBGContext))
        return false;

    DR7 dr7;
    uintdr7((ULONG_PTR)DBGContext.Dr7, &dr7);

    dr7.HWBP_MODE[hwbpIndex] = MODE_DISABLED;
    dr7.HWBP_SIZE[hwbpIndex] = SIZE_1;
    dr7.HWBP_TYPE[hwbpIndex] = TYPE_EXECUTE;

    DBGContext.Dr7 = (ULONG_PTR)dr7uint(&dr7);

    switch(hwbpIndex)
    {
    case 0:
        DBGContext.Dr0 = 0;
        break;
    case 1:
        DBGContext.Dr1 = 0;
        break;
    case 2:
        DBGContext.Dr2 = 0;
        break;
    case 3:
        DBGContext.Dr3 = 0;
        break;
    default:
        return false;
    }

    if(!SetThreadContext(hThread, &DBGContext))
        return false;

    return true;
}