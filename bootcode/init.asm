struc ENV
	.pfnLoadLibraryA    resd 1
	.pfnGetProcAddress  resd 1
	.pfnWSAStartup      resd 1
	.pfnWSASocketA      resd 1
	.pfnWSAConnect      resd 1
	.pfnWSARecv         resd 1
endstruc

global _Initialize
extern _Find_LoadLibraryA
extern _Find_GetProcAddress

section .text
_Initialize:
	push ebp
	mov ebp, esp
	call pusheip
strtab:
	sWs2_32_dll db "Ws2_32.dll", 0
	sWSAStartup db "WSAStartup", 0
	sWSASocketA db "WSASocketA", 0
	sWSAConnect db "WSAConnect", 0
	sWSARecv    db "WSARecv", 0
initfunc: ;(al = stroffset, bl = resoffset)
	;shl eax, 24
	;shr eax, 24
	;shl ebx, 24
	;shr ebx, 24
	;and eax, 0FFH
	;and ebx, 0FFH
	movzx eax, al
	movzx ebx, bl
	sub esp, 8 ;param for getprocaddress
	
	mov ecx, [ebp - 12]
	mov [esp], ecx ;hModule
	
	mov edx, [ebp - 4]
	add edx, eax ;edx = pRes->xx
	
	mov [esp+4], eax ;pStr
	
	mov edx, [ebp - 8]
	call edx
	
	mov ecx, [ebp + 8]
	mov [ecx + ebx], eax
	ret

pusheip:
	call _Find_GetProcAddress
	push eax ;push pGetProcAddress
	mov ebx, [ebp + 8]
	mov [ebx + ENV.pfnGetProcAddress], eax
	; pRes->pfnGetProcAddress = pGetProcAddress

	call _Find_LoadLibraryA
	; eax = pLoadLibraryA
	mov ebx, [ebp + 8]
	mov [ebx + ENV.pfnLoadLibraryA], eax
	; pRes->pfnLoadLibraryA = pLoadLibraryA
	
	mov edx, [ebp - 4]
	lea ebx, [edx + sWs2_32_dll - strtab]
	push ebx 
	call eax; LoadLibrary
	push eax ;push hModule
	
	mov al, sWSAStartup - strtab
	mov bl, ENV.pfnWSAStartup
	call initfunc

	mov al, sWSASocketA - strtab
	mov bl, ENV.pfnWSASocketA
	call initfunc

	mov al, sWSAConnect - strtab
	mov bl, ENV.pfnWSAConnect
	call initfunc

	mov al, sWSARecv - strtab
	mov bl, ENV.pfnWSARecv
	call initfunc

	mov esp, ebp
	pop ebp
	ret

