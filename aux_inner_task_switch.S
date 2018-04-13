#include <asm.h>

ENTRY(aux_inner_task_switch)
	movl 4(%esp), %eax
	movl %ebp, (%eax)
	movl 8(%esp), %esp
	popl %ebp
	ret
