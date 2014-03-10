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



/*
 * structure for synchronizing routine pt_sync()
 */
 typedef struct pt_sync{
	lmsize_t *nsync, *pnsync; 		/* local counter of threads, used in pt_sync() */
	lmsize_t *nthreads, *pnthreads;   /* number of threads which will be synchronized, should be specified before syncing */
	pthread_mutex_t mutex, *pmutex;	 	 /* mutex used by pt_sync() */
	pthread_mutex_t block, *pblock;  	/* mutex used by pt_sync() */
	pthread_cond_t condvar, *pcondvar;  	/* condition variable used by pt_sync() */
	pthread_cond_t last, *plast; 		/* condition variable used by pt_sync() */
}pt_sync_t;



/*
 * strycture of pointers used by each individual Data_Thread
 * points on data_thread_str_t
 */

typedef struct data_thread_int_str{
	pthread_t 		*data_threadPID;		/* thread ID of all threads in group data_threads */
	lmchar_t 		*name_of_channel;	/* each spawn thread corresponds to required channel (data link)
								store the name in this variable */
	lmint_t			*status_run;		/* if 1, thread is active, if 0, thread should be terminated */
}data_thread_int_str_t;

typedef struct data_thread_args{
	pthread_mutex_t   	*plock;	 	/* mutex */
	pthread_cond_t    	*pcond;   	/* condition variable */
	sem_t 		  	*psem;		/* semaphore */
	node_t 			*Node;		/* libm3l node_t structure pointer specifying name and other data for Data_Thread*/
	lmint_t  		*psocket,  *pretval, *pcheckdata;	/* socket ID passed to data_Thread, message upon receiving it, return value */
	lmsize_t		*pcounter, *prcounter;  /* number of available threads, sync variable, number of 
							remaining threads = *pcounter - taken threads */
	lmchar_t 		*pname_of_data_set, *pSR_mode;	/* stores data_set name which is then compared in data_thread and SM_mode */
	pt_sync_t		*psync;
	data_thread_int_str_t	*pData_Str;	/* Data_Thread specific data PID, name_of_channel etc. */
}data_thread_args_t;

/*
 * heap data for Data_Thread, allcoated in Allocate_Data_Thread_DataSet
 */
typedef struct data_thread_str{
	pthread_mutex_t   	lock;	 	/* mutex */
	pthread_cond_t    	cond;   	/* condition variable */
	sem_t 		 	sem;		/* semaphore */

	lmsize_t 		n_data_threads;  /* number of thread in group data_threads */
	lmsize_t 		*data_threads_availth_counter, *data_threads_remainth_counter; 	/* number of available and free threads  */
	lmchar_t 		*name_of_data_set, *SR_mode;	/* stores data_set name which is then compared in data_thread  and SR_moode*/
	lmint_t  		*socket,  *retval, *checkdata;		/* socket ID passed to data_Thread, message upon receiving it, yes or not to check data*/
	pt_sync_t		*sync;
	data_thread_int_str_t	**Data_Str;	/* Data_Thread specific data PID, name_of_channel etc. */
}data_thread_str_t;



/*
 * structure with pointers on SR_thread_str_t, each SR_Data_Thread has one
 */
typedef struct SR_thread_args{
	pthread_mutex_t   	*plock;				/* mutex */
	pthread_cond_t    	*pdcond;   			/* condition variable */
	sem_t 		  	*psem, *psem_g;			/* semaphore */
	lmchar_t 		*pbuffer;       		/* pointer to buffer */
	lmint_t 		*psockfd, *pEofBuff, *psync;    /* socket id, unique to every thread, can be in thread stack, End-ofbuffer 
								signal */
	lmchar_t 		*pSR_mode;			/* threads modes - Sender(S), Receiver(R) */
	lmchar_t		*pATDT_mode, *pKA_mode;      			
	lmint_t 		*pSRt_mode, *pEOFC_ENDt;	/* SR mode, Return value from SR_Data_Threads */
	lmsize_t  		*pthr_cntr;    			/* thread counter */
	lmsize_t 		*pcounter, *prcounter, *pngotten;    /* number of available R_threads, number of remaining 
									threads = *pcounter - taken threads 
									length of buffer from TCP/IP */
	pt_sync_t		*psync_loc;
}SR_thread_args_t;

/*
 * heap data for SR_Data_Thread, allocated in SR_Data_Thread
 */
typedef struct SR_thread_str{
	pthread_mutex_t   	lock;								/* mutex */
	pthread_cond_t    	dcond;								/* condition variable */	
	sem_t 		  	sem, sem_g;							/* semaphore */
	lmchar_t 		*buffer;			/* buffer where the exchange data will be written and read from */
	pthread_t 		*data_threads;			/* thread ID of all threads in group data_threads */
	lmint_t  		*sockfd, *EofBuff, *sync;	/* socket id, unique to every thread, can be in thread stack , End-ofbuffer signal */
	lmchar_t 		*SR_mode;			/* threads mode - Sender(S), Receiver(R) */
	lmchar_t 		*ATDT_mode, *KA_mode;
	lmsize_t  		*thr_cntr;            					/* thread counter */
	lmsize_t 		*R_availth_counter, *R_remainth_counter, *ngotten; 	/* number of available and free threads, length of
											buffer from TCP/IP   */
	pt_sync_t		*sync_loc;
	lmint_t			*mode, *EOFC_END;
}SR_thread_str_t;





/*
 * data strucure for SR_hub
 */
typedef struct SR_hub_thread_str{
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
	pt_sync_t		*psync_loc;
}SR_hub_thread_str_t;



/*
 * structure for thread communicating with server
 * - need to have access to Data_Thread and SR_Data_Thread
 */
typedef struct Server_Comm_DataStr{
	pthread_t 		*data_threadsPID;			/* thread ID of all threads in group data_threads */
	data_thread_args_t *Data_Thread_Pointer;
// 	SR_thread_str_t *SR_Data_Thread_Pointer;
}Server_Comm_DataStr_t;

/*
 * stores data needed to identify modes of connection etc.
 */
typedef struct client_fce_struct{
	lmchar_t *data_name;
	lmchar_t SR_MODE;
	lmint_t mode;
}client_fce_struct_t;


#endif
