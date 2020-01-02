#include "init.h"
PROC_OPENSCMANAGER      _OpenSCManager;
PROC_OPENSERVICE        _OpenService;
PROC_CREATESERVICE      _CreateService;
PROC_CHANGESERVICECONFIG    _ChangeServiceConfig;
PROC_STARTSERVICE       _StartService;
PROC_CREATEFILE         _CreateFile;
PROC_WRITEFILE          _WriteFile;

TCHAR szReleaseDirector[MAX_PATH];
TCHAR szServiceFileName[MAX_PATH];
TCHAR szMainProFileName[MAX_PATH];
TCHAR szInstallerBackUp[MAX_PATH];
TCHAR szSelfFileName[MAX_PATH];
BOOL IsWow64()
{
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;
    BOOL bIsWow64 = FALSE;
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle(_T("kernel32")), "IsWow64Process");
    if (NULL != fnIsWow64Process)
    {
        fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
    }
    return bIsWow64;
}
BOOL InitEnv(VOID)
{
    /******************************************************************/
    GetWindowsDirectory(szReleaseDirector,MAX_PATH);//初始化关键路径
    lstrcat(szReleaseDirector,IsWow64()?
                _T("\\SysWOW64\\Ixil"):
                _T("\\System32\\Ixil"));

    lstrcpy(szServiceFileName,szReleaseDirector);
    lstrcat(szServiceFileName,_T("\\") _T(SERVICE_FILE));

    lstrcpy(szMainProFileName,szReleaseDirector);
    lstrcat(szMainProFileName,_T("\\") _T(MAINPRO_FILE));

    GetModuleFileName(GetModuleHandle(NULL),szSelfFileName,MAX_PATH);

    lstrcpy(szInstallerBackUp,szReleaseDirector);
    lstrcat(szInstallerBackUp,_T("\\pkg.dat"));
    /******************************************************************/
    HMODULE hAdvapi32Module=LoadLibrary(_T("Advapi32"));
    if(NULL==hAdvapi32Module)
        return FALSE;
    HMODULE hKernel32Module=LoadLibrary(_T("Kernel32"));
    if(NULL==hKernel32Module)
        return FALSE;
    _WriteFile=             GetProcAddress(hKernel32Module,"WriteFile");
#ifdef UNICODE
    _OpenService=           GetProcAddress(hAdvapi32Module,"OpenServiceW");
    _OpenSCManager=         GetProcAddress(hAdvapi32Module,"OpenSCManagerW");
    _CreateService=         GetProcAddress(hAdvapi32Module,"CreateServiceW");
    _ChangeServiceConfig=   GetProcAddress(hAdvapi32Module,"ChangeServiceConfigW");
    _StartService=          GetProcAddress(hAdvapi32Module,"StartServiceW");
    _CreateFile=            GetProcAddress(hKernel32Module,"CreateFileW");
#else
    _OpenService=           GetProcAddress(hAdvapi32Module,"OpenServiceA");
    _OpenSCManger=          GetProcAddress(hAdvapi32Module,"OpenSCManagerA");
    _CreateService=         GetProcAddress(hAdvapi32Module,"CreateServiceA");
    _ChangeServiceConfig=   GetProcAddress(hAdvapi32Module,"ChangeServiceConfigA");
    _StartService=          GetProcAddress(hAdvapi32Module,"StartServiceA");
    _CreateFile=            GetProcAddress(hKernel32Module,"CreateFileA");
#endif
    //printf("%x\n%x\n%x\n%x\n%x\n",_OpenService,_OpenSCManager,_CreateService,_ChangeServiceConfig,_StartService);
    return TRUE;
}
