#ifndef INIT_H
#define INIT_H
#include "ldapi.h"
#include <winsock2.h>
typedef int (*WSAStartupProc)(
		WORD      wVersionRequired,
		LPWSADATA lpWSAData
		);

typedef SOCKET (WSAAPI *WSASocketAProc)(
		int                 af,
		int                 type,
		int                 protocol,
		LPWSAPROTOCOL_INFOA lpProtocolInfo,
		GROUP               g,
		DWORD               dwFlags
		);

typedef int (WSAAPI *WSAConnectProc)(
		SOCKET				s,
		const struct sockaddr *name,
		int 				namelen,
		LPWSABUF 			lpCallerData,
		LPWSABUF			lpCalleeData,
		LPQOS				lpSQOS,
		LPQOS				lpGQOS
		);

typedef int (WSAAPI *WSARecvProc)(
		SOCKET                             s,
		LPWSABUF                           lpBuffers,
		DWORD                              dwBufferCount,
		LPDWORD                            lpNumberOfBytesRecvd,
		LPDWORD                            lpFlags,
		LPWSAOVERLAPPED                    lpOverlapped,
		LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
		);

typedef int (*WSAGetLastErrorProc)();

typedef struct __ENV
{
	LoadLibraryAProc	pfnLoadLibraryA;
	GetProcAddressProc	pfnGetProcAddress;
	WSAStartupProc  	pfnWSAStartup;
	WSASocketAProc  	pfnWSASocketA;
	WSAConnectProc		pfnWSAConnect;
	WSARecvProc     	pfnWSARecv;
} ENV, *LPENV;

void Initialize(LPENV pEnv);

#endif /* INIT_H */

