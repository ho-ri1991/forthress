global string_length
global print_string
global print_char
global print_newline
global print_uint
global print_int
global parse_uint
global parse_int
global read_char
global read_word
global string_copy
global string_equals

section .data
newline_char: db 10
decimal_asciis: db '0123456789'
sign_char: db '-'

section .text

string_length: ; uint64_t string_length(const char* str)
    xor rax, rax
.iterate:
    cmp byte[rdi + rax], 0
    je .end
    inc rax
    jmp .iterate
.end:
    ret

print_string: ; void print_string(const char* str)
    push rdi
    call string_length
    pop rdi
    mov rdx, rax
    mov rax, 1
    mov rsi, rdi
    mov rdi, 1
    syscall
    ret


print_char: ; void print_char(char c)
    push rbp
    mov rbp, rsp
    push rdi
    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    mov rdx, 1
    syscall
    mov rsp, rbp
    pop rbp
    ret

print_newline:
    mov rax, 1
    mov rdi, 1
    mov rsi, newline_char
    mov rdx, 1
    syscall
    ret


print_uint: ; void print_uint(uint64_t)
    push rbp
    mov rbp, rsp
    mov rax, rdi
    mov r8, 10
    xor rdx, rdx
    push 0
.loop:
    mov rdx, 0
    div r8
    mov dl, [decimal_asciis + rdx]
    dec rsp
    mov [rsp], dl
    test rax, rax
    jnz .loop
    mov rdi, rsp
    call print_string
    mov rsp, rbp
    pop rbp
    ret



print_int: ; void print_int(int64_t)
    push rbp
    mov rbp, rsp

    test rdi, rdi
    jns .next
    push rdi
    mov dil, byte[sign_char]
    call print_char
    pop rdi
    neg rdi
.next:
    call print_uint

    mov rsp, rbp
    pop rbp
    ret

string_equals: ; int string_equals(const char* s1, const char* s2)
.cond:
    cmp byte[rdi], 0
    jne .loop
    cmp byte[rsi], 0
    je .end
.loop:
    mov al, byte[rdi]
    mov cl, byte[rsi]
    inc rdi
    inc rsi
    cmp al, cl
    je .cond
    mov rax, 0
    ret
.end:
    mov rax, 1
    ret


read_char: ; uint64_t read_char()
    push rbp
    mov rbp, rsp
    dec rsp
    mov rax, 0     ; read syscall
    mov rdi, 0     ; stdin
    lea rsi, [rsp] ; buffer
    mov rdx, 1     ; size
    syscall
    test rax, rax
    jz .end
    xor rax, rax
    mov al, byte[rsp]
.end:
    mov rsp, rbp
    pop rbp
    ret 

read_word: ; (char*, uint64_t) read_word(char* buf, uint64_t size)
    push rbp
    mov rbp, rsp
    push rbx
    push rdi
    push rsi
    mov r8, rdi
    mov r9, rsi
    xor r10, r10
.ignoreloop:
    mov rdx, 1   ; size 
    mov rsi, r8  ; buffer
    mov rax, 0   ; read syscall
    mov rdi, 0   ; stdin
    syscall
    test rax, rax
    jz .end
    mov bl, byte[r8]
    cmp bl, 0x20
    je .ignoreloop
    cmp bl, 0x09
    je .ignoreloop
    cmp bl, 0x0A
    je .ignoreloop
    cmp bl, 0x0D
    dec r9
    inc r8
.readloop:
    test r9, r9
    je .errorend
    mov rdx, 1   ; size 
    mov rsi, r8  ; buffer
    mov rax, 0   ; read syscall
    mov rdi, 0   ; stdin
    syscall
    test rax, rax
    jz .end
    mov bl, byte[r8]
    cmp bl, 0x20
    je .end
    cmp bl, 0x09
    je .end
    cmp bl, 0x0A
    je .end
    cmp bl, 0x0D
    je .end
    dec r9
    inc r8
    jmp .readloop
.end:
    pop rsi
    pop rdi
    pop rbx
    mov byte[r8], 0x00
    mov rax, rdi
    sub rsi, r9
    mov rdx, rsi
    mov rsp, rbp
    pop rbp
    ret
.errorend:
    pop rsi
    pop rdi
    pop rbx
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

; rdi points to a string
; returns rax: number, rdx : length
parse_uint:
    xor rcx, rcx
    xor rax, rax
    mov r9, 10
.loop:
    xor r8, r8
    mov r8b, byte[rdi]
    cmp r8b, 0x30
    js .end
    cmp r8b, 0x40
    jns .end
    mul r9
    sub r8, 0x30
    add rax, r8
    inc rcx
    inc rdi
    jmp .loop
.end:
    mov rdx, rcx
    ret

; rdi points to a string
; returns rax: number, rdx : length
parse_int:
    xor r8, r8
    cmp byte[rdi], 0x2D ; sign
    jnz .parseuint
    mov r8, 1
    inc rdi
.parseuint:
    push r8
    call parse_uint
    pop r8
    test r8, r8
    jz .end
    inc rdx
    neg rax
.end:
    ret

string_copy: ; int string_copy(const char* src, char* dest, uint64_t size)
    mov r8, rsi
    mov rax, 0
.loop:
    test rdx, rdx
    jz .end
    mov r9b, byte[rdi]
    mov byte[rsi], r9b
    inc rsi
    inc rdi
    dec rdx
    test r9b, r9b
    jnz .loop
    mov rax, r8
.end:
    ret

