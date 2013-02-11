

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
	pthread_mutex_t   lock, *plock;	  /* mutex */
	pthread_mutex_t   lockC, *plockC;	  /* mutex */
	pthread_barrier_t barr, *pbarr;   /* barrier */
	pthread_barrier_t barrC, *pbarrC;   /* barrier */
	pthread_cond_t    cond, *pcond;   /* condition variable */
	lmint_t condition, *pcondition;   /* condition for pthread_cond_t */
	node_t *Node;                     /* libm3l node_t structure pointer */
	pthread_t  VARIABLE, *PVARIABLE;
}data_thread_args_t;



typedef struct data_thread_str{
	lmsize_t n_data_threads;              /* number of thread in group data_threads */
	pthread_t *data_threads;              /* thread ID of all threads in group data_threads */
	data_thread_args_t *Data_Glob_Args;   /* pointer to heap for this group of data_threads */
}data_thread_str_t;

#endif