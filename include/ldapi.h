#ifndef DYNLD_H
#define DYNLD_H
#include <windef.h>
typedef struct { CHAR data[12]; } STRING12;
typedef HMODULE WINAPI (*LoadLibraryAProc)(LPCSTR lpFileName);
typedef FARPROC WINAPI (*GetProcAddressProc)(HMODULE hModule, LPCSTR  lpProcName);
LPVOID FindInKernel32(STRING12 sFunction);
extern LoadLibraryAProc Find_LoadLibraryA();
extern GetProcAddressProc Find_GetProcAddress();
#endif
