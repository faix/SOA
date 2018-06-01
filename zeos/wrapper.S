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
	jge correct
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
	cmpl $0, %eax
        jge correct1
        xor %eax, %eax
        movl %eax, errno
        movl $-1, %eax
        mov %ebp, %esp
correct1:
	pop %ebp
	ret

ENTRY(getpid)
	push %ebp
	movl %esp, %ebp
	movl $20, %eax
	int $0x80
        cmpl $0, %eax
        jge correct2
        xor %eax, %eax
        movl %eax, errno
        movl $-1, %eax
        mov %ebp, %esp
correct2:
	pop %ebp
	ret

ENTRY(fork)
	push %ebp
	movl %esp, %ebp
	movl $2, %eax
	int $0x80
	cmpl $0, %eax
        jge correct3
        xor %eax, %eax
        movl %eax, errno
        movl $-1, %eax
        mov %ebp, %esp
correct3:
        pop %ebp
        ret

ENTRY(exit)
	push %ebp
	movl %esp, %ebp
	movl $1, %eax
	int $0x80
	cmpl $0, %eax
        jge correct4
        xor %eax, %eax
        movl %eax, errno
        movl $-1, %eax
        mov %ebp, %esp
correct4:
        pop %ebp
        ret

ENTRY(get_stats)
	push %ebp
        movl %esp, %ebp
        pushl %ebx
        pushl %ecx
	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl $35, %eax
	int $0x80
	cmpl $0, %eax
	jge correct5
	xor %eax, %eax
	movl %eax, errno
	movl $-1, %eax
correct5:
	popl %ecx
	popl %ebx
	pop %ebp
	ret 
