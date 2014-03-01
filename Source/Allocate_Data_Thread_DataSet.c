



#include "libm3l.h"
#include "lsipdx_header.h"
#include "Server_Functions_Prt.h"
#include "Allocate_Data_Thread_DataSet.h"

data_thread_str_t *Allocate_Data_Thread_DataSet(node_t *Gnode){
/*
 * function allocated data_thread_str_t *Data_Thread which is used by data_Thread
 */
	lmsize_t i;
	data_thread_str_t *Data_Thread;
	find_t *SFounds;

	Data_Thread = NULL;
	
	if(Gnode == NULL){
		Warning("Data_Thread: NULL Gnode");
		return NULL;
	}
/*
 * malloc the main node
 */
	if( (Data_Thread = (data_thread_str_t *)malloc(sizeof(data_thread_str_t))) == NULL)
		Perror("Data_Thread: Data_Thread malloc");
/*
 * find how many data sets - defines how many data_threads to spawn
 */

// 	if( (SFounds = m3l_Locate(Gnode, "/COMM_DEF/Channels/Channel", "/*/*/*", (lmchar_t *)NULL)) != NULL){
	if( (SFounds = m3l_Locate(Gnode, "/Buffer/Channel", "/*/*", (lmchar_t *)NULL)) != NULL){
		
		Data_Thread->n_data_threads = m3l_get_Found_number(SFounds);
		
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
// /*
//  * malloc the main node
//  */
// 	if( (Data_Thread = (data_thread_str_t *)malloc(sizeof(data_thread_str_t))) == NULL)
// 		Perror("Data_Thread: Data_Thread malloc");
/* 
 * malloc data on heap, will be used to share data between threads
 * the data is then freed in Data_Thread.c function
 */
/*
 * the Data_Thread->data_threads containing PID of each spawn thread will be 
 * allocated in Start_Data_Thread, now we do not know how many data threads will be used
 */
	if( (Data_Thread->data_threads = (pthread_t *)malloc(sizeof(pthread_t) * Data_Thread->n_data_threads)) == NULL)
		Perror("Data_Thread: Data_Thread->data_threads malloc");	
	if( (Data_Thread->data_threads_availth_counter = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Data_Thread: Data_Thread->data_threads_availth_counter");
	if( (Data_Thread->data_threads_remainth_counter = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Data_Thread: Data_Thread->data_threads_remainth_counter");
	if( (Data_Thread->socket = (lmint_t *)malloc(sizeof(lmint_t))) == NULL)
		Perror("Data_Thread: Data_Thread->socket");
	if( (Data_Thread->retval = (lmint_t *)malloc(sizeof(lmint_t))) == NULL)
		Perror("Data_Thread: Data_Thread->retval");
	if ( (Data_Thread->name_of_data_set  = (lmchar_t *)malloc(MAX_NAME_LENGTH* sizeof(lmchar_t))) == NULL)
		Perror("Data_Thread: Data_Thread->name_of_data_set");	
	if ( (Data_Thread->SR_mode  = (lmchar_t *)malloc(sizeof(lmchar_t))) == NULL)
		Perror("Data_Thread: Data_Thread->SR_mode");
/*
 * initialize sync 
 */
	if ( (Data_Thread->sync  = (pt_sync_t *)malloc(sizeof(pt_sync_t))) == NULL)
		Perror("Data_Thread: Data_Thread->sync");
	if ( (Data_Thread->sync->nsync  = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Data_Thread: Data_Thread->sybc->nsync");	
	if ( (Data_Thread->sync->nthreads  = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Data_Thread: Data_Thread->sybc->nthreads");

	Pthread_mutex_init(&Data_Thread->sync->mutex);
	Pthread_mutex_init(&Data_Thread->sync->block);
	Pthread_cond_init(&Data_Thread->sync->condvar);
	Pthread_cond_init(&Data_Thread->sync->last);
/*
 * plus the values of the syncing - ie. nsync = 0 and nthreads = n_data_threads + 1
 * during the run, the *Data_Thread->sync->nsync should not be never reset
 * *Data_Thread->sync->nthreads will always be set to number of jobs the 
 * syncing points is required to sync
 */	
	*Data_Thread->data_threads_availth_counter  = 0;
	*Data_Thread->data_threads_remainth_counter = 0;
	*Data_Thread->sync->nsync    = 0;
// 
/*
 * set the value of for syncing thread to number of data sets + 1 (it. sync all Data_Thread (n_data_threads) + 
 * 1 for Server_Body. This is done in Start_Data_Thread, at the moment we do not know 
 * how many threads will be used.
 */
	*Data_Thread->sync->nthreads = Data_Thread->n_data_threads + 1;
/*
 * initialize mutex, barrier and condition variable
 */
	Pthread_mutex_init(&Data_Thread->lock);
	Pthread_cond_init(&Data_Thread->cond);
	Sem_init(&Data_Thread->sem, 0);

	return Data_Thread;
}