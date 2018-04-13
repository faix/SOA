/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <utils.h>

#define DEFAULT_QUANTUM 10


struct list_head readyqueue;

struct list_head freequeue;

struct task_struct *idle_task;

struct task_struct *task1;

void task_switch(union task_union *new);

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */

int quantum_left = 0;

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
	idle_task->quantum = DEFAULT_QUANTUM;
	allocate_DIR(idle_task);

	uidle->stack[KERNEL_STACK_SIZE-1] =(unsigned long) &cpu_idle;
	uidle->stack[KERNEL_STACK_SIZE-2] = 0;

	idle_task->espReg = (unsigned long)&(uidle->stack[KERNEL_STACK_SIZE-2]);

}

void init_task1(void)
{
	struct list_head *l = list_first(&freequeue);
	list_del(l);
	task1 = list_head_to_task_struct(l);
	union task_union *utask1 = (union task_union *)task1;

	task1->PID=1;
	task1->quantum= DEFAULT_QUANTUM;
	task1-> state = ST_RUN;
	quantum_left = task1-> quantum;
	allocate_DIR(task1);
	set_user_pages(task1);

	tss.esp0 =(DWord) &(utask1->stack[KERNEL_STACK_SIZE]);
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

void inner_task_switch(union task_union *new){
	tss.esp0 = (DWord)&(new->stack[KERNEL_STACK_SIZE]);
	set_cr3(new->task.dir_pages_baseAddr);
	aux_inner_task_switch(&(current()->espReg), new->task.espReg);
}

int get_quantum(struct task_struct *ts) {return ts-> quantum;}

void set_quantum(struct task_struct *ts, int quantum) {ts->quantum = quantum;}

void sched_next_rr(){
	struct list_head *next;
	struct task_struct *t_next;
	next = list_first(&readyqueue);

	if(next){
		list_del(next);
		t_next = list_head_to_task_struct(next);
	}
	else t_next = idle_task;
	update_st(&(current()->st.system_ticks), &(current()->st.elapsed_total_ticks));
	update_st(&(t_next->st.ready_ticks), &(t_next->st.elapsed_total_ticks));
	t_next -> state = ST_RUN;
	quantum_left =get_quantum(t_next);
	task_switch((union task_union *) t_next);
}

void update_process_state_rr(struct task_struct *t, struct list_head *dst_queue){
	if(t->state != ST_RUN) list_del(&(t->list));
	if(dst_queue != NULL){
		list_add_tail(&(t->list), dst_queue);
		if(dst_queue != &readyqueue) t-> state = ST_BLOCKED;
		else{
			update_st(&(t->st.system_ticks), &(t->st.elapsed_total_ticks));
			t-> state = ST_READY;
		}
	}
	else t->state = ST_RUN;
}

void update_sched_data_rr(){
	--quantum_left;
}

int needs_sched_rr(){
	if(quantum_left == 0 && !list_empty(&readyqueue)) return 1;
	if(quantum_left == 0) quantum_left = get_quantum(current());
	return 0;
}

void schedule(){
	update_sched_data_rr();
	if(needs_sched_rr()){
		update_process_state_rr(current(), &readyqueue);
		sched_next_rr();
	}
}
