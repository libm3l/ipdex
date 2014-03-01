/*
 *     Copyright (C) 2012  Adam Jirasek
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *     
 *     contact: libm3l@gmail.com
 * 
 */




/*
 *     Header file lsipdx_header.h
 *
 *     Date: 2013-09-07
 * 
 *
 *     Modifications:
 *     Date		Version		Patch number		CLA 
 *
 *
 *     Description
 * 
 */





#ifndef __IPDX_HEADER_H__
#define __IPDX_HEADER_H__

#include <pthread.h>
#include <semaphore.h>

#define EOFCY "0"EOFbuff
#define EOFCN "1"EOFbuff

#define EOFClen 1+EOBlen



 typedef struct pt_sync{
	lmsize_t *nsync, *pnsync; 		/* local counter of threads, used in pt_sync() */
	lmsize_t *nthreads, *pnthreads;   /* number of threads which will be synchronized, should be specified before syncing */
	pthread_mutex_t mutex, *pmutex;	 	 /* mutex used by pt_sync() */
	pthread_mutex_t block, *pblock;  	/* mutex used by pt_sync() */
	pthread_cond_t condvar, *pcondvar;  	/* condition variable used by pt_sync() */
	pthread_cond_t last, *plast; 		/* condition variable used by pt_sync() */
}pt_sync_t;




typedef struct data_thread_args{
	pthread_mutex_t   	*plock;	 	/* mutex */
	pthread_cond_t    	*pcond;   	/* condition variable */
	sem_t 		  	*psem;		/* semaphore */
	node_t 			*Node;		/* libm3l node_t structure pointer specifying name and other data for Data_Thread*/
	lmint_t  		*psocket,  *pretval;	/* socket ID passed to data_Thread, message upon receiving it, return value */
	lmsize_t		*pcounter, *prcounter;  /* number of available threads, sync variable, number of remaining threads = *pcounter - taken threads */
	lmchar_t 		*pname_of_data_set, *pSR_mode;	/* stores data_set name which is then compared in data_thread and SM_mode */
	pt_sync_t		*psync;
	
}data_thread_args_t;


typedef struct data_thread_str{
	pthread_mutex_t   	lock;	 	/* mutex */
	pthread_cond_t    	cond;   	/* condition variable */
	sem_t 		 	sem;		/* semaphore */

	lmsize_t 		n_data_threads;  /* number of thread in group data_threads */
	lmsize_t 		*data_threads_availth_counter, *data_threads_remainth_counter; 	/* number of available and free threads  */
	pthread_t 		*data_threads;              /* thread ID of all threads in group data_threads */
	lmchar_t 		*name_of_data_set, *SR_mode;	/* stores data_set name which is then compared in data_thread  and SM_moode*/
	lmint_t  		*socket,  *retval;		/* socket ID passed to data_Thread, message upon receiving it */
	pt_sync_t		*sync;

}data_thread_str_t;









typedef struct SR_thread_args{
	pthread_barrier_t 	*pbarr;  			/* barrier */
	pthread_mutex_t   	*plock;				/* mutex */
	pthread_cond_t    	*pdcond;   			/* condition variable */
	sem_t 		  	*psem, *psem_g;			/* semaphore */
	lmchar_t 		*pbuffer;       		/* pointer to buffer */
	lmint_t 		*psockfd, *pEofBuff, *psync;    /* socket id, unique to every thread, can be in thread stack, End-ofbuffer signal */
	lmchar_t 		*pSR_mode;			/* threads modes - Sender(S), Receiver(R) */
	lmchar_t		*pATDT_mode, *pKA_mode;      			
	lmint_t 		*pSRt_mode, *pEOFC_ENDt;	/* SR mode, Return value from SR_Data_Threads */
	lmsize_t  		*pthr_cntr;    			/* thread counter */
	lmsize_t 		*pcounter, *prcounter, *pngotten;  /* number of available R_threads, number of remaining threads = *pcounter - taken threads 
											length of buffer from TCP/IP */
	pt_sync_t		*psync_loc;
}SR_thread_args_t;


typedef struct SR_thread_str{
 	pthread_barrier_t 	barr;  								/* barrier */
	pthread_mutex_t   	lock;									/* mutex */
	pthread_cond_t    	dcond;								/* condition variable */	
	sem_t 		  	sem, sem_g;							/* semaphore */
	lmchar_t 		*buffer;			/* buffer where the exchange data will be written and read from */
	pthread_t 		*data_threads;			/* thread ID of all threads in group data_threads */
	lmint_t  		*sockfd, *EofBuff, *sync;	/* socket id, unique to every thread, can be in thread stack , End-ofbuffer signal */
	lmchar_t 		*SR_mode;			/* threads mode - Sender(S), Receiver(R) */
	lmchar_t 		*ATDT_mode, *KA_mode;
	lmsize_t  		*thr_cntr;            					/* thread counter */
	lmsize_t 		*R_availth_counter, *R_remainth_counter, *ngotten; 	/* number of available and free threads, length of buffer from TCP/IP   */
	pt_sync_t		*sync_loc;
	lmint_t			*mode, *EOFC_END;
}SR_thread_str_t;







typedef struct SR_hub_thread_str{
	pthread_barrier_t 	*pbarr;
	sem_t 		  	*psem, *psem_g;
	pthread_mutex_t   	*plock;	
	pthread_cond_t    	*pcond;
	lmsize_t  		*pcounter, *pn_avail_loc_theads, *pn_rec_proc, *prcounter;
	lmint_t 		*pThread_Status, *pThread_S_Status;
	lmsize_t		*pThread_R_Status;
	pthread_t 		*data_thread;					/* thread ID */
	lmint_t 		*psockfd;					/* pointer to array of opened sockets */
	node_t 			*pList;
	lmchar_t		*pATDT_mode, *pKA_mode;	
	lmint_t 		*pSRh_mode, *pEOFC_ENDh;
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


typedef struct client_fce_struct{
	lmchar_t *data_name;
	lmchar_t SR_MODE;
	lmint_t mode;
}client_fce_struct_t;


#endif
