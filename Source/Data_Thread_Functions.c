
#include "libm3l.h"
#include "lsipdx_header.h"
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





/*
 * delete thread
 *
 * check how many threads already arrived
 */
// // 				if(n_avail_loc_theads < n_rec_proc + 1){
// // 					some processes already arrived
// //					this can be done by checking local_cntr, if not 0, some sets already arrived
// // 				}
// 					Pthread_mutex_lock(c->plock);
// 					if(*c->pretval == 0){
// 						len1 = strlen(c->pname_of_data_set);
// 						if(len1 == len && strncmp(c->pname_of_data_set,local_set_name, len) == 0){
// 		
// 							if(*c->pData_Str->status_run == 2 ){
// /*
//  * at least one client for this Data thread arrived
//  */
// 								if(c->pPopts->opt_f == 'f'){
// 									
// 									
// 									for(i=0; i < n_rec_proc + 1 - n_avail_loc_theads; i++){
// 										if(close(SR_Threads->sockfd[i]) == -1)
// 											Perror("Data_Thread close");
// 										SR_Threads->sockfd[i] = 0;
// 									}
// 									*c->pretval = 4;
// 									goto HERE;
// 								}
// 								else{
// /*
//  * set sync job increment to 0, it was set by server body to -1
//  * it is because we are not going to close any
//  * Data_Thread so the number os synced jobs remains unchanged
//  */
// 									*c->psync->incrm = 0;
// /*,
//  * notify server by setting retval to 2 and ignore request to close connection
//  */
// 									*c->pretval = 2;
// 									Pthread_mutex_unlock(c->plock);
//  /*
//   * pt_sync_mod(c->psync,1, 0) can be used because it will not affect anything
//   */
// 									pt_sync_mod(c->psync,1, 0);
// 									break;
// 								}
// 							}
// 							else if(*c->pData_Str->status_run == 3 ){
// /*
//  * all SR_Data_Threads for this Data_Thread arrived, ignore request
//  */
// /*
//  * set sync job increment to 0, it was set by server body to -1
//  * it is because we are not going to close any
//  * Data_Thread so the number os synced jobs remains unchanged
//  */
// 									*c->psync->incrm = 0;
// /*,
//  * notify server by setting retval to 2 and ignore request to close connection
//  */
// 									*c->pretval = 3;
// 									Pthread_mutex_unlock(c->plock);
//  /*
//   * pt_sync_mod(c->psync,1, 0) can be used because it will not affect anything
//   */
// 									pt_sync_mod(c->psync,1, 0);
// 									break;
// 							}
// /*
//  * set SR_mode to T as terminate
//  */
// 							*c->pretval = 1;
// HERE:
// 							for(i=0; i < n_rec_proc + 1; i++)
// 								SR_Threads->SR_mode[i] = 'T';
// /*
//  * this thread is to be removed
//  */
// 							(*c->prcounter)--;
// // 							*c->pretval = 1;
// 							*c->pThreadID = pthread_self();
// 							*c->pData_Str->status_run = 0;
// /*
//  * set status run for SR_Hub and SR_Data_Thread to 0, ie. terminate while loops
//  */
// 							*SR_Threads->status_run = 0;
// /*
//  * Singal SR_hub sem_wait(c->psem) for this semaphore, ie. as if all SR threads
//  * arrived. For SR threads in normal "working" state this is done 
//  * after }while(n_avail_loc_theads != 0) loop
//  */
// 							Sem_post(&loc_sem);
// /*
//  * delete this node, it will remove the item from the buffer
//  */
// 							if( m3l_Umount(&c->Node) != 1)
// 								Perror("m3l_Umount");
// /*
//  * no leave do - while(n_avail_loc_theads != 0) loop 
//  * the value of status_run is set to 0 
//  * so the outer while loop (status_run == 1) will be automatically left too
//  * This sequence may change later when SR_Data_Threads and SR_Hub are terminated
//  */
// 							Pthread_mutex_unlock(c->plock);
// /*
//  * the last executed Data_Thread will lower number of synced jobs upon leaving
//  * the c->psync->incrm was set by Server_Body to -1
//  */
// 							pt_sync_mod(c->psync,1, 0);
// 							goto END;
// 						}
// 					}
// 
// 					Pthread_mutex_unlock(c->plock);
// /*
//  * the last executed Data_Thread will lower number of synced jobs upon leaving
//  * the c->psync->incrm was set by Server_Body to -1
//  */
// 					pt_sync_mod(c->psync,1, 0);
// 					break;