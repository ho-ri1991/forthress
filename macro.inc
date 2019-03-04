%define _last_word 0

%macro native 3
section .data
w_%2:
  dq _last_word
  db %1, 0
  db %3
xt_%2:
  dq i_%2
section .text
i_%2:
%define _last_word w_%2
%endmacro

%macro native 2
native %1, %2, 0
%endmacro

%macro colon 2
section .data
w_%2:
  dq _last_word
  db %1, 0
  db 0
xt_%2:
  dq i_docol
%define _last_word w_%2
%endmacro

%define input_buf_size 1024

%macro branch 1 
dq xt_branch
dq %1 - $ - 8
%endmacro

%macro branch0 1 
dq xt_branch0
dq %1 - $ - 8
%endmacro

