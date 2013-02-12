

#ifndef __SERVER_HEADER_H__
#define __SERVER_HEADER_H__

#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>

// typedef struct thread_args{
// 	lmint_t count, *pcount;
// 	lmint_t var;
// 	pthread_mutex_t   lock, *plock;
// 	pthread_cond_t    cond, *pcond;
// 	pthread_barrier_t barr, *pbarr;
// }thread_args_t;



typedef struct data_thread_args{
	pthread_mutex_t   lock, *plock;	 	/* mutex */
	pthread_mutex_t   dlock, *pdlock;	 /* mutex */
	pthread_barrier_t barr, *pbarr;  	/* barrier */
	pthread_cond_t    cond, *pcond;   	/* condition variable */
	pthread_cond_t    dcond, *pdcond;   	/* condition variable */
	node_t *Node;                     	/* libm3l node_t structure pointer */
	pthread_t  VARIABLE, *PVARIABLE;  	/* thread number sent for identification */
 	lmint_t   socket, *psocket; 		/* socket ID passed to data_Thread, message upon receiving it */
 	lmsize_t *pcounter, *prcounter;
}data_thread_args_t;



typedef struct data_thread_str{
	lmsize_t n_data_threads;              					/* number of thread in group data_threads */
 	lmsize_t *data_threads_status_counter, *data_threads_remain_counter; 	/* number of free threads, ie. not associated with  */
	pthread_t *data_threads;              					/* thread ID of all threads in group data_threads */
	data_thread_args_t *Data_Glob_Args;   					/* pointer to heap for this group of data_threads */
}data_thread_str_t;

#endif