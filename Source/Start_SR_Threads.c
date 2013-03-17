

#include "libm3l.h"
#include "Server_Header.h"
#include "Start_SR_Threads.h"
#include "Server_Functions_Prt.h"


SR_thread_str_t *Start_SR_Threads(lmint_t n_threads){

	lmsize_t i;
	lmint_t pth_err;
	SR_thread_str_t  *SR_Data_Thread;
	SR_thread_args_t *SR_DataArgs;  
/*
 * malloc the main node
 */
	if( (SR_Data_Thread = (SR_thread_str_t *)malloc(sizeof(SR_thread_str_t))) == NULL)
		Perror("SR_Start_SR_Threads: SR_SR_Data_Thread malloc");
/* 
 * malloc data in heap, will be used to share data between threads
 */
	if( (SR_Data_Thread->data_threads = (pthread_t *)malloc(sizeof(pthread_t) * n_threads)) == NULL)
		Perror("Start_SR_Threads: SR_Data_Thread->data_threads malloc");
/*
 * initialize mutex, barrier and condition variable
 */
	Pthread_mutex_init(&SR_Data_Thread->lock);
	Pthread_barrier_init(&SR_Data_Thread->barr, n_threads);
	Pthread_cond_init(&SR_Data_Thread->cond);
	Pthread_cond_init(&SR_Data_Thread->dcond);
	Sem_init(&SR_Data_Thread->sem, 0);
/*
 * spawn threads
 */	
	for(i=0; i < n_threads; i++){
		if( (SR_DataArgs = (SR_thread_args_t *)malloc(sizeof(SR_thread_args_t))) == NULL)
			Perror("Start_SR_Threads: SR_DataArgs malloc");	
		
		SR_DataArgs->plock 		= &SR_Data_Thread->lock;	
		SR_DataArgs->psem 		= &SR_Data_Thread->sem;	
		SR_DataArgs->pbarr 		= &SR_Data_Thread->barr;	
		SR_DataArgs->pcond 		= &SR_Data_Thread->cond;	
		SR_DataArgs->pdcond 		= &SR_Data_Thread->dcond;	
/*
 * create thread
 */
// 		while ( (pth_err = pthread_create(&SR_Data_Thread->data_threads[i], NULL, &SR_Data_Threads,  SR_DataArgs)) != 0 && errno == EAGAIN);
// 		if(pth_err != 0)
// 			Perror("pthread_create()"); 
	}
/*
 * wait on this barrier until all threads are created - the barriers are waited on in Data_Threads and this is the last one
 * makes sure we leave the function after all threads are created
 */
	Pthread_barrier_wait(&SR_Data_Thread->barr);
	
	return SR_Data_Thread;
}