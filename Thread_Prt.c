
#include "libm3l.h"
#include "Server_Header.h"

#include "Thread_Prt.h"

void *Data_Threads(void *arg)
{
	data_thread_args_t *c = (data_thread_args_t *)arg;
	lmint_t rcbarr;
	
	Pthread_mutex_lock(c->plock);
	
	printf("In thread %lu \n", pthread_self());	
	
	if(m3l_Cat(c->Node, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
		Error("CatData");
	
// 	sleep(1);
	Pthread_mutex_unlock(c->plock);
	
	Pthread_barrier_wait(c->pbarr);

/*
 * wait for condition variable 
 */	
// again:
// 		pthread_mutex_lock(c->plock);
// 
// 		while (! *c->pcondition)
// 			if( pthread_cond_wait(c->pcond, c->plock)  != 0)
//				Perror("pthread_cond_wait()"); 
// 		
// 		if(*c->PVARIABLE == pthread_self())
// 			printf("Thread %lu received condition, variable is %lu \n", pthread_self(), *c->PVARIABLE);
// 		else
// 			printf("Received number does not correspond %lu   VAR is %lu \n", pthread_self(),  *c->PVARIABLE);
// 
// 		pthread_mutex_unlock(c->plock);
// 		
// 		printf(" Thread thread waiting on barrier\n");
// 		rcbarr = pthread_barrier_wait(c->pbarrC);
// 		if(rcbarr != 0 && rcbarr != PTHREAD_BARRIER_SERIAL_THREAD){
// 			Error("Data_Threads: pthread_barrier_wait()\n");
// 			exit(-1);
// 		}
// 		if(*c->PVARIABLE != pthread_self()) goto again;

/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);
		
 	return NULL;
}