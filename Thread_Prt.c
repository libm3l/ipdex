
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
	
	Pthread_barrier_wait(c->pbarr);

/*
 * wait for condition variable 
 */

// while( Queue_IsEmpty( Q ) ) {
//       pthread_cond_wait( &notEmpty, &mutex );
//       if( Queue_IsEmpty( Q ) )
//         continue;       /* Keep blocking if the queue is empty. */
//       break;
//     }

		pthread_mutex_lock(c->plock);

		while (*c->pcounter == 0)
			Pthread_cond_wait(c->pcond, c->plock);
		
		printf(" After Cond   %lu  %ld\n", *c->PVARIABLE, *c->pcounter);
		
		if(*c->PVARIABLE == pthread_self()){
			printf("Thread %lu received condition, variable is %lu \n", pthread_self(), *c->PVARIABLE);
			*c->pcounter--;   /* decrement counter of available thread */
			received = 1;     /* indicate thread received connection */
		}
		else
			printf("Received number does not correspond %lu   VAR is %lu \n", pthread_self(),  *c->PVARIABLE);
		
		Pthread_mutex_unlock(c->plock);
		
		
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);
		
 	return NULL;
}