/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>


struct list_head readyqueue;

struct list_head freequeue;

struct task_struct *idle_task;

struct task_struct *task1;

void task_switch(union task_union *new);

void inner_task_switch(union task_union *new);
/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}


extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
	struct list_head *l = list_first(&freequeue);
	list_del(l);
	idle_task = list_head_to_task_struct(l);
	union task_union *uidle = (union task_union *)idle_task;

	idle_task->PID=0;
	//idle_task->quantum = DEFAULT_QUANTUM;
	allocate_DIR(idle_task);

	uidle->stack[KERNEL_STACK_SIZE-1] = &cpu_idle;
	uidle->stack[KERNEL_STACK_SIZE-2] = 0;

	idle_task->espReg = &(idle_union->stack[KERNEL_STACK_SIZE-2]);

}

void init_task1(void)
{
	struct list_head *l = list_first(&freequeue);
	list_del(l);
	task1 = list_head_to_task_struct(l);
	union task_union *utask1 = (union task_union *)task1;

	task1->PID=1;
	//task1->quantum= DEFAULT_QUANTUM;
	allocate_DIR(task1);
	set_user_pages(task1);

	tss.esp0 = &(task1_union->stack[KERNEL_STACK_SIZE]);
	set_cr3(task1->dir_pages_baseAddr);

}


void init_freequeue(){
	INIT_LIST_HEAD(&freequeue);
	for(int i=0; i < NR_TASKS; i++){
		task[i].task.PID=-1;
		list_add_tail(&(task[i].task.list), &freequeue);
	}

}

void init_readyqueue(){
	INIT_LIST_HEAD(&readyqueue);
}

void init_sched(){
	init_freequeue();
	init_readyqueue();
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

