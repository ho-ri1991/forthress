%include "lib.inc"
%include "words.inc"

global _start

section .bss
resq 1023
rstack_start: resq 1
input_buf: resb input_buf_size
user_dict: resq 65536
user_mem: resq 65536
state: resq 1

section .data
last_word: dq _last_word
here: dq user_dict

section .rodata
main_stub: dq xt_interpreter

section .text

next:
  mov w, [pc]
  add pc, 8
  jmp [w]

_start:
  jmp i_init
