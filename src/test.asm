section .text

extern ExitProcess

global _start
global mainCRTStartup

_start:
	mov rbp, rsp
	push rbp
	call mainCRTStartup
	mov rcx, rax
	call ExitProcess


mainCRTStartup:
	ret
