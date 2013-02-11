

#ifndef __SERVER_HEADER_H__
#define __SERVER_HEADER_H__

#include <pthread.h>
#include <semaphore.h>


typedef struct thread_args{
	lmint_t count, *pcount;
	lmint_t var;
	pthread_mutex_t   lock, *plock;
	pthread_cond_t    cond, *pcond;
	pthread_barrier_t barr, *pbarr;
}thread_args_t;



typedef struct data_thread_str{
	lmsize_t n_data_threads;
	pthread_t *data_threads;
}data_thread_str_t;

typedef struct data_thread_args{
	pthread_mutex_t   lock, *plock;	
	pthread_barrier_t barr, *pbarr;
	node_t *Node;
}data_thread_args_t;

#endif