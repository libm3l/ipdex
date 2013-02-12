
#include "libm3l.h"
#include "Server_Header.h"

#include "Thread_Prt.h"

void *Data_Threads(void *arg)
{
	data_thread_args_t *c = (data_thread_args_t *)arg;
	lmint_t rcbarr;
	lmint_t received;
	
	received = 0;
	
	Pthread_mutex_lock(c->plock);
	
// 	printf("In thread %lu \n", pthread_self());	
	
	if(m3l_Cat(c->Node, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
		Error("CatData");
	
	Pthread_mutex_unlock(c->plock);
/*
 * wait on this barrier until all threads are started
 */	
	Pthread_barrier_wait(c->pbarr);
	
	printf("%lu: before second BARRIER\n", pthread_self());
/*
 * wait on this barrier until main thread sets value of counter and lock c->plock
 */
	Pthread_barrier_wait(c->pbarr);

/*
 * wait for condition variable 
 */
	do
	{
		
		printf("%lu: before do loop\n", pthread_self());
		pthread_mutex_lock(c->plock);
		printf("%lu: after locking plock\n", pthread_self());

		while (*c->pcounter == 0 || *c->prcounter == 0)
			Pthread_cond_wait(c->pcond, c->plock);
		
		printf("%lu: After Cond  %lu  %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
		
		if(*c->PVARIABLE == pthread_self()){
			printf("%lu: ----------------------   SUCCESS -- Thread received sockeet, variable is %lu \n", pthread_self(), *c->PVARIABLE);
			(*c->pcounter)--;   /* decrement counter of available thread */
			received = 1;     /* indicate thread received connection */
// 			(*c->prcounter)--;   /* decrement counter of thread  which will check condition*/

		}
		else
			printf("%lu: Received number does not correspond   VAR is %lu \n", pthread_self(),  *c->PVARIABLE);
		
		(*c->prcounter)--;   /* decrement counter of thread  which will check condition*/
		
			printf("%lu: Decrementing prcoutner %lu %ld \n", pthread_self(), *c->PVARIABLE, *c->prcounter);


		Pthread_mutex_unlock(c->plock);
/*
 * signal main thread you're done
 */

		printf("%lu: Waiting %lu on last pcond %ld\n", pthread_self(), *c->PVARIABLE, *c->prcounter);
		
		pthread_mutex_lock(c->pdlock);
		printf("%lu: after %lu locking pdlock %ld\n", pthread_self(), *c->PVARIABLE, *c->prcounter);

		while (*c->prcounter != 0){
			printf("%lu: IN BROADCAST WHILE  %lu %ld\n", pthread_self(), *c->PVARIABLE, *c->prcounter);
			Pthread_cond_wait(c->pdcond, c->pdlock);}
		
		printf("%lu: before broadcasting %lu  %ld\n", pthread_self(), *c->PVARIABLE, *c->prcounter);

		pthread_cond_broadcast(c->pdcond);
		
		printf("%lu: after broadcasting %lu  %ld\n", pthread_self(), *c->PVARIABLE, *c->prcounter);

		Pthread_mutex_unlock(c->pdlock);
		
		printf("%lu: after unlocking pdlock %lu  %ld\n", pthread_self(), *c->PVARIABLE, *c->prcounter);

		
	}while(received != 1);

	printf(" -------------------------------------------------   Thread %lu received its SOCKET\n", pthread_self() );
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);
		
 	return NULL;
}