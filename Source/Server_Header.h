

#ifndef __SERVER_HEADER_H__
#define __SERVER_HEADER_H__

#include <pthread.h>
#include <semaphore.h>

typedef struct data_thread_args{
	pthread_mutex_t   *plock;	 	/* mutex */
	pthread_barrier_t *pbarr;  	/* barrier */
	pthread_cond_t    *pcond;   	/* condition variable */
	pthread_cond_t    *pdcond;   	/* condition variable */
	sem_t 		  *psem;		/* semaphore */
	node_t *Node;                     	/* libm3l node_t structure pointer */
 	lmint_t  *psocket; 			/* socket ID passed to data_Thread, message upon receiving it */
 	lmsize_t *pcounter, *prcounter;         /* number of available threads, number of remaining threads = *pcounter - taken threads */
	lmchar_t *pname_of_data_set, *pSR_mode;  /* stores data_set name which is then compared in data_thread and SM_mode */
}data_thread_args_t;



typedef struct data_thread_str{
	pthread_mutex_t   lock;	 	/* mutex */
	pthread_barrier_t barr;  	/* barrier */
	pthread_cond_t    cond;   	/* condition variable */
	pthread_cond_t    dcond;   	/* condition variable */	
	sem_t 		  sem;		/* semaphore */
	
	lmsize_t n_data_threads;              					/* number of thread in group data_threads */
 	lmsize_t *data_threads_availth_counter, *data_threads_remainth_counter; /* number of available and free threads  */
	pthread_t *data_threads;              					/* thread ID of all threads in group data_threads */
	lmchar_t *name_of_data_set, *SR_mode;                                   /* stores data_set name which is then compared in data_thread  and SM_moode*/
 	lmint_t  *socket; 		                                        /* socket ID passed to data_Thread, message upon receiving it */
}data_thread_str_t;


typedef struct SR_thread_args{
	pthread_mutex_t   *plock;	/* mutex */
	pthread_barrier_t *pbarr;  	/* barrier */
	pthread_cond_t    *pcond;   	/* condition variable */
	pthread_cond_t    *pdcond;   	/* condition variable */
	sem_t 		  *psem;	/* semaphore */
	lmchar_t *pbuffer;              /* pointer to buffer */
	lmint_t sockfd;                 /* socket id, unique to every thread, can be in thread stack */
	lmchar_t SR_mode;               /* threads mode - Sender(S), Receiver(R) */
}SR_thread_args_t;

typedef struct SR_thread_str{
	pthread_mutex_t   lock;	 	/* mutex */
	pthread_barrier_t barr;  	/* barrier */
	pthread_cond_t    cond;   	/* condition variable */
	pthread_cond_t    dcond;   	/* condition variable */	
	sem_t 		  sem;		/* semaphore */
	lmchar_t *buffer;		/* buffer where the exchange data will be written and read from */
	pthread_t *data_threads;	/* thread ID of all threads in group data_threads */
}SR_thread_str_t;

#endif