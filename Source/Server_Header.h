

#ifndef __SERVER_HEADER_H__
#define __SERVER_HEADER_H__

#include <pthread.h>
#include <semaphore.h>


 typedef struct pt_sync{
	lmsize_t *nsync, *pnsync;   			/* number of threads which will be synchronized, should be specified before syncing */
/*
 * local variables used by syncing routine, shoulc not be manipulated outside
 */
	lmsize_t *nthreads, *pnthreads;   		/* local counter of threads, used in psync */
	pthread_mutex_t mutex, *pmutex;	
	pthread_mutex_t block, *pblock;
	pthread_cond_t condvar, *pcondvar;
	pthread_cond_t last, *plast;
}pt_sync_t;




typedef struct data_thread_args{
	pthread_mutex_t   	*plock;	 	/* mutex */
	pthread_barrier_t 	*pbarr;  		/* barrier */
	pthread_cond_t    	*pcond;   		/* condition variable */
	pthread_cond_t    	*pdcond;   		/* condition variable */
	sem_t 		  	*psem;		/* semaphore */
	node_t 			*Node;		/* libm3l node_t structure pointer specifying name and other data for Data_Thread*/
	lmint_t  		*psocket,  *pretval;		/* socket ID passed to data_Thread, message upon receiving it, return value */
	lmsize_t		*pcounter, *psync_loc, *prcounter;         		/* number of available threads, sync variable, number of remaining threads = *pcounter - taken threads */
	lmchar_t 		*pname_of_data_set, *pSR_mode;	/* stores data_set name which is then compared in data_thread and SM_mode */
	pt_sync_t		*psync;
	
}data_thread_args_t;


typedef struct data_thread_str{
	pthread_mutex_t   	lock;	 		/* mutex */
	pthread_barrier_t 	barr;  		/* barrier */
	pthread_cond_t    	cond;   		/* condition variable */
	pthread_cond_t    	dcond;   		/* condition variable */	
	sem_t 		 	sem;			/* semaphore */

	lmsize_t 		n_data_threads;              								/* number of thread in group data_threads */
	lmsize_t 		*data_threads_availth_counter, *data_threads_remainth_counter; 	/* number of available and free threads  */
	lmsize_t 		*sync_loc;
	pthread_t 		*data_threads;              								/* thread ID of all threads in group data_threads */
	lmchar_t 		*name_of_data_set, *SR_mode;							/* stores data_set name which is then compared in data_thread  and SM_moode*/
	lmint_t  		*socket,  *retval;			 							/* socket ID passed to data_Thread, message upon receiving it */
	pt_sync_t		*sync;

}data_thread_str_t;









typedef struct SR_thread_args{
	pthread_barrier_t 	*pbarr;  					/* barrier */
	pthread_mutex_t   	*plock;					/* mutex */
	pthread_cond_t    	*pdcond;   					/* condition variable */
	sem_t 		  	*psem, *psem_g;			/* semaphore */
	lmchar_t 		*pbuffer;       					/* pointer to buffer */
	lmint_t 		*psockfd, *pEofBuff, *psync;       		/* socket id, unique to every thread, can be in thread stack, End-ofbuffer signal */
	lmchar_t 		*pSR_mode;      				/* threads mode - Sender(S), Receiver(R) */
	lmsize_t  		*pthr_cntr;    					/* thread counter */
	lmsize_t 		*pcounter, *prcounter, *pngotten;  	/* number of available R_threads, number of remaining threads = *pcounter - taken threads 
											length of buffer from TCP/IP */

	pt_sync_t		*psync_loc;
}SR_thread_args_t;


typedef struct SR_thread_str{
 	pthread_barrier_t 	barr;  								/* barrier */
	pthread_mutex_t   	lock;									/* mutex */
	pthread_cond_t    	dcond;								/* condition variable */	
	sem_t 		  	sem, sem_g;							/* semaphore */
	lmchar_t 		*buffer;									/* buffer where the exchange data will be written and read from */
	pthread_t 		*data_threads;								/* thread ID of all threads in group data_threads */
	lmint_t  		*sockfd, *EofBuff, *sync;						/* socket id, unique to every thread, can be in thread stack , End-ofbuffer signal */
	lmchar_t 		*SR_mode;								/* threads mode - Sender(S), Receiver(R) */
	lmsize_t  		*thr_cntr;            							/* thread counter */
	lmsize_t 		*R_availth_counter, *R_remainth_counter, *ngotten; 	/* number of available and free threads, length of buffer from TCP/IP   */
	pt_sync_t		*sync_loc;
}SR_thread_str_t;







typedef struct SR_hub_thread_str{
	pthread_barrier_t 	*pbarr;
	sem_t 		  	*psem, *psem_g;
	pthread_mutex_t   	*plock;	
	pthread_cond_t    	*pcond;
	lmsize_t  			*pcounter, *pn_avail_loc_theads, *pn_rec_proc;
	lmint_t 			*pThread_Status;
	pthread_t 			*data_thread;				/* thread ID */
}SR_hub_thread_str_t;









typedef struct attr_buff_str{
	lmchar_t SR_mode;
	lmint_t socketnr;
}attr_buff_str_t; 

typedef struct buff_str{
	lmchar_t name_of_required_data_set[MAX_NAME_LENGTH];
	attr_buff_str_t *process_data;
	lmint_t NR, TR, TS;  /* number of Receiving processes, number of taken processes, number of taken senders (TS can be either 0 or 1) */
}buff_str_t;

#endif
