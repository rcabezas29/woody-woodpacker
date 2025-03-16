%define KEY_SIZE 64
%define WOODY_STACK_SIZE 0x70

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


; The argument is used for protection flags
%macro mprotect 1

    mov rsi, [r15 + 0x10]
    mov rdi, [r15 + 0x18]
    add rdi, rsi
    mov rax, 0xFFFFFFFFFFFFF000
    ;not rax
    and rdi, rax
    mov rax, SYS_MPROTECT
    mov rsi, [rsp]
    mov rdx, %1 
    syscall

%endmacro

; Code Original Size = r15 (Size 0x8)
; Original Entry Point = r15 + 0x8 (Size 0x8)
; Randomized base address = r15 + 0x10 (Size 0x8)
; Original Virtual Address = r15 + 0x18 (Size 0x8)
; Decryption Key = r15 + 0x20 (Size 0x41)

section .note.GNU-stack

section .text

global payload_address

payload_address:
    call get_rip

    ;; Calculate original entry point in runtime
    sub rax, CALL_SIZE
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
    mov [r15 + 0x8], rax
    mov [r15 + 0x10], r14
    mov rsi, OVA
    mov [r15 + 0x18], rsi

    jmp print_woody


    woody_string db '....WOODY....', 0xA
    woody_len equ $-woody_string

    ask_key_string db 'key: '
    key_string_len equ $-ask_key_string

    newline db 0xA

    get_rip:
        mov rax, [rsp]
        ret

print_woody:
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    lea rsi, [rel woody_string]
    mov rdx, woody_len
    syscall

; Ask key
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    lea rsi, [rel ask_key_string]
    mov rdx, key_string_len
    syscall


; Read key
    mov rax, SYS_READ
    mov rdi, STDIN
    lea rsi, [r15 + 0x20]
    mov rdx, KEY_SIZE + 1
    syscall

; Change memory permissions
    mprotect 0x7 ; Read, write and exec permissions

; Start decryption
    mov rdi, [r15 + 0x10] ; Load Randomized Base Address
    add rdi, [r15 + 0x18] ; Compute Runtime Virtual Address
    xor r8, r8            ; Start counter to 0
    lea r14, [r15 + 0x20] ; Load decryption key
decrypt_loop:
    mov rax, r8
    xor rdx, rdx
    mov ecx, KEY_SIZE
    div ecx
    add rdx, r14
    mov al, [rdx]
    xor byte [rdi], al

    inc rdi
    inc r8
    cmp r8, [rsp]
    jne decrypt_loop


    mprotect 0x5 ; Read and exec permissions

decrypt_done:
    mov rax, [r15 + 0x8]
    add rsp, WOODY_STACK_SIZE
    pop rsp
    pop rdx
    jmp rax
    nop ; everything is going to fail if this nop is deleted
