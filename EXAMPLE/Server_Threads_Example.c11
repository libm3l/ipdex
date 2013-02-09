#include <pthread.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

// void my_thread_func(void* arg){
//     my_struct foo = *((my_struct*)(arg)); /* Cast the void* to our struct type */
//     /* Access foo.a, foo.b, foo.c, etc. here */
// }
// 
// Or, if you are passing a pointer to your structure:
// 
// void my_thread_func(void* arg){
//     my_struct* foo = (my_struct*)arg; /* Cast the void* to our struct type */
//     /* Access foo->a, foo->b, foo->c, etc. here */
// }

typedef struct thread_args {
	int nthreads;
	int count, *pcount;
	int var;
	pthread_mutex_t   lock, *plock;
	pthread_cond_t    cond, *pcond;
	pthread_barrier_t barr, *pbarr;
} thread_args_t;


pthread_mutex_t   global_lock;


void counter_init(thread_args_t *c);
int  counter_get(thread_args_t *c);
void counter_incr(thread_args_t *c);
void counter_wait_threshold(thread_args_t *c, int threshold);

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

int main(void)
{
	pthread_t thread1, *thread2;
	int nthreads = 7, i, j;
	thread_args_t *c, *cp;


// 	thread_mutex_init(&global_lock, NULL);

	c = malloc(sizeof(thread_args_t));
	thread2 = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
	
	c->nthreads = nthreads;
	
	counter_init(c);
	
	pthread_barrier_init(&c->barr,  NULL, 9);
	
	for(j=0; j<3; j++){
	
		c->count = 0;
	
		cp = malloc(sizeof(thread_args_t));
	
		cp->nthreads = 7;
		cp->pcount = &c->count;
		cp->var = 8;
		cp->plock = &c->lock;
		cp->pcond = &c->cond;
		cp->pbarr = &c->barr;

		
		pthread_create(&thread1, NULL, &worker,  cp);
		
		for(i=0; i< nthreads; i++){
			cp = malloc(sizeof(thread_args_t));
			
			cp->nthreads = 7;
			cp->pcount = &c->count;
			cp->var = i;
			cp->plock = &c->lock;
			cp->pcond = &c->cond;
			cp->pbarr = &c->barr;
			pthread_create(&thread2[i], NULL, &worker2, cp);
		}
			
		printf("Main thread waiting for all other threads to finish\n");	
		pthread_barrier_wait(&c->barr);

		printf("Main thread JOINING other threads to finish\n");	

		for(i=0; i< nthreads; i++){
			pthread_join(thread2[i], NULL);
// 			printf("thread %d is finished\n", i);
		}
			
		printf("All threads finished\n");	

		pthread_join(thread1, NULL);
		printf("thread 1 is finished\n");

		printf("final count is %d\n\n\n", c->count);
	}

	pthread_barrier_destroy(&c->barr);
		
	free(c);
	free(thread2);

	return 0;
}

void counter_init(thread_args_t *c)
{
	c->count = 0;
	pthread_mutex_init(&c->lock, NULL);
	pthread_cond_init(&c->cond, NULL);
}

int counter_get(thread_args_t *c)
{
	pthread_mutex_lock(c->plock);
	int val = *c->pcount;
	pthread_mutex_unlock(c->plock);
	return val;
}

void counter_incr(thread_args_t *c)
{
	pthread_mutex_lock(c->plock);
	int val = *c->pcount;
	val = val + 1;
	*c->pcount = val;
	pthread_cond_broadcast(c->pcond);
	pthread_mutex_unlock(c->plock);
}

void counter_wait_threshold(thread_args_t *c, int threshold)
{
	
	pthread_mutex_lock(c->plock);

	while (*c->pcount < threshold)
		pthread_cond_wait(c->pcond, c->plock);
	

	sleep(1);
	// now c->count >= threshold

	pthread_mutex_unlock(c->plock);

}

