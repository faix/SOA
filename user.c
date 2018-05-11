#include <libc.h>


char buff[24];

void print_results(int pid){
	struct stats results;
	int err = get_stats(pid, &results);
	if(err < 0) return;
	itoa(pid, buff);
	write(1, "\nResults of PID", 16);
	write(1, buff, strlen(buff));
	itoa(results.user_ticks, buff);
	write(1, "\nuser ticks", 12);
	write(1, buff, strlen(buff));
	itoa(results.blocked_ticks, buff);
	write(1, "\nblocked ticks", 15);
	write(1, buff, strlen(buff));
	itoa(results.ready_ticks, buff);
	write(1, "\nready ticks", 13);
	write(1, buff, strlen(buff));
	itoa(results.system_ticks, buff);
	write(1, "\nsystem ticks", 14);
	write(1, buff, strlen(buff));
}

void cpu_burst(){ // Workload 1

	int aux, pid;
	for(aux = 0; aux < 1; aux++ ){
        	pid = fork();
		if(pid == 0) fork();
	}
	for(int i = 0; i < 5000; i++){
                        for(int j = 0; j < 5000; j++){
                                if(i*j == 4000000) write(1, "\nWorkload", 10);
                        }
                }
	print_results(getpid());


}

void ioburst() //workload 2
{
        int aux, pid;
        for(aux = 0; aux < 1; aux++ ){
                pid = fork();
                if(pid == 0) fork();
        }
	read(0,0, 2000);
        print_results(getpid());
}

void mixed(){ //workload 3

	int aux, pid;
        for(aux = 0; aux < 1; aux++ ){
                pid = fork();
                if(pid == 0) fork();
        }
	for(int i = 0; i < 5000; i++){
                        for(int j = 0; j < 5000; j++){
                                if(i*j == 400000000) write(1, "\nWorkload", 10);
                        }
        }

        read(0,0, 2000);
        print_results(getpid());
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
  set_sched_policy(1);
  cpu_burst();
  if(getpid() == 1)print_results(0);

  //read(0,0,500000);

/*struct stats st;
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
  }*/

  while(1) { }
}
