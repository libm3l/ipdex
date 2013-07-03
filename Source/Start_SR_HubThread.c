

#include "libm3l.h"
#include "Server_Header.h"
#include "Start_SR_HubThread.h"
#include "SR_hub.h"
#include "Server_Functions_Prt.h"
#include "SR_hub.h"


SR_hub_thread_str_t *Start_SR_HubThread(SR_thread_str_t *SR_Threads, data_thread_args_t *c, lmsize_t *n_avail_loc_theads, lmsize_t  *n_rec_proc, lmint_t *Thread_Status, sem_t loc_sem){

	lmsize_t i;
	lmint_t pth_err;
	SR_hub_thread_str_t  *SR_Hub_Thread;
/*
 * malloc the main node
 */
	if( (SR_Hub_Thread = (SR_hub_thread_str_t *)malloc(sizeof(SR_hub_thread_str_t))) == NULL)
		Perror("SR_Start_SR_Threads: SR_Hub_Thread malloc");
/* 
 * malloc data in heap, will be used to share data between threads
 */
	if( (SR_Hub_Thread->data_thread = (pthread_t *)malloc(sizeof(pthread_t) )) == NULL)
		Perror("Start_SR_Threads: SR_Hub_Thread->data_thread malloc");
/*
 * associate values in SR_Hub_Thread 
 */
	SR_Hub_Thread->pbarr 		= &SR_Threads->barr;		/* wait until all SR_threads reach barrier, then start actual transfer of the data from S to R(s) */
	SR_Hub_Thread->psem 		= &loc_sem;
	SR_Hub_Thread->psem_g		= &SR_Threads->sem_g;	/* once the data transfer is finished increase increment of available data_threads */
	SR_Hub_Thread->plock		= c->plock;	
	SR_Hub_Thread->pcond		= c->pcond;
	SR_Hub_Thread->pcounter		= c->pcounter;
	SR_Hub_Thread->pn_avail_loc_theads	= n_avail_loc_theads;
	SR_Hub_Thread->pn_rec_proc		= n_rec_proc;
	SR_Hub_Thread->pThread_Status 	= Thread_Status;
/*
 * create thread
 */
	while ( (pth_err = pthread_create(&SR_Hub_Thread->data_thread[0], NULL, &SR_hub,  SR_Hub_Thread)) != 0 && errno == EAGAIN);
	if(pth_err != 0)
		Perror("pthread_create()"); 

	return SR_Hub_Thread;
}