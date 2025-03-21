; void	encrypt_xor(uint8_t *buffer, uint64_t const size, uint64_t const key_size, uint8_t const key[key_size])
; buffer (rdi) 
; size (rsi)
; key_size (rdx)
; key (rcx)

global encrypt_xor

encrypt_xor:
    push rsp
    mov rax, 0
    mov r8, 0
    _loop:
        cmp rax, rsi
        je _finish

        mov bl, byte [rcx + r8]
        xor byte [rdi], bl
        inc rdi
        inc rax
        inc r8

        cmp r8, rdx
        jne _loop

        _restart_key_counter:
        xor r8, r8

        jmp _loop

_finish:
    pop rsp
    ret
