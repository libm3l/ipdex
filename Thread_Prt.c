
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
	
 	printf("\n\n\nIn thread %lu \n", pthread_self());	
	
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
	while(1)
	{
/*
 * if already went through do loop, wait here at sync point until all threads are here
 */
		thread_finished = 0;
		
		Pthread_mutex_lock(c->plock);
				
		if(start == 1){
			
			printf("%lu: SECOND Do %lu   %ld %ld THREAD - %d, START %d \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter, thread_finished, start);
			
// 			Pthread_mutex_lock(c->plock);
			if(*c->prcounter == 0){
/* 
 * the last thread, broadcast
 * indicate this is the last thread
 */
				thread_finished = 1;
				printf("%lu:before FINAL broadcasting  ONE  %lu   %ld %ld THRE - %d\n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter, thread_finished);
				pthread_cond_broadcast(c->pdcond);
			}
			else{
/*
 * still some threads working, wait for them
 * indicate this is waiting thread
 */
				thread_finished = 0;
				while (*c->prcounter != 0)
					Pthread_cond_wait(c->pdcond, c->plock);
					printf("%lu:after WAIT %lu   %ld %ld THRE - %d\n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter, thread_finished);
			}
// 			Pthread_mutex_unlock(c->plock);
/*
 * if last thread, release semaphore so that main can make another loop
 */
			if( thread_finished == 1){
				printf("%lu:   SEM_POST OPERATION   %lu  \n", pthread_self());
				Sem_post(c->psem);
				thread_finished = 0;
			}
// 			Pthread_mutex_unlock(c->plock);
		}
/*
 * wait for data sent by main thread
 */

// 		Pthread_mutex_lock(c->plock);
		printf("%lu: Before Cond  %lu  %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
		
// 		while (*c->pcounter == 0 || *c->prcounter == 0)
		while (*c->prcounter == 0)
			Pthread_cond_wait(c->pcond, c->plock);

		(*c->prcounter)--;   /* decrement counter of thread  which will check condition*/
		
		printf("%lu: After Cond  %lu  %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
		
		if(*c->PVARIABLE == pthread_self()){
			(*c->pcounter)--;   /* decrement counter of available thread */
			received = 1;        /* indicate thread received connection */
			printf("%lu: ----------------------   SUCCESS -- Thread received sockeet, variable is %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
			Pthread_mutex_unlock(c->plock);
			printf("%lu: after unlocking pdlock   %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
			break;
		}
		
//  		printf("%lu: before  unlocking pdlock   %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
		Pthread_mutex_unlock(c->plock);
 		printf("%lu: after unlocking pdlock   %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);

		
		if(start == 0)
			start = 1;
		else 
			start = 0;
		
		
	} //while(received != 1);
	
	
	printf(" -------------------------------------------------   Thread %lu received its SOCKET\n", pthread_self() );

		Pthread_mutex_lock(c->plock);
		
			printf(" -------------------------------------------------   Thread %lu SYNCING\n", pthread_self() );

			if(*c->prcounter == 0){
				printf("%lu:before FINAL   - REC broadcasting   %lu   %ld %ld \n", pthread_self(), *c->PVARIABLE, *c->pcounter, *c->prcounter);
				pthread_cond_broadcast(c->pdcond);
				thread_finished = 1;
			}
			else{
				while (*c->prcounter != 0)
					Pthread_cond_wait(c->pdcond, c->plock);
				thread_finished = 0;
			}

		if( thread_finished == 1){
			printf("%lu:   ACCEPTED SEM_POST OPERATION   %lu  \n", pthread_self());
			Sem_post(c->psem);
			thread_finished = 0;
		}	
		Pthread_mutex_unlock(c->plock);
		


	printf(" -------------------------------------------------   Thread %lu EXITS\n", pthread_self() );
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);
		
 	return NULL;
}