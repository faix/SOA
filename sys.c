/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>
#include <errno.h>

#include <stats.h>

#define LECTURA 0
#define ESCRIPTURA 1

int next_pid=1000;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF;
  if (permissions!=ESCRIPTURA) return -EACCES;
  return 0;
}

int sys_ni_syscall()
{
	return -ENOSYS;
}

int sys_write(int fd, char * buffer, int size){
	char temp_buffer[size];
	int err = check_fd(fd, ESCRIPTURA);
	if(err < 0) return err;

	if(buffer == NULL || size <= 0) return -1;
	if(copy_from_user(buffer, temp_buffer, size) >= 0)
	sys_write_console(temp_buffer, size);

	return size;
}

int sys_gettime(){
	return get_zeos_ticks();
}

int sys_getpid()
{
	return current()->PID;
}

int ret_from_fork(){
	return 0;
}

int sys_fork()
{
	struct list_head *child;
	union task_union *tuchild;

	if(list_empty(&freequeue)) return -ENOMEM;

	child = list_first(&freequeue);
	list_del(child);
	tuchild = (union task_union*)list_head_to_task_struct(child);

	copy_data(current(), tuchild, sizeof(union task_union));

	allocate_DIR((struct task_struct*)tuchild);

	page_table_entry *ptchild = get_PT(&tuchild->task);

	int new_pag, i;

	for(i=0; i < NUM_PAG_DATA; i++){
		new_pag = alloc_frame();
		if(new_pag != -1) set_ss_pag(ptchild, PAG_LOG_INIT_DATA+i, new_pag);
		else{
			for(int j=0; j< i; j++){
				free_frame(get_frame(ptchild, PAG_LOG_INIT_DATA+j));
				del_ss_pag(ptchild, PAG_LOG_INIT_DATA+j);
			}
			list_add_tail(child, &freequeue);
			return -EAGAIN;
		}
	}

	page_table_entry *ptparent = get_PT(current());

	for(i = 0; i < NUM_PAG_KERNEL; i++) set_ss_pag(ptchild, i, get_frame(ptparent,  i));

	for(i = 0; i < NUM_PAG_CODE; i++) set_ss_pag(ptchild, PAG_LOG_INIT_CODE+i, get_frame(ptparent, PAG_LOG_INIT_CODE+i));

	for(i = NUM_PAG_KERNEL + NUM_PAG_CODE; i < NUM_PAG_CODE + NUM_PAG_DATA + NUM_PAG_KERNEL; i++){
		set_ss_pag(ptparent, i+NUM_PAG_DATA, get_frame(ptchild, i));
		copy_data((void*)(i<<12), (void*)((i + NUM_PAG_DATA)<<12), PAGE_SIZE);
		del_ss_pag(ptparent, i+NUM_PAG_DATA);
	}

	set_cr3(get_DIR(current()));
	tuchild->task.PID = next_pid;
	next_pid++;

	tuchild->task.espReg = &tuchild->stack[KERNEL_STACK_SIZE-19];
	tuchild->stack[KERNEL_STACK_SIZE-19]=0;
	tuchild->stack[KERNEL_STACK_SIZE-18] = (DWord)&ret_from_fork;

	tuchild->task.state =ST_READY;
	list_add_tail(&(tuchild->task.list), &readyqueue);
	return tuchild->task.PID;
}

void sys_exit()
{
	page_table_entry * pt = get_PT(current());
	for(int i= 0 ; i < NUM_PAG_DATA; i++){
		free_frame(get_frame(pt, PAG_LOG_INIT_DATA+i));
		del_ss_pag(pt, PAG_LOG_INIT_DATA + i);
	}

	current() -> PID = -1;
	current() -> quantum = 0;
	current() -> state = NULL;

	list_add_tail(&(current()-> list), &freequeue);

	sched_next_rr();
}

int sys_get_stats(int pid, struct stats *st){

	if(pid<0) return -EINVAL;
	for(int i=0; i<NR_TASKS; i++){
		if(task[i].task.PID == pid){
			copy_to_user(&(task[i].task.st), st, sizeof(struct stats));
			return 0;
		}
	}
	return -1;

}
