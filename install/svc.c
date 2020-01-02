/*
VOID WINAPI ServiceMain(DWORD  dwNumServicesArgs, PSTR *ppcServiceArgVectors);
VOID RunInSession(TCHAR *szModuleFile,DWORD dwSessionId);
VOID WINAPI ServiceHandler(DWORD szControlCode,
                    DWORD dwEventType,
                    LPVOID lpEventData,
                    LPVOID lpContext);
*/

static int InstallService(LPCSTR szServiceName, LPCSTR szDisplayName, LPCSTR szFileName) {
    SC_HANDLE hService, hSCM;
    hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(hSCM == NULL)
        return 1;

    hService=OpenServiceA(hSCM, szServiceName, GENERIC_ALL);
    if(hService) {
		Log("change config\n");
        if(ChangeServiceConfigA(hService,
                SERVICE_WIN32_OWN_PROCESS,
                SERVICE_AUTO_START,
                0,
                szFileName,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL) == 0) {
			Log("ChangeServiceConfigA error %d\n", GetLastError());
			return 1;
		}
    } else {
		Log("create service\n");
        hService = CreateServiceA(hSCM,
                szServiceName,
                szDisplayName,
                SERVICE_ALL_ACCESS,
                SERVICE_WIN32_OWN_PROCESS,
                SERVICE_AUTO_START,
                SERVICE_ERROR_NORMAL,
                szFileName,
                NULL,
                NULL,
                TEXT(""),
                NULL,
                NULL);
    }

    CloseServiceHandle(hSCM);

    if(hService==NULL) return 1;

    CloseServiceHandle(hService);

    return 0;
}


int CheckAndRestoreService() {
	CHAR szModuleFileName[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL),szModuleFileName,sizeof szModuleFileName);
    return InstallService(SERVICE_NAME,"ServiceName",szModuleFileName);
}
