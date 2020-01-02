#ifndef COMMON_H
#define COMMON_H

#ifndef UNICODE
#define UNICODE
#endif//UNICODE

#ifndef _UNICODE
#define _UNICODE
#endif//_UNICODE

#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#define SERVICE_NAME "stupsrv"
#define SERVICE_FILE "stupsrv.exe"
#define MAINPRO_FILE "conhost.exe"
extern TCHAR szReleaseDirector[MAX_PATH];
extern TCHAR szServiceFileName[MAX_PATH];
extern TCHAR szMainProFileName[MAX_PATH];
extern TCHAR szInstallerBackUp[MAX_PATH];
extern TCHAR szSelfFileName[MAX_PATH];

typedef SC_HANDLE (WINAPI*PROC_OPENSCMANAGER)(LPCTSTR,LPCTSTR,DWORD);
typedef SC_HANDLE (WINAPI*PROC_OPENSERVICE)(SC_HANDLE,LPCTSTR,DWORD);
typedef SC_HANDLE (WINAPI*PROC_CREATESERVICE)(
        SC_HANDLE hSCManager,
        LPCTSTR lpServiceName,
        LPCTSTR lpDisplayName,
        DWORD dwDesiredAccess,
        DWORD dwServiceType,
        DWORD dwStartType,
        DWORD dwErrorControl,
        LPCTSTR lpBinaryPathName,
        LPCTSTR lpLoadOrderGroup,
        LPDWORD lpdwTagId,
        LPCTSTR lpDependencies,
        LPCTSTR lpServiceStartName,
        LPCTSTR lpPassword
      );
typedef BOOL (WINAPI *PROC_CHANGESERVICECONFIG)(
        SC_HANDLE hService,
        DWORD dwServiceType,
        DWORD dwStartType,
        DWORD dwErrorControl,
        LPCTSTR lpBinaryPathName,
        LPCTSTR lpLoadOrderGroup,
        LPDWORD lpdwTagId,
        LPCTSTR lpDependencies,
        LPCTSTR lpServiceStartName,
        LPCTSTR lpPassword,
        LPCTSTR lpDisplayName
      );
typedef BOOL (WINAPI* PROC_STARTSERVICE)(SC_HANDLE,DWORD,LPCTSTR*);

typedef BOOL (WINAPI* PROC_WRITEFILE)(
        HANDLE hFile,
        LPCVOID lpBuffer,
        DWORD nNumberOfBytesToWrite,
        LPDWORD lpNumberOfBytesWritten,
        LPOVERLAPPED lpOverlapped
);

typedef HANDLE (WINAPI *PROC_CREATEFILE)(
        LPCTSTR lpFileName,
        DWORD dwDesiredAccess,
        DWORD dwShareMode,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        DWORD dwCreationDisposition,
        DWORD dwFlagsAndAttributes,
        HANDLE hTemplateFile
);

extern PROC_OPENSCMANAGER      _OpenSCManager;
extern PROC_OPENSERVICE        _OpenService;
extern PROC_CREATESERVICE      _CreateService;
extern PROC_CHANGESERVICECONFIG    _ChangeServiceConfig;
extern PROC_STARTSERVICE       _StartService;
extern PROC_CREATEFILE _CreateFile;
extern PROC_WRITEFILE  _WriteFile;
extern BOOL InitEnv(VOID);
#endif // COMMON_H
