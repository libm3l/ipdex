
#ifndef __LOCHEADH__
#define __LOCHEADH__

#include <pthread.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

	typedef struct thread_args {int nthreads;
		int count, *pcount;
		int var;
		pthread_mutex_t   lock, *plock;
		pthread_cond_t    cond, *pcond;
		pthread_barrier_t barr, *pbarr;
	} thread_args_t;

	pthread_mutex_t   global_lock;

#endif
