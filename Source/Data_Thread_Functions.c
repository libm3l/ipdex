
#include "libm3l.h"
#include "lsipdx_header.h"

#include "Server_Functions_Prt.h"
#include "Data_Thread_Functions.h"

lmsize_t Data_Thread_Case_0(data_thread_args_t *c, SR_thread_str_t *SR_Threads, lmsize_t N_avail_loc_theads, lmsize_t *local_cntr,
	lmint_t  *Thread_Status, lmint_t *Thread_S_Status, lmsize_t *Thread_R_Status, lmsize_t n_rec_proc){

	lmsize_t n_avail_loc_theads;
	
	n_avail_loc_theads = N_avail_loc_theads;
/*
 * save socket number and mode of the jobe (S, R), increase increment and set return value to 1
 */
	SR_Threads->sockfd[*local_cntr]	= *c->psocket;
	SR_Threads->SR_mode[*local_cntr] = *c->pSR_mode;
	(*local_cntr)++;
	*c->pretval = 1;

// 	if(*c->pSR_mode  == 'S')sockfd_R = *c->psocket;
/*
 * indicate that at least one request arrived for this thread
 */
	*c->pData_Str->status_run = 2;
/*
 * if thread status is S, set Thread_S_Status = 1 to block any other arriving S thread from 
 * being considered until the tranfer is finished (Check_Request.c and SR_Hub.c)
 */
	if( *c->pSR_mode == 'S') *Thread_S_Status = 1;
/*
 * if thread status is R, increment Thread_R_Status. Once the counter reaches value of requested
 * R_Thread any other arriving R_thread will be blocked until the tranfer is finished (Check_Request.c and SR_Hub.c)
 */
	else if(*c->pSR_mode == 'R'){
		(*Thread_R_Status)++;
	}
	else
		Error("Data_Thread: Wrong SR_mode");
/* 
 * when the thread is positively identified, decrement counter of available thread for next round of identification, 
 * once n_avail_loc_theads == 0 all SR threads arrived, leave do - while loop and decrement (*c->pcounter)--
 * ie. next arriving threads will not use this thread because it is alrady used
 */
		n_avail_loc_theads--;
/*
 * if number of available SR threads is 0, ie. all S and R requests for particular data set arrived
 * decrement number of available data sets
 */
		if(n_avail_loc_theads == 0){
/*
 * set number of available processes for SR_Thread to  n_rec_proc+1 = number of Receivers + Sender
 */
		*SR_Threads->R_availth_counter = n_rec_proc+1;
/*
 * set Thread_Status to 1
 */
		*Thread_Status = 1;
 /* 
  * decrement counter of thread  which will check condition, used to identify how many threads are still active
  * use in case of "fixed" communication setup - see SR_Hub sequence: if(*c->pcounter == 1); Pthread_cond_signal(c->pcond);
  */
		(*c->prcounter)--;
		*c->pData_Str->status_run = 3;
	}
	return n_avail_loc_theads;
}




void Data_Thread_Case_200(data_thread_args_t *c, SR_thread_str_t *SR_Threads, lmsize_t n_rec_proc, sem_t *loc_sem ){
	
	lmsize_t i;

	for(i=0; i < n_rec_proc + 1; i++)
		SR_Threads->SR_mode[i] = 'T';
/*
 * this thread is to be removed
 */
		(*c->prcounter)--;
// 							*c->pretval = 1;
		*c->pThreadID = pthread_self();
		*c->pData_Str->status_run = 0;
/*
 * set status run for SR_Hub and SR_Data_Thread to 0, ie. terminate while loops
 */
		*SR_Threads->status_run = 0;
/*
 * Singal SR_hub sem_wait(c->psem) for this semaphore, ie. as if all SR threads
 * arrived. For SR threads in normal "working" state this is done 
 * after }while(n_avail_loc_theads != 0) loop
 */
		Sem_post(loc_sem);
/*
 * delete this node, it will remove the item from the buffer
 */
		if( m3l_Umount(&c->Node) != 1)
			Perror("m3l_Umount");
/*
 * no leave do - while(n_avail_loc_theads != 0) loop 
 * the value of status_run is set to 0 
 * so the outer while loop (status_run == 1) will be automatically left too
 * This sequence may change later when SR_Data_Threads and SR_Hub are terminated
 */
		Pthread_mutex_unlock(c->plock);
/*
 * the last executed Data_Thread will lower number of synced jobs upon leaving
 * the c->psync->incrm was set by Server_Body to -1
 */
		pt_sync_mod(c->psync,1, 0);
	return;
}