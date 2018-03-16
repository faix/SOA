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

#define LECTURA 0
#define ESCRIPTURA 1

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

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}
