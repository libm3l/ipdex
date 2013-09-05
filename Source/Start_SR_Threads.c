

#include "libm3l.h"
#include "Server_Header.h"
#include "Start_SR_Threads.h"
#include "SR_Data_Threads.h"
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
	if( (SR_Data_Thread->sockfd = (lmint_t *)malloc(n_threads * sizeof(lmint_t))) == NULL)
		Perror("Start_SR_Threads: SR_Data_Thread->sockfd malloc");
	if( (SR_Data_Thread->EofBuff = (lmint_t *)malloc(n_threads * sizeof(lmint_t))) == NULL)
		Perror("Start_SR_Threads: SR_Data_Thread->EofBuff malloc");
	if( (SR_Data_Thread->SR_mode = (lmchar_t *)malloc(n_threads * sizeof(lmchar_t))) == NULL)
		Perror("Start_SR_Threads: SR_Data_Thread->SR_mode malloc");
	if( (SR_Data_Thread->thr_cntr = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads: SR_Data_Thread->thr_cntr malloc");
	if( (SR_Data_Thread->R_availth_counter = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads: SR_Data_Thread->R_availth_counter");
	if( (SR_Data_Thread->R_remainth_counter = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads: SR_Data_Thread->R_remainth_counter");
	if( (SR_Data_Thread->ngotten = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads:  SR_Data_Thread->ngotten");
	if( (SR_Data_Thread->buffer = (lmchar_t *)malloc( (MAXLINE+1)*sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads:  SR_Data_Thread->buffer");
	if( (SR_Data_Thread->sync = (lmint_t *)malloc(sizeof(lmint_t))) == NULL)
		Perror("Start_SR_Threads:  SR_Data_Thread->sync");
	if( (SR_Data_Thread->ATDT_mode = (lmchar_t *)malloc(n_threads * sizeof(lmchar_t))) == NULL)
		Perror("Start_SR_Threads: SR_Data_Thread->ATDT_mode malloc");
	if( (SR_Data_Thread->KA_mode = (lmchar_t *)malloc(n_threads * sizeof(lmchar_t))) == NULL)
		Perror("Start_SR_Threads: SR_Data_Thread->KA_mode malloc");
	if( (SR_Data_Thread->mode = (lmint_t *)malloc(sizeof(lmint_t))) == NULL)
		Perror("Start_SR_Threads: SR_Data_Thread->mode malloc");
/*
 * initialize mutex, barrier and condition variable
 */
	Pthread_mutex_init(&SR_Data_Thread->lock);
	Pthread_cond_init(&SR_Data_Thread->dcond);
/* 
 * initialize barrier, the coutner should be the same as number of R_threads, will be used to sync all R_threads 
 */
 	Pthread_barrier_init(&SR_Data_Thread->barr,  n_threads+1);
	
	Sem_init(&SR_Data_Thread->sem, 0);
	Sem_init(&SR_Data_Thread->sem_g, 0); 

	*SR_Data_Thread->R_remainth_counter = 0;
	*SR_Data_Thread->R_availth_counter = 0;
/*
 * initialize sync data structure
 */
	if ( (SR_Data_Thread->sync_loc  = (pt_sync_t *)malloc(sizeof(pt_sync_t))) == NULL)
		Perror("Data_Thread: Data_Thread->sync");
	if ( (SR_Data_Thread->sync_loc->nsync  = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Data_Thread: Data_Thread->sybc->nsync");	
	if ( (SR_Data_Thread->sync_loc->nthreads  = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Data_Thread: Data_Thread->sybc->nthreads");
	Pthread_mutex_init(&SR_Data_Thread->sync_loc->mutex);
	Pthread_mutex_init(&SR_Data_Thread->sync_loc->block);
	Pthread_cond_init(&SR_Data_Thread->sync_loc->condvar);
	Pthread_cond_init(&SR_Data_Thread->sync_loc->last);
	*SR_Data_Thread->sync_loc->nsync    = 0;
	*SR_Data_Thread->sync_loc->nthreads = n_threads;
	*SR_Data_Thread->thr_cntr = 0;
/*
 * spawn threads
 */	
	for(i=0; i < n_threads; i++){
		if( (SR_DataArgs = (SR_thread_args_t *)malloc(sizeof(SR_thread_args_t))) == NULL)
			Perror("Start_SR_Threads: SR_DataArgs malloc");	

		SR_DataArgs->plock 		= &SR_Data_Thread->lock;	
		SR_DataArgs->psem 		= &SR_Data_Thread->sem;
		SR_DataArgs->psem_g 		= &SR_Data_Thread->sem_g;
		SR_DataArgs->pdcond 		= &SR_Data_Thread->dcond;
		SR_DataArgs->pbarr	 	= &SR_Data_Thread->barr;
		SR_DataArgs->pSR_mode 		= SR_Data_Thread->SR_mode;
		SR_DataArgs->psockfd 		= SR_Data_Thread->sockfd;
		SR_DataArgs->pthr_cntr 		= SR_Data_Thread->thr_cntr;
		SR_DataArgs->prcounter		= SR_Data_Thread->R_remainth_counter;
		SR_DataArgs->pcounter		= SR_Data_Thread->R_availth_counter;
		SR_DataArgs->pbuffer		= SR_Data_Thread->buffer;
		SR_DataArgs->pngotten		= SR_Data_Thread->ngotten;
		SR_DataArgs->pEofBuff		= SR_Data_Thread->EofBuff;
		SR_DataArgs->psync		= SR_Data_Thread->sync;
		
		SR_DataArgs->psync_loc 		= SR_Data_Thread->sync_loc;
		SR_DataArgs->psync_loc->pnsync 	= SR_Data_Thread->sync_loc->nsync;
		SR_DataArgs->psync_loc->pnthreads= SR_Data_Thread->sync_loc->nthreads;
		SR_DataArgs->psync_loc->pmutex 	= &SR_Data_Thread->sync_loc->mutex;
		SR_DataArgs->psync_loc->pblock	= &SR_Data_Thread->sync_loc->block;
		SR_DataArgs->psync_loc->pcondvar= &SR_Data_Thread->sync_loc->condvar;
		SR_DataArgs->psync_loc->plast	= &SR_Data_Thread->sync_loc->last;
		
// 		SR_DataArgs->pATDT_mode 	= SR_Data_Thread->ATDT_mode;
// 		SR_DataArgs->pKA_mode 		= SR_Data_Thread->KA_mode;
		SR_DataArgs->pSRt_mode 		= SR_Data_Thread->mode;
/*
 * create thread
 */
		while ( (pth_err = pthread_create(&SR_Data_Thread->data_threads[i], NULL, &SR_Data_Threads,  SR_DataArgs)) != 0 && errno == EAGAIN);
		if(pth_err != 0)
			Perror("pthread_create()"); 
	}
/*
 * when all threads are spawned, signal Thread_Prt functions about it
 */
	Sem_post(&SR_Data_Thread->sem_g);
	
	return SR_Data_Thread;
}