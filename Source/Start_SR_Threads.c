

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
		Perror("Start_SR_Threads:  SR_Data_Thread->buffer");/*
 * initialize mutex, barrier and condition variable
 */
	Pthread_mutex_init(&SR_Data_Thread->lock);
	Pthread_mutex_init(&SR_Data_Thread->lock_g);
	Pthread_cond_init(&SR_Data_Thread->cond);
	Pthread_cond_init(&SR_Data_Thread->cond_g);
	Pthread_cond_init(&SR_Data_Thread->dcond);
/* 
 * initialize barrier, the coutner should be the same as number of R_threads, will be used to sync all R_threads 
 */
// 	Pthread_barrier_init(&SR_Data_Thread->barr,  n_threads-1);
	Sem_init(&SR_Data_Thread->sem, 0);
	Sem_init(&SR_Data_Thread->sem_g, 0);
/*
 * spawn threads
 */	
	for(i=0; i < n_threads; i++){
		if( (SR_DataArgs = (SR_thread_args_t *)malloc(sizeof(SR_thread_args_t))) == NULL)
			Perror("Start_SR_Threads: SR_DataArgs malloc");	

		SR_DataArgs->plock 		= &SR_Data_Thread->lock;	
		SR_DataArgs->plock_g 		= &SR_Data_Thread->lock_g;	
		SR_DataArgs->psem 		= &SR_Data_Thread->sem;	
		SR_DataArgs->psem_g 		= &SR_Data_Thread->sem_g;	
		SR_DataArgs->pcond 		= &SR_Data_Thread->cond;	
		SR_DataArgs->pcond_g 		= &SR_Data_Thread->cond_g;	
		SR_DataArgs->pdcond 		= &SR_Data_Thread->dcond;	
		SR_DataArgs->pSR_mode 		= SR_Data_Thread->SR_mode;	
		SR_DataArgs->psockfd 		= SR_Data_Thread->sockfd;	
		SR_DataArgs->pthr_cntr 		= SR_Data_Thread->thr_cntr;
		SR_DataArgs->prcounter		= SR_Data_Thread->R_remainth_counter;
		SR_DataArgs->pcounter		= SR_Data_Thread->R_availth_counter;
		SR_DataArgs->pbuffer		= SR_Data_Thread->buffer;
		SR_DataArgs->pngotten		= SR_Data_Thread->ngotten;
		SR_DataArgs->pEofBuff		= SR_Data_Thread->EofBuff;
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
	Sem_post(&SR_Data_Thread->sem);
	Pthread_mutex_unlock(&SR_Data_Thread->lock);
	
	return SR_Data_Thread;
}