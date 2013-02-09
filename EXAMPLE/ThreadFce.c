
#include "Local_Header.h"
#include "ThreadFce.h"


void *worker(void *arg)
{
	thread_args_t *c = (thread_args_t *)arg;

	int i;
	for (i = 0; i < 10; i++) {
		printf("tick %d, counter is %d\n", i, counter_get(c));
		counter_incr(c);
		sleep(1);
		
		if(i == 6){
// 			printf(" TICK waiting on barrier\n");
			pthread_barrier_wait(c->pbarr);
// 			printf(" TICK resumes\n");
		}
	}
	
	free(c);

 	return NULL;
}

void *worker2(void *arg)
{
	thread_args_t *c = (thread_args_t *)arg;
	counter_wait_threshold(c, 6);
	printf("counter in thread %d has reached 6!\n", c->var);
	sleep(2);
	
// 	printf("counter in thread has reached 6  - WAITING!\n");
	pthread_barrier_wait(c->pbarr);
	printf("Ended waiting!\n");
	
	free(c);
 	return NULL;
}