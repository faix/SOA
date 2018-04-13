#include <libc.h>
#include <stats.h>

int add2(int par1, int par2);

char buff[24];

int pid;

int get_stats(int pid, struct stats *st);

int add(int par1, int par2){
	return par1 + par2;
}
long inner(long n){

	int i;
	long suma;
	suma = 0;
	for (i=0; i<n; i++) suma = suma + i;
	return suma;
}

long outer(long n){
	int i;
	long acum;
	acum = 0;
	for(i=0; i <n; i++) acum = acum + inner(i);
	return acum;
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  /*long count, acum, asmSum;
  count = 75;
  acum = 0;
  acum = outer(count);    
  asmSum = add2(count, acum);
  write(1, "SOA, Hello from PEP", 19);*/
  struct stats st;
  int temp = getpid();
  int error = get_stats(temp, &st);
  if(error == 0) write(1, "Get stats worked", 16);
  write(1, "Before Fork", 11);
  char aux[6];
  itoa(temp, aux);
  write(1, aux, 6);
  int pid = fork();
  if(pid >= 0){
	if(pid == 0){
		char numpid[6];
		itoa(getpid(), numpid);
		write(1, "Child PID", 10);
		write(1, numpid, 6);
		exit();
	}
	else{
		write(1, "I am Father", 11);
		char numpid[6];
		itoa(getpid(), numpid);
		write(1, numpid, 6);
	}
  }
  
  //exit();
  while(1) { }
  return 0;
}
