%define KEY_SIZE 64

%define SYS_READ 0
%define SYS_WRITE 1
%define SYS_EXIT 60

%define STDIN 0
%define STDOUT 1

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

_print_woody:
    mov qword [rsp], 0x2E2E2E2E
    mov qword [rsp + 4], 0x444F4F57
    mov qword [rsp + 8], 0x2E2E2E59
    mov word [rsp + 12], 0x0A2E

    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    mov rdx, 14
    syscall

_ask_key:
    sub rsp, 10
    mov qword [rsp], 0x73736150
    mov qword [rsp + 4], 0x64726F77
    mov word [rsp + 8], 0x203A

    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, rsp
    mov rdx, 10
    syscall

_read_input_key:
    sub rsp, 64
    mov rax, SYS_READ
    mov rdi, STDIN
    mov rsi, key   ; buffer to store input
    mov rdx, 64    ; number of bytes to read
    syscall

_start_decrypt:
    mov rsi, [text_start]  ; Address of .text section
    mov rdx, [text_size]   ; Size of .text section
    mov rdi, key           ; Address of key
    mov rcx, 64            ; Size of key

; decrypt_loop:
;     mov r8b, byte [rsi]
;     mov r9b, byte [rdi]
;     xor r8b, r9b
;     mov [rsi], r8b
;     inc rsi
;     inc rdi
;     dec rdx
;     test rdx, rdx
;     jz decrypt_done
;     cmp rdi, key + 64
;     jne decrypt_loop
;     mov rdi, key           ; Reset key pointer
;     jmp decrypt_loop

decrypt_done:
    mov rsp, rbp
    pop rbp

    pop rdx
    pop rsi
    pop rdi
    pop rax

    jmp _end

_variables:
text_start: times 8 nop    ; variable to store the start address of .text section
text_size: times 8 nop     ; variable to store the size of .text section
key: times 64 nop  ; buffer to store the key

_end:
