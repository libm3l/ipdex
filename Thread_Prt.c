
#include "libm3l.h"
#include "Server_Header.h"

#include "Thread_Prt.h"

void *Data_Threads(void *arg)
{
	data_thread_args_t *c = (data_thread_args_t *)arg;
	lmint_t rcbarr;
	lmint_t received, start, thread_finished;
	
	received = 0;
	start = 0;
	thread_finished = 0;
	
	Pthread_mutex_lock(c->plock);
	
// 	printf("In thread %lu \n", pthread_self());	
	
	if(m3l_Cat(c->Node, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
		Error("CatData");
	
	Pthread_mutex_unlock(c->plock);
/*
 * wait on this barrier until all threads are started
 */	
	Pthread_barrier_wait(c->pbarr);
/*
 * wait on this barrier until main thread sets value of counter and lock c->plock
 */
	Pthread_barrier_wait(c->pbarr);
/*
 * wait for condition variable 
 */
	do
	{

		if(start == 1){

		Pthread_mutex_lock(c->plock);
			if(*c->prcounter == 0){
				printf("%lu:before FINAL broadcasting  ONE  %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
				pthread_cond_broadcast(c->pdcond);
				counter_finished++;

			}
			else{
				while (*c->prcounter != 0)
					Pthread_cond_wait(c->pdcond, c->plock);
			}


		Pthread_mutex_unlock(c->plock);

// 		if(counterF == number of processes) set semaphore;		

		}
		start = 1;





// 		printf("%lu: before do loop\n", pthread_self());

		Pthread_mutex_lock(c->plock);

// 		printf("%lu: after locking plock\n", pthread_self());

		while (*c->pcounter == 0 || *c->prcounter == 0)
// 		while (*c->pcounter == 0)
			Pthread_cond_wait(c->pcond, c->plock);


		
		printf("%lu: After Cond  %lu  %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
		
		if(*c->PVARIABLE == pthread_self()){
			printf("%lu: ----------------------   SUCCESS -- Thread received sockeet, variable is %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
			(*c->pcounter)--;   /* decrement counter of available thread */
			received = 1;        /* indicate thread received connection */
// 			printf("%lu: ----------------------   SUCCESS1111 -- Thread received sockeet, variable is %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE ,*c->pcounter, *c->prcounter);
// 			sleep(3);
// 			printf("%lu: ----------------------   SUCCESS2222 -- Thread received sockeet, variable is %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);

// 			(*c->prcounter)--;   /* decrement counter of thread  which will check condition*/
		}

// 		}
// 		else
// 			printf("%lu: Received number does not correspond   VAR is %lu \n", pthread_self(),  *c->PVARIABLE);
		
		(*c->prcounter)--;   /* decrement counter of thread  which will check condition*/
		
// 			printf("%lu: Decrementing prcoutner %ld \n", pthread_self(), *c->prcounter);


// 		Pthread_mutex_unlock(c->plock);
/*
 * signal main thread you're done
 */

// 		printf("%lu: Waiting on last pcond %ld\n", pthread_self(), *c->prcounter);
		
// 		pthread_mutex_lock(c->pdlock);
// 		printf("%lu: after locking pdlock %ld\n", pthread_self(), *c->prcounter);


// 		if(*c->prcounter == 0){
// 			printf("%lu:before broadcasting   %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
// 			pthread_cond_broadcast(c->pdcond);
// 		}
// 		else{
// 			while (*c->prcounter != 0)
// 				Pthread_cond_wait(c->pdcond, c->plock);
// 		}
		
// 		printf("%lu: before broadcasting\n", pthread_self());

		
// 		printf("%lu: after broadcasting\n", pthread_self());
 		printf("%lu: before  unlocking pdlock   %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);

		Pthread_mutex_unlock(c->plock);
		
 		printf("%lu: after unlocking pdlock   %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);

		
	}while(received != 1);

		Pthread_mutex_lock(c->plock);
			if(*c->prcounter == 0){
				printf("%lu:before FINAL   - REC broadcasting   %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
				pthread_cond_broadcast(c->pdcond);
			}
			else{
				while (*c->prcounter != 0)
					Pthread_cond_wait(c->pdcond, c->plock);
			}
		Pthread_mutex_unlock(c->plock);

	printf(" -------------------------------------------------   Thread %lu received its SOCKET\n", pthread_self() );
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);
		
 	return NULL;
}