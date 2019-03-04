%include "macro.inc"

%define pc r15
%define w r14
%define rstack r13

section .rodata
unknown_token_message:
  db 'unknown token', 10, 0
bye_message:
  db 'bye', 10, 0

native 'drop', drop
  add rsp, 8
  jmp next

native 'swap', swap
  pop r8
  pop r9
  push r8
  push r9
  jmp next

native 'dup', dup
  pop r8
  push r8
  push r8
  jmp next

native 'rot', rot
  pop r8
  pop r9
  pop r10
  push r9
  push r8
  push r10
  jmp next

native '+', add
  pop r8
  pop r9
  add r8, r9
  push r8
  jmp next

native '*', mul
  pop rax
  pop r8
  mul r8
  push rax
  jmp next

native '/', div
  xor rdx, rdx
  pop rax
  pop r8
  div r8
  push rax
  jmp next

native '%', mod
  xor rdx, rdx
  pop rax
  pop r8
  div r8
  push rdx
  jmp next

native '-', sub
  pop r8
  pop r9
  sub r8, r9
  push r8
  jmp next

native 'inc', inc
  pop r8
  inc r8
  push r8
  jmp next

native 'dec', dec
  pop r8
  dec r8
  push r8
  jmp next

native '=', eq
  pop r8
  pop r9
  cmp r8, r9
  setz r8b
  push r8
  jmp next

native '<', less
  pop r8
  pop r9
  cmp r8, r9
  xor r8, r8
  setl r8b
  push r8
  jmp next

native 'not', not
  pop r8
  test r8, r8
  setz r8b
  push r8
  jmp next

native 'and', and
  pop r8
  pop r9
  and r8, r9
  push r8
  jmp next

native 'key', key
  call read_char
  and rax, 0x00000000000000FF
  push rax
  jmp next

native 'emit', emit
  pop rdi
  and rdi, 0x00000000000000FF
  call print_char
  jmp next

native 'count', count
  pop rdi
  call string_length
  push rax
  jmp next

native '.', dot
  pop rdi
  call print_int
  call print_newline
  jmp next

native '.S', dot_s
  mov rbx, rsp
.loop:
  cmp rbx, rbp
  jz .end
  mov rdi, [rbx]
  add rbx, 8
  call print_int
  call print_newline
  jmp .loop
.end:
  jmp next

native 'init', init
  mov rstack, rstack_start
  mov pc, main_stub
  mov rbp, rsp
  jmp next

; branch   42 ...
;        ^    ^
;        pc   relative jmp from here
native 'branch', branch
  add pc, [pc]
  add pc, 8
  jmp next

native '0branch', branch0
  pop r8
  test r8, r8
  jnz .end
  add pc, [pc]
.end:
  add pc, 8
  jmp next

native 'docol', docol
  sub rstack, 8
  mov [rstack], pc
  add w, 8
  mov pc, w
  jmp next

native ':', col
  mov rdi, input_buf
  mov rsi, input_buf_size
  call read_word ; user-def code name
  mov r10, [last_word]
  mov r11, [here]
  mov [r11], r10
  mov [last_word], r11
  add r11, 8

  push r11
  push rdx
  mov rdi, rax
  mov rsi, r11
  inc rdx
  call string_copy
  pop rdx
  pop r11
  add r11, rdx

  inc r11
  mov byte[r11], 0 ; flag=0
  inc r11
  mov qword[r11], i_docol
  add r11, 8
  mov [here], r11
  mov qword[state], 1
  jmp next

native ';', semicol, 1
  mov r11, [here]
  mov qword[r11], xt_exit
  add r11, 8
  mov [here], r11
  mov qword[state], 0
  jmp next

native 'exit', exit
  mov pc, [rstack]
  add rstack, 8
  jmp next

native 'word', word
  pop rdi
  mov rsi, input_buf_size
  call read_word
  push rdx
  jmp next

native 'prints', prints
  pop rdi
  call print_string
  jmp next

native 'bye', bye
  mov rdi, bye_message
  call print_string
  mov rax, 60
  xor rdi, rdi
  syscall

native 'inbuf', inbuf
  push qword input_buf
  jmp next

native 'find', find 
  pop r8
  mov r9, [last_word]
.loop:
  test r9, r9
  jz .not_found
  push r8
  push r9
  lea rdi, [r9 + 8]
  mov rsi, r8
  call string_equals
  pop r9
  pop r8
  test rax, rax
  jnz .found
  mov r9, [r9]
  jmp .loop
.found:
  push r9
  jmp .end
.not_found:
  push qword 0
.end:
  jmp next

native 'cfa', cfa
  pop rdi
  push rdi
  add rdi, 8
  call string_length
  pop rdi
  lea rdi, [rdi + rax + 10]
  push rdi
  jmp next

native 'last_word', last_word
  push qword[last_word]
  jmp next

native 'lit', lit
  push qword [pc]
  add pc, 8
  jmp next

native '@', fetch
  pop r8
  push qword [r8]
  jmp next

native 'c@', fetch_char
  pop r8
  xor r9, r9
  mov r9b, byte[r8]
  push r9 
  jmp next

native '!', store
  pop r8  ; data
  pop r9  ; address
  mov [r9], r8
  jmp next

native 'c!', store_char
  pop r8  ; char
  pop r9  ; address
  mov byte[r9], r8b
  jmp next

native 'mem', mem
  push user_mem
  jmp next

native 'number', number
  pop rdi
  call parse_int
  push rax
  push rdx
  jmp next

native 'here', here
  push qword[here]
  jmp next

native 'execute', execute
  pop r8
  mov w, r8
  jmp [r8]

native '>r', datastack2rstack
  pop r8
  sub rstack, 8
  mov [rstack], r8
  jmp next

native 'r>', rstack2datastack
  mov r8, [rstack]
  add rstack, 8
  push rsp
  jmp next

native 'r@', rstack2datastack_copy
  mov r8, [rstack]
  push r8
  jmp next

native 'syscall', syscall
  pop r9
  pop r8
  pop r10
  pop rdx
  pop rsi
  pop rdi 
  pop rax
  syscall
  push rax
  jmp next

native 'state', state
  push state
  jmp next

; byte code of interpreter
colon 'interpreter', interpreter
xt_loop:
  dq xt_lit, state
  dq xt_fetch
  branch0 interpret


compile:
  dq xt_inbuf
  dq xt_word
  dq xt_not
  branch0 .parse_token
  dq xt_bye
.parse_token:
  dq xt_inbuf
  dq xt_find
  dq xt_dup
  branch0 .maybe_number
  dq xt_cfa
  dq xt_dup
  dq xt_dec
  dq xt_fetch_char
  dq xt_lit, 1
  dq xt_eq  ; check if immediate is set
  branch0 .store_byte_code
  dq xt_execute
  branch xt_loop

.store_byte_code:
  dq xt_here
  dq xt_swap
  dq xt_store
  dq xt_here
  dq xt_lit, 8
  dq xt_add
  dq xt_lit, here
  dq xt_swap
  dq xt_store
  branch xt_loop

.maybe_number:
  dq xt_drop
  dq xt_inbuf
  dq xt_number
  branch0 .unknown_token
  dq xt_lit, 8
  dq xt_here
  dq xt_sub
  dq xt_fetch
  dq xt_lit, xt_branch
  dq xt_eq
  branch0 .store_branch_offset
  dq xt_lit, 8
  dq xt_here
  dq xt_sub
  dq xt_fetch
  dq xt_lit, xt_branch0
  dq xt_eq
  branch0 .store_branch_offset
  dq xt_here
  dq xt_lit, xt_lit
  dq xt_store
  dq xt_lit, here
  dq xt_here
  dq xt_lit, 8
  dq xt_add
  dq xt_store
  dq xt_here
  dq xt_swap
  dq xt_store
  dq xt_lit, here
  dq xt_here
  dq xt_lit, 8
  dq xt_add
  dq xt_store
  branch xt_loop

.store_branch_offset:
  dq xt_here
  dq xt_swap
  dq xt_store
  dq xt_lit, here
  dq xt_here
  dq xt_lit, 8
  dq xt_add
  dq xt_store
  branch xt_loop

.unknown_token:
  dq xt_drop  ; drop result of xt_number
  dq xt_lit, unknown_token_message
  dq xt_prints
  branch xt_loop


interpret:
  dq xt_inbuf
  dq xt_word
  dq xt_not
  branch0 .parse_token
  dq xt_bye
.parse_token:
  dq xt_inbuf
  dq xt_find
  dq xt_dup
  branch0 .maybe_number
  dq xt_cfa
  dq xt_execute
  branch xt_loop
.maybe_number:
  dq xt_drop
  dq xt_inbuf
  dq xt_number
  branch0 .unknown_token
  branch xt_loop
.unknown_token:
  dq xt_drop  ; drop result of xt_number
  dq xt_lit, unknown_token_message
  dq xt_prints
  branch xt_loop

