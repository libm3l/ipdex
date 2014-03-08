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
 *     Function Data_Thread.c
 *
 *     Date: 2013-07-16
 * 
 * 
 *     Description:
 * 
 *
 *     Input parameters:
 * 
 *
 *     Return value:
 * 
 * 
 *
 *     Modifications:
 *     Date		Version		Patch number		CLA 
 *
 *
 *     Description
 * 
 */





#include "libm3l.h"
#include "lsipdx_header.h"
#include "Start_Data_Thread.h"
#include "Data_Thread.h"
#include "Server_Functions_Prt.h"
#include "Associate_Data_Thread.h"

lmsize_t Start_Data_Thread(node_t *Gnode, data_thread_str_t *Data_Thread){
/*
 * function spawns data_threads
 * the number of threads is defined by the number of data sets to be transferred
 * each data_thread then spawns additional n-threads which take care of communication
 * where 1 thread is Sender and n-1 threads are Receivers
 * 
 * Allocated data set in this function are freed when threads are finished (ie. satement free(c) in Data_Thread.c) 
 * and the main data structure is freed in function invoking this function (ie. Server_Body.c)
 * 
 * the heap memery was allocated in Allocate_Data_Thread_DataSet
 */
	lmsize_t i, retval;
	lmint_t pth_err;
	find_t *SFounds;
	data_thread_args_t *DataArgs;
	node_t *List;
	
	if(Gnode == NULL){
		Warning("Data_Thread: NULL Gnode");
		return -1;
	}
/*
 * find how many data sets - defines how many data_threads to spawn
 */
	if( (SFounds = m3l_Locate(Gnode, "/Buffer/Channel", "/*/*", (lmchar_t *)NULL)) != NULL){
		
		Data_Thread->n_data_threads = m3l_get_Found_number(SFounds);
		retval = Data_Thread->n_data_threads;
		
		if(Data_Thread->n_data_threads == 0){
			Error("Server: did not find any Data_set");
			m3l_DestroyFound(&SFounds);
		}
	}
	else
	{
		printf("Server: did not find any Data_set\n");
		exit(0);
	}
/* 
 * malloc data Data_Thread->Data_Str, will be used to store data specific to each Data_Thread (ie. PID, name of channel etc.)
 * the data is then freed in Data_Thread.c function
 */

	if(Data_Thread->Data_Str == NULL){
		if( (Data_Thread->Data_Str = (data_thread_int_str_t **)malloc(sizeof(data_thread_int_str_t *) * Data_Thread->n_data_threads)) == NULL)
		Perror("Data_Thread: Data_Thread->Data_Str malloc");
	}
	else{
		Error("Data_Thread: Data_Thread->Data_Str already malloced");
	}
/*
 * allocate Data_Str
 */
	for(i=0; i < Data_Thread->n_data_threads; i++){
		
		if( (Data_Thread->Data_Str[i] = (data_thread_int_str_t *)malloc(sizeof(data_thread_int_str_t))) == NULL)
			Perror("Data_Thread: Data_Thread->Data_Str[] malloc");
/*
 * malloc for PID of the thread
 */
		if( (Data_Thread->Data_Str[i]->data_threadPID = (pthread_t *)malloc(sizeof(pthread_t))) == NULL)
			Perror("Data_Thread: Data_Thread->Data_Str->data_threads malloc");
/*
 * malloc variable for its name (connection.channel name). This variable
 * will be then defined in Start_Data_Thread where each individual thread 
 * gets its own connection
 */
		if( (Data_Thread->Data_Str[i]->name_of_channel = (lmchar_t *)malloc(MAX_NAME_LENGTH* sizeof(lmchar_t))) == NULL)
			Perror("Data_Thread: Data_Thread->Data_Str->name_of_channel malloc");
/*
 * malloc status_run variable and set it to 1; if 0 thread will be terminated
 */
		if( (Data_Thread->Data_Str[i]->status_run = (lmint_t *)malloc(MAX_NAME_LENGTH* sizeof(lmint_t))) == NULL)
			Perror("Data_Thread: Data_Thread->Data_Str->lmint_t malloc");
		*Data_Thread->Data_Str[i]->status_run = 1;
	}
	
// 	if( (Data_Thread->data_threads_availth_counter = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
// 		Perror("Data_Thread: Data_Thread->data_threads_availth_counter");
// 	if( (Data_Thread->data_threads_remainth_counter = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
// 		Perror("Data_Thread: Data_Thread->data_threads_remainth_counter");
// 	if( (Data_Thread->socket = (lmint_t *)malloc(sizeof(lmint_t))) == NULL)
// 		Perror("Data_Thread: Data_Thread->socket");
// 	if( (Data_Thread->retval = (lmint_t *)malloc(sizeof(lmint_t))) == NULL)
// 		Perror("Data_Thread: Data_Thread->retval");
// 	if ( (Data_Thread->name_of_data_set  = (lmchar_t *)malloc(MAX_NAME_LENGTH* sizeof(lmchar_t))) == NULL)
// 		Perror("Data_Thread: Data_Thread->name_of_data_set");	
// 	if ( (Data_Thread->SR_mode  = (lmchar_t *)malloc(sizeof(lmchar_t))) == NULL)
// 		Perror("Data_Thread: Data_Thread->SR_mode");
// /*
//  * initialize sync 
//  */
// 	if ( (Data_Thread->sync  = (pt_sync_t *)malloc(sizeof(pt_sync_t))) == NULL)
// 		Perror("Data_Thread: Data_Thread->sync");
// 	if ( (Data_Thread->sync->nsync  = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
// 		Perror("Data_Thread: Data_Thread->sybc->nsync");	
// 	if ( (Data_Thread->sync->nthreads  = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
// 		Perror("Data_Thread: Data_Thread->sybc->nthreads");
// 
// 	Pthread_mutex_init(&Data_Thread->sync->mutex);
// 	Pthread_mutex_init(&Data_Thread->sync->block);
// 	Pthread_cond_init(&Data_Thread->sync->condvar);
// 	Pthread_cond_init(&Data_Thread->sync->last);
/*
 * plus the values of the syncing - ie. nsync = 0 and nthreads = n_data_threads + 1
 * during the run, the *Data_Thread->sync->nsync should not be never reset
 * *Data_Thread->sync->nthreads will always be set to number of jobs the 
 * syncing points is required to sync
 */	
// 	*Data_Thread->data_threads_availth_counter  = 0;
// 	*Data_Thread->data_threads_remainth_counter = 0;
// 	*Data_Thread->sync->nsync    = 0;
/*
 * set the value of for syncing thread to number of data sets + 1 (it. sync all Data_Thread (n_data_threads) + 
 * 1 for Server_Body
 */
	*Data_Thread->sync->nthreads = Data_Thread->n_data_threads + 1;	
/*
 * initialize mutex and condition variable
 */
// 	Pthread_mutex_init(&Data_Thread->lock);
// 	Pthread_cond_init(&Data_Thread->cond);
// 	Sem_init(&Data_Thread->sem, 0);
/*
 * spawn threads
 */	
	for(i=0; i < Data_Thread->n_data_threads; i++){
// 		if( (DataArgs = (data_thread_args_t *)malloc(sizeof(data_thread_args_t))) == NULL)
// 			Perror("Data_Thread: DataArgs malloc");
/*
 * set Node pointer to i-th data set in /Buffer/Channel
 * this determines that i-th thread will take care of channel with 
 * name specified in /Buffer/Channel/Name_of_Channel
 */

		List = m3l_get_Found_node(SFounds, i);
		if(  (DataArgs = Associate_Data_Thread(List, Data_Thread)) == NULL)
			Error("Start_Data_Thread: DataArgs NULL pointer");
// 		DataArgs->Node  		= m3l_get_Found_node(SFounds, i);
// 		DataArgs->plock 		= &Data_Thread->lock;	
// 		DataArgs->psem 			= &Data_Thread->sem;	
// 		DataArgs->pcond 		= &Data_Thread->cond;	
// 		DataArgs->psocket    		=  Data_Thread->socket;	
// 		DataArgs->pretval    		=  Data_Thread->retval;	
// 		DataArgs->pcounter    		=  Data_Thread->data_threads_availth_counter;
// 		DataArgs->prcounter    		=  Data_Thread->data_threads_remainth_counter;
// 		DataArgs->pname_of_data_set   	=  Data_Thread->name_of_data_set;
// 		DataArgs->pSR_mode	    	=  Data_Thread->SR_mode;
// 		DataArgs->pcheckdata 		=  Data_Thread->checkdata;
// 
// 		DataArgs->psync 		= Data_Thread->sync;
// 		DataArgs->psync->pnsync 	= Data_Thread->sync->nsync;
// 		DataArgs->psync->pnthreads 	= Data_Thread->sync->nthreads;
// 		DataArgs->psync->pmutex 	= &Data_Thread->sync->mutex;
// 		DataArgs->psync->pblock		= &Data_Thread->sync->block;
// 		DataArgs->psync->pcondvar	= &Data_Thread->sync->condvar;
// 		DataArgs->psync->plast		= &Data_Thread->sync->last;
/*
 * malloc pData_Str and associate with Data_Str
 */
		if( (DataArgs->pData_Str = (data_thread_int_str_t *)malloc(sizeof(data_thread_int_str_t))) == NULL)
			Perror("Data_Thread: DataArgs->pData_Str malloc");	
		DataArgs->pData_Str->name_of_channel = Data_Thread->Data_Str[i]->name_of_channel;
		DataArgs->pData_Str->status_run      = Data_Thread->Data_Str[i]->status_run;
/*
 * create thread
 */
//         if ( pthread_create(&Data_Thread->data_threads[i], NULL, &Data_Threads,  DataArgs) != 0 ){
//             if(errno == EAGAIN) /* If Interrupted system call, restart - back to while ()  UNP V1 p124  */
//                 continue;
//         else
//             Perror("pthread_create()");
//         }

		while ( ( pth_err = pthread_create(Data_Thread->Data_Str[i]->data_threadPID, NULL, &Data_Threads,  DataArgs)) != 0 && errno == EAGAIN);
		if(pth_err != 0)
			Perror("pthread_create()");
/*
 * create a node
 */
	}
	m3l_DestroyFound(&SFounds);
	
	return retval;
}