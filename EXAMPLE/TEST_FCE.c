
#include "Local_Header.h"
#include "ThreadFce.h"
#include "TEST_FCE.h"

void TEST_FCE(int nthreads, thread_args_t *c){
	
	pthread_t thread1, *thread2;
	thread_args_t *cp;
	int i;

	if ( (thread2 = (pthread_t *)malloc(sizeof(pthread_t) * nthreads))  == NULL)
		perror("malloc thread2");
	
	if ( (cp = malloc(sizeof(thread_args_t))) == NULL)
		perror("malloc cp");
	
	
	
	cp->nthreads = 7;
	cp->pcount = &c->count;
	cp->var = 8;
	cp->plock = &c->lock;
	cp->pcond = &c->cond;
	cp->pbarr = &c->barr;
	
	pthread_create(&thread1, NULL, &worker,  cp);
// 		
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

	printf("free thread2\n");
	free(thread2);
}


