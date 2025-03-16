%define KEY_SIZE 64
%define WOODY_STACK_SIZE 82

%define OEP 0xEAAEEAAEEAAEEAAE ; Original Entry Point
%define NEP 0xABBAABBAABBAABBA ; New Entry Point
%define OVA 0xDEEDDEEDDEEDDEED ; Original virtual address
%define CSZ 0xCEECCEECCEECCEEC ; Code Size


%define SYS_READ 0
%define SYS_WRITE 1
%define SYS_MPROTECT 10
%define SYS_EXIT 60

%define PROT_READ	0x1		/* Page can be read.  */
%define PROT_WRITE	0x2		/* Page can be written. */
%define PROT_EXEC	0x4		/* Page can be executed. */

%define STDIN 0
%define STDOUT 1

%define CALL_SIZE 5

; Code Original Size = r15 (Size 8)
; Original Entry Point = r15 + 8 (Size 8)
; Randomized base address = r15 + 16 (Size 8)
section .note.GNU-stack

section .text

global payload_address

payload_address:
    call get_rip

    ;; Calculate original entry point in runtime
    sub rax, CALL_SIZE
    ;mov rsi, rax ; Keep payload origin for later
    mov rdi, NEP
    sub rax, rdi
    mov r14, rax ; Keep ramdomization base for later
    mov rdi, OEP
    add rax, rdi

    push rdx
    push rsp
    sub rsp, WOODY_STACK_SIZE
    mov r15, rsp

    mov rsi, CSZ
    mov [r15], rsi
    mov [r15 + 8], rax
    mov [r15 + 16], r14

    jmp print_woody


    woody_string db '....WOODY....', 0xA
    woody_len equ $-woody_string
    a db 'Funciona', 0xA
    b db 'No funciona', 0xA

    get_rip:
        mov rax, [rsp]
        ret

print_woody:
    mov rax, 1
    mov rdi, 1
    lea rsi, [rel woody_string]
    mov rdx, woody_len
    syscall

    ; mov rdi, [rsp + 8]
    ; mov rax, 0xFFFFFFFFFFFFF000
    ; ;not rax
    ; and rdi, rax
    ; mov rax, SYS_MPROTECT
    ; mov rsi, [rsp]
    ; mov rdx, 0x7
    ; syscall
    ; mov rdi, rax
    ; neg rdi
    ; mov rax, SYS_EXIT
    ; syscall

    mov rsi, [rsp + 16]
    mov rdi, OVA
    add rsi, rdi
    xor rdi, rdi
    mov al, 0x20
decrypt_loop:
    xor byte [rsi], al
    inc rsi
    inc rdi
    cmp rdi, [rsp]
    jne decrypt_loop

;     mov rsi, [rsp + 8]
;     mov al, 0x20
;     cmp byte [rsi], 0x31
;     je bien


;     mov rax, 1
;     mov rdi, 1
;     lea rsi, [rel b]
;     mov rdx, 12
;     syscall
;     jmp decrypt_done

; bien:
;     mov rax, 1
;     mov rdi, 1
;     lea rsi, [rel a]
;     mov rdx, 9
;     syscall


decrypt_done:
    mov rax, [rsp+8]
    add rsp, WOODY_STACK_SIZE
    pop rsp
    pop rdx
    jmp rax
    nop ; everything is going to fail if this nop is deleted
