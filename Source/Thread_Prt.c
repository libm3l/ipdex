
#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"

#include "Thread_Prt.h"

void *Data_Threads(void *arg)
{
	data_thread_args_t *c = (data_thread_args_t *)arg;
	lmint_t rcbarr;
	lmint_t received;
	
	pthread_t  MyThreadID;
/*
 * get my thread ID
 */
	MyThreadID = pthread_self();
	
	Pthread_mutex_lock(c->plock);
		printf("\n\n In THREAD %lu\n\n", MyThreadID);
		if(m3l_Cat(c->Node, "--all", "-L", "-P", "*",   (lmchar_t *)NULL) != 0)
			Warning("CatData");
	Pthread_mutex_unlock(c->plock);
	
/*
 * wait on this barrier until all threads are started
 * the barrier is called n-times (n=number of Data_Threads + 1) where the last call is made
 * from Data_Thread which spawns this thread
 */	
	Pthread_barrier_wait(c->pbarr);
/*
 * wait on this barrier until main thread sets value of counter and lock c->plock
 * the last call to _wait() is done in the main function after returning back from Data_Threads = Data_Thread(Gnode)
 */
	Pthread_barrier_wait(c->pbarr);
/*
 * wait for condition variable 
 */
	do
	{
/*
 * if already went through do loop, wait here at sync point until all threads are here
 */
		received = 0;		
		Pthread_mutex_lock(c->plock);
/*
 * wait for data sent by main thread
 */
		while (*c->prcounter == 0)
			Pthread_cond_wait(c->pcond, c->plock);

		(*c->prcounter)--;   /* decrement counter of thread  which will check condition*/
				
		if(*c->PVARIABLE == MyThreadID){
			(*c->pcounter)--;   /* if the thread is positively identified, decrement counter of available thread for next round of identification */
			received = 1;        /* indicate thread received connection */
		}
/*
 * synchronized all threads at the end, the last thread will broadcast
 */	
		if(*c->prcounter == 0){
/* 
 * the last thread, broadcast
 * indicate this is the last thread
 */
			pthread_cond_broadcast(c->pdcond);
/* 
 * unlock semaphore in the main program so that another loop can starte
 */
			Sem_post(c->psem);
		}
		else{
/*
 * still some threads working, wait for them
 * indicate this is waiting thread
 */
// 			while (*c->prcounter != 0)
				Pthread_cond_wait(c->pdcond, c->plock);
		}
			
 		Pthread_mutex_unlock(c->plock);	
		
	}while(received != 1);
	
	
	printf(" -------------------------------------------------   Thread %lu received its SOCKET\n", MyThreadID );
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);
		
 	return NULL;
}