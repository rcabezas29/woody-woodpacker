%define KEY_SIZE 64
%define WOODY_STACK_SIZE 82

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
    mov qword r14, 0x4242424242424242

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
    lea rsi, [r15 + 18]   ; buffer to store input
    mov rdx, KEY_SIZE    ; number of bytes to read
    syscall

_get_text_section_addr:
    call .delta
    .delta:
        pop rbp
        sub rbp, .delta
    mov rax, r14
    lea rsi, [rbp + _start] ; Save in rsi .text address
    sub rsi, rax ; (- TEXT SIZE)

_start_decrypt:
    mov rdx, r14   ; Size of .text section
    lea rdi, [r15 + 18]           ; Address of key

_decrypt_loop:
    mov r8b, byte [rsi]
    mov r9b, byte [rdi]
    xor r8b, r9b
    mov [rsi], r8b
    inc rsi
    inc rdi
    dec rdx
    test rdx, rdx
    jz decrypt_done
    cmp rdi, [r15 + 18 + 64]
    jne _decrypt_loop
    lea rdi, [r15 + 18]           ; Reset key pointer
    jmp _decrypt_loop

decrypt_done:
    add rsp, WOODY_STACK_SIZE
    pop rsp
    pop rdx

_end:
