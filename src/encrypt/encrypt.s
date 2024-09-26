section .text

global _start

_start:
    push rax
    push rdi
    push rsi
    push rdx

    push rbp
    mov rbp, rsp
    sub rsp, 0x20

    mov qword [rsp], 0x2E2E2E2E
    mov qword [rsp + 4], 0x444F4F57
    mov qword [rsp + 8], 0x2E2E2E59
    mov word [rsp + 12], 0x0A2E

    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    mov rdx, 14
    syscall

    mov rsp, rbp
    pop rbp

    pop rdx
    pop rsi
    pop rdi
    pop rax
