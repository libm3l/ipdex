
#include "libm3l.h"
#include "Server_Header.h"

#include "Thread_Prt.h"

void *Data_Threads(void *arg)
{
	data_thread_args_t *c = (data_thread_args_t *)arg;
	lmint_t rcbarr;
	
	if( pthread_mutex_lock(c->plock) != 0)
		Perror("Data_Thread: pthread_mutex_lock()");
	printf("In thread \n");	
	
	if(m3l_Cat(c->Node, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
		Error("CatData");
	
// 	sleep(1);
	if( pthread_mutex_unlock(c->plock) != 0)
		Perror("Data_Thread: pthread_mutex_unlock()");
	
	rcbarr = pthread_barrier_wait(c->pbarr);
	if(rcbarr != 0 && rcbarr != PTHREAD_BARRIER_SERIAL_THREAD){
		Error("Data_Threads: pthread_barrier_wait()\n");
		exit(-1);
	}
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);
		
 	return NULL;
}