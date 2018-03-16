#include <asm.h>
#include <errno.h>

ENTRY(write)
	push %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %ecx
	pushl %edx
	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl 16(%ebp), %edx
	movl $4, %eax
	int $0x80
	cmpl $0, %eax
	jg correct
	xor %eax, %eax
	movl %eax, errno
	movl $-1, %eax
	mov %ebp, %esp
correct:
	popl %edx
	popl %ecx
	popl %ebx
	popl %ebp
	ret

ENTRY(gettime)
	push %ebp
        movl %esp, %ebp
        movl $10, %ebx
	int $0x80
	pop %ebp
	ret
