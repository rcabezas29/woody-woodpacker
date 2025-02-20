%define KEY_SIZE 64
%define WOODY_STACK_SIZE 78

%define SYS_READ 0
%define SYS_WRITE 1
%define SYS_EXIT 60

%define STDIN 0
%define STDOUT 1

section .text

global _start

_start:
    push rdx
    push rsp

    sub rsp, WOODY_STACK_SIZE
    mov r15, rsp

_print_woody:
    mov qword [r15], 0x2E2E2E2E
    mov qword [r15 + 4], 0x444F4F57
    mov qword [r15 + 8], 0x2E2E2E59
    mov word [r15 + 12], 0x0A2E

    mov rax, 1
    mov rdi, 1
    mov rsi, r15
    mov rdx, 14
    syscall

_ask_key:
    mov qword [r15], 0x73736150
    mov qword [r15 + 4], 0x64726F77
    mov word [r15 + 8], 0x203A

    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, r15
    mov rdx, 10
    syscall

_read_input_key:
    mov rax, SYS_READ
    mov rdi, STDIN
    lea rsi, [r15 + 14]   ; buffer to store input
    mov rdx, 64    ; number of bytes to read
    syscall

_get_text_section_addr:
    call .delta
    .delta:
        pop rbp
        sub rbp, .delta
    lea rsi, [rbp + _start] ; (- TEXT SIZE) Save in rsi .text address

; _start_decrypt:
;     mov rsi, [text_start]  ; Address of .text section
;     mov rdx, [text_size]   ; Size of .text section
;     mov rdi, key           ; Address of key
;     mov rcx, 64            ; Size of key

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
    add rsp, WOODY_STACK_SIZE
    pop rsp
    pop rdx

_end:
