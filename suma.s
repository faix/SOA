# 1 "suma.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "suma.S"
# 1 "include/asm.h" 1
# 2 "suma.S" 2

.globl add2; .type add2, @function; .align 0; add2:
 push %ebp
 mov %esp, %ebp
 mov 0x8(%ebp), %eax
 add 0xc(%ebp), %eax
 pop %ebp
 ret
