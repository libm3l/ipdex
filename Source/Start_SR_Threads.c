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
 *     Function Start_SR_Threads.c
 *
 *     Date: 2013-09-05
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
#include "Start_SR_Threads.h"
#include "SR_Threads.h"
#include "Server_Functions_Prt.h"


SR_thread_str_t *Start_SR_Threads(lmint_t n_threads){

	lmsize_t i;
	lmint_t pth_err;
	SR_thread_str_t  *SR_Thread;
	SR_thread_args_t *SR_DataArgs;  
	
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
/*
 * malloc the main node
 */
	if( (SR_Thread = (SR_thread_str_t *)malloc(sizeof(SR_thread_str_t))) == NULL)
		Perror("SR_Start_SR_Threads: SR_SR_Thread malloc");
/* 
 * malloc data in heap, will be used to share data between threads
 */
	if( (SR_Thread->data_threads = (pthread_t *)malloc(sizeof(pthread_t) * n_threads)) == NULL)
		Perror("Start_SR_Threads: SR_Thread->data_threads malloc");
	if( (SR_Thread->sockfd = (lmint_t *)malloc(n_threads * sizeof(lmint_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->sockfd malloc");
	if( (SR_Thread->EofBuff = (lmint_t *)malloc(n_threads * sizeof(lmint_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->EofBuff malloc");
	if( (SR_Thread->SR_mode = (lmchar_t *)malloc(n_threads * sizeof(lmchar_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->SR_mode malloc");
	if( (SR_Thread->thr_cntr = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->thr_cntr malloc");
	if( (SR_Thread->R_availth_counter = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->R_availth_counter");
	if( (SR_Thread->R_remainth_counter = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->R_remainth_counter");
	if( (SR_Thread->ngotten = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads:  SR_Thread->ngotten");
	if( (SR_Thread->buffer = (lmchar_t *)malloc( (MAXLINE+1)*sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads:  SR_Thread->buffer");
	if( (SR_Thread->sync = (lmint_t *)malloc(sizeof(lmint_t))) == NULL)
		Perror("Start_SR_Threads:  SR_Thread->sync");
	if( (SR_Thread->ATDT_mode = (lmchar_t *)malloc(n_threads * sizeof(lmchar_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->ATDT_mode malloc");
	if( (SR_Thread->KA_mode = (lmchar_t *)malloc(n_threads * sizeof(lmchar_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->KA_mode malloc");
	if( (SR_Thread->mode = (lmint_t *)malloc(sizeof(lmint_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->mode malloc");
	if( (SR_Thread->status_run = (lmint_t *)malloc(sizeof(lmint_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->status_run malloc");
/*
 * set initial value of socket descriptor to 0
 */
	for(i=0; i<n_threads;i++)
		SR_Thread->sockfd[i] = 0;
/*
 * initialize mutex and condition variable
 */
	Pthread_mutex_init(&SR_Thread->lock);
	Pthread_cond_init(&SR_Thread->dcond);
	
	Sem_init(&SR_Thread->sem, 0);
	Sem_init(&SR_Thread->sem_g, 0); 

	*SR_Thread->R_remainth_counter = 0;
	*SR_Thread->R_availth_counter = 0;
	
	*SR_Thread->status_run = 0;
/*
 * initialize sync data structure
 */
	if ( (SR_Thread->sync_loc  = (pt_sync_t *)malloc(sizeof(pt_sync_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->sync_loc");
	if ( (SR_Thread->sync_loc->nsync  = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->sync_loc->nsync");	
	if ( (SR_Thread->sync_loc->nthreads  = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Start_SR_Threads: SR_Thread->sync_loc->nthreads");
	
	Pthread_mutex_init(&SR_Thread->sync_loc->mutex);
	Pthread_mutex_init(&SR_Thread->sync_loc->block);
	Pthread_cond_init(&SR_Thread->sync_loc->condvar);
	Pthread_cond_init(&SR_Thread->sync_loc->last);
	
	*SR_Thread->sync_loc->nsync    = 0;
	*SR_Thread->sync_loc->nthreads = n_threads;
	*SR_Thread->thr_cntr = 0;
/*
 * spawn threads
 */	
	for(i=0; i < n_threads; i++){
		if( (SR_DataArgs = (SR_thread_args_t *)malloc(sizeof(SR_thread_args_t))) == NULL)
			Perror("Start_SR_Threads: SR_DataArgs malloc");	

		SR_DataArgs->plock 		= &SR_Thread->lock;
		SR_DataArgs->psem 		= &SR_Thread->sem;
		SR_DataArgs->psem_g 		= &SR_Thread->sem_g;
		SR_DataArgs->pdcond 		= &SR_Thread->dcond;
		SR_DataArgs->pSR_mode 		= SR_Thread->SR_mode;
		SR_DataArgs->psockfd 		= SR_Thread->sockfd;
		SR_DataArgs->pthr_cntr 		= SR_Thread->thr_cntr;
		SR_DataArgs->prcounter		= SR_Thread->R_remainth_counter;
		SR_DataArgs->pcounter		= SR_Thread->R_availth_counter;
		SR_DataArgs->pbuffer		= SR_Thread->buffer;
		SR_DataArgs->pngotten		= SR_Thread->ngotten;
		SR_DataArgs->pEofBuff		= SR_Thread->EofBuff;
		SR_DataArgs->pstatus_run	= SR_Thread->status_run;
		
		SR_DataArgs->psync			= SR_Thread->sync;
		SR_DataArgs->psync_loc 			= SR_Thread->sync_loc;
		SR_DataArgs->psync_loc->pnsync 		= SR_Thread->sync_loc->nsync;
		SR_DataArgs->psync_loc->pnthreads	= SR_Thread->sync_loc->nthreads;
		SR_DataArgs->psync_loc->pmutex 		= &SR_Thread->sync_loc->mutex;
		SR_DataArgs->psync_loc->pblock		= &SR_Thread->sync_loc->block;
		SR_DataArgs->psync_loc->pcondvar	= &SR_Thread->sync_loc->condvar;
		SR_DataArgs->psync_loc->plast		= &SR_Thread->sync_loc->last;
		
		SR_DataArgs->pSRt_mode 			= SR_Thread->mode;
/*
 * create thread
 */
		while ( (pth_err = pthread_create(&SR_Thread->data_threads[i], &attr, &SR_Threads,   (void *)SR_DataArgs)) != 0 && errno == EAGAIN);
		if(pth_err != 0)
			Perror("pthread_create()"); 
	}

	return SR_Thread;
}