#include <pthread.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include "Local_Header.h"
#include "ThreadFce.h"


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


int main(void)
{
// 	pthread_t thread1, *thread2;
	int nthreads = 7, i, j;
	thread_args_t *c, *cp;

	if ( (c = malloc(sizeof(thread_args_t))) == NULL)
		perror("malloc c");
// 	thread2 = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
	
	c->nthreads = nthreads;
	
	counter_init(c);
	
	pthread_barrier_init(&c->barr,  NULL, 9);
	
	for(j=0; j<3; j++){
	
		c->count = 0;
	
// 		cp = malloc(sizeof(thread_args_t));
// 	
// 		cp->nthreads = 7;
// 		cp->pcount = &c->count;
// 		cp->var = 8;
// 		cp->plock = &c->lock;
// 		cp->pcond = &c->cond;
// 		cp->pbarr = &c->barr;
		
		TEST_FCE(nthreads, c);

		
// 		pthread_create(&thread1, NULL, &worker,  cp);
// 		
// 		for(i=0; i< nthreads; i++){
// 			cp = malloc(sizeof(thread_args_t));
// 			
// 			cp->nthreads = 7;
// 			cp->pcount = &c->count;
// 			cp->var = i;
// 			cp->plock = &c->lock;
// 			cp->pcond = &c->cond;
// 			cp->pbarr = &c->barr;
// 			pthread_create(&thread2[i], NULL, &worker2, cp);
// 		}
// 			
// 		printf("Main thread waiting for all other threads to finish\n");	
// 		pthread_barrier_wait(&c->barr);
// 
// 		printf("Main thread JOINING other threads to finish\n");	
// 
// 		for(i=0; i< nthreads; i++){
// 			pthread_join(thread2[i], NULL);
// // 			printf("thread %d is finished\n", i);
// 		}
// 			
// 		printf("All threads finished\n");	
// 
// 		pthread_join(thread1, NULL);
// 		printf("thread 1 is finished\n");
// 
// 		printf("final count is %d\n\n\n", c->count);
	}

	pthread_barrier_destroy(&c->barr);

	printf("free C\n");	
	free(c);
// 	free(thread2);

	return 0;
}