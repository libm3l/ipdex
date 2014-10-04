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
#include "Server_Functions_Prt.h"
#include "Start_SR_Threads.h"
#include "Start_SR_HubThread.h"
#include "SR_hub.h"
#include "Data_Thread.h"

void *Data_Threads(void *arg)
{
/* 
 * this is a thread which is associated to a specific data set name
 * the thread identifies incomming requests by comparing data set name with the 
 * arriving data set name and if possitive, decrement number of available threads 
 * for the particular data set (the numbet of available threads at the beginning = S + number_o_R threads
 * Once the number of available threads == 0, (ie. all S + R threads arrived) the 
 * thread notifies SR_Hub which synchronizes the S_R transfer (SR_Data_Threads)
 *
 * Number of all active Data_Thread == number of transferred data sets + 1. The values is set in 
 * Data_Thread (*Data_Thread->sync->nthreads = *Data_Thread->n_data_threads + 1;
 */
	data_thread_args_t *c = (data_thread_args_t *)arg;
	lmint_t local_cntr;
	node_t *List, *TmpNode;
	
	lmsize_t len, len1, *data_rec_proc, n_rec_proc, n_avail_loc_theads, i;
	
	lmchar_t *data_set_name, local_set_name[MAX_NAME_LENGTH];
	find_t *SFounds, *THRStat_SFounds;

	lmint_t  *Thread_Status, *Thread_S_Status, round;
	lmsize_t *Thread_R_Status;
	
	SR_thread_str_t *SR_Threads;
	SR_hub_thread_str_t  *SR_Hub_Thread;

	sem_t loc_sem;
	
	opts_t *Popts, opts;

	Popts = &opts;
	
	m3l_set_Find(&Popts);
	round = 0;
	len = 0;
/*
 * get my thread ID
 */
// 	MyThreadID = pthread_self();
	
	Pthread_mutex_lock(c->plock);
// 		if(m3l_Cat(c->Node, "--all", "-L", "-P", "*",   (lmchar_t *)NULL) != 0)
// 			Warning("CatData");
/*
 * find name of data set
 * if it does not exist or more then one data set is found, give error message
 */
		if( (SFounds = m3l_locate(c->Node, "./Channel/Name_of_Channel", "./*/*", Popts)) != NULL){
			if( m3l_get_Found_number(SFounds) != 1)
				Error("Data_Thread: Only one Name_of_Channel per Channel allowed");
/* 
 * pointer to list of found nodes
 */
				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
					Error("Data_Thread: NULL Name_of_Channel");
/*
 * get the name of channel (connection) this thread will take care of
 * save the name in local variable and data_thread_args_t->data_thread_int_str_t->name_of_channel
 * variable. It is used to identify which thread should be later terminated if
 * user requires closing specific name connection
 */
				data_set_name = m3l_get_data_pointer(List);
				if( (len = m3l_get_List_totdim(List)-1) < 1)
					Error("Data_Thread: too short name of data set");
				if( snprintf(local_set_name, MAX_NAME_LENGTH,"%s",data_set_name) < 0)
					Perror("snprintf");
				local_set_name[len] ='\0';
				if( snprintf(c->pData_Str->name_of_channel, MAX_NAME_LENGTH,"%s",data_set_name) < 0)
					Perror("snprintf");
				c->pData_Str->name_of_channel[len] ='\0';
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			Error("Data_Thread: Name_of_Channel not found\n");
		}
/*
 * find name of process which will read the data set
 * there is only one writing processes
 */
		if( (SFounds = m3l_locate(c->Node, "./Channel/Receiving_Processes", "./*/*", Popts)) != NULL){

			if( m3l_get_Found_number(SFounds) != 1)
				Error("Data_Thread: Only one Receiving_Processes set per Channel allowed");
/* 
 * pointer to list of found nodes
 */
				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
					Error("NULData_Thread: Missing Receiving_Processes set");

				data_rec_proc = (lmsize_t *)m3l_get_data_pointer(List);
				if(  ( n_rec_proc = data_rec_proc[0]) < 1){
					printf("Data_Thread - name of data set is %s\n", local_set_name);
					Error("Data_Thread - number of receiving processes too low");
				}
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			printf("Data_Thread: Receiving_Processes not found\n");
		}
/*
 * set number of available local thread equal to number of readers + 1 writing
 */
		n_avail_loc_theads = n_rec_proc + 1;

		if( (THRStat_SFounds = m3l_locate(c->Node, "./Channel/Thread_Status", "./*/*", Popts)) == NULL){
			printf("Data_Thread: did not find any Thread_Status\n");
			m3l_DestroyFound(&THRStat_SFounds);
			exit(0);
		}
/*
 * find pointer the Thread_Status, Thread_S_Status and Thread_R_Status
 */
		TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
		Thread_Status = (lmint_t *)m3l_get_data_pointer(TmpNode);

		*Thread_Status = 0;
		m3l_DestroyFound(&THRStat_SFounds);
		
		if( (THRStat_SFounds = m3l_locate(c->Node, "./Channel/S_Status", "./*/*", Popts)) == NULL){
			printf("Data_Thread: did not find any S_Status\n");
			m3l_DestroyFound(&THRStat_SFounds);
			exit(0);
		}

		TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
		Thread_S_Status = (lmint_t *)m3l_get_data_pointer(TmpNode);
		*Thread_S_Status = 0;
		m3l_DestroyFound(&THRStat_SFounds);

		if( (THRStat_SFounds = m3l_locate(c->Node, "./Channel/R_Status", "./*/*", Popts)) == NULL){
			printf("Thread_Status: did not find any R_Status\n");
			m3l_DestroyFound(&THRStat_SFounds);
			exit(0);
		}

		TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
		Thread_R_Status = (lmsize_t *)m3l_get_data_pointer(TmpNode);
		*Thread_R_Status = 0;
		m3l_DestroyFound(&THRStat_SFounds);
/*
 * spawn SR_thread, wait until all SR_threads for this data set are spawned. Use semafore for syncing
 */
	if(  (SR_Threads = Start_SR_Threads(n_avail_loc_theads)) == NULL)
		Perror("Data_Thread: Start_SR_Threads error");

	*SR_Threads->R_availth_counter = n_rec_proc+1;
/*
 * start SR_hub thread, do it before signaling the Server body signals that it is ready 
 * to go on. Before that, initialize local semaphore
 */
	Sem_init(&loc_sem, 0);
	
	if ( (SR_Hub_Thread = Start_SR_HubThread(SR_Threads, c, &n_avail_loc_theads, &n_rec_proc, Thread_Status, 
		&loc_sem, Thread_S_Status, Thread_R_Status)) == NULL)
		Error("Data_Thread: Start_SR_HubThread");
/*
 * if the threads are opened at the startup of the 
 * server, ie. from the definition file, 
 * set round = 1 so that they do not post the semafore in *c->pcheckdata == 100
 * The semaphore is used only by threads which are dynamically added
 */
	if(*c->pcheckdata < 0)round = 1;

	Pthread_mutex_unlock(c->plock);
/*
 * sync all SR_Threads and SR_hub so that Data_Thread goes further once they 
 * are all spawned. Without that there were problems with case 200 where Data_Thread 
 * sometimes deleted List before SR_Hub started, upon start SR_hub needs to identify 
 * some values from the list.
 * 
 * The value of processes which are synced on this pt_sync_mod is increased by 2, ie.
 * number of SR_Data_Threads + SR_Hub + Data_Thread
 */
	pt_sync_mod(SR_Threads->sync_loc, 0, 2);
/*
 * initialization phase is over, now there is a while(1) loop in whith the threads are identified
 * according to their name. The while loop is terminated in case 200, ie. when requested to be terminated
 */
// 	while(*c->pData_Str->status_run==1){
	while(1){
		
		local_cntr = 0;
/*
 * start identifying threads once identified, leave do loop
 */
		do{
/*
 * if already went through do loop, wait here at sync point until all threads are here
 * set the value of for syncing thread to number of data sets + 1 (it. sync all Data_Thread (n_data_threads) + 
 * 1 for Server_Body (one synchronization point is in Server_Body
 */
			pt_sync(c->psync);
/*
 * if arriving request was not sys_link_ type of request
 * check which connection arrived
 */
			switch(*c->pcheckdata){
				case 0:

					Pthread_mutex_lock(c->plock);
/*
 * if pretval == 0 and Thread_Status == 0 (thread available for pooling)
 */

					if(*Thread_Status == 0 && *c->pretval == 0){
/* 
 * if the data thread was not identified yet
 */
						len1 = strlen(c->pname_of_data_set);
						if(len1 == len && strncmp(c->pname_of_data_set,local_set_name, len) == 0){
/*
 * save socket number and mode of the jobe (S, R), increase increment and set return value to 1
 */				
							SR_Threads->sockfd[local_cntr]	= *c->psocket;
							SR_Threads->SR_mode[local_cntr] = *c->pSR_mode;
							local_cntr++;
							*c->pretval = 1;
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
							}
						}
					}
/*
 * synchronized all threads at the end
 */	
					Pthread_mutex_unlock(c->plock);

					pt_sync(c->psync);
					break;

				case 100:
/*
 * request was _sys_link_ request
 */
/*
 * if the first round, ie. Data_Thread was added now (for this thread round == 0),
 * post semaphore. The semaphore signalizes Server_Body that it can enter
 * the pt_sync. For case of adding thread, the Server body has just one pt_sync instead of two.
 * The first missing pt_sync is then provided by the newly spawned Data_Thread for which round == 0. 
 * This semaphore makes sure that the pt_sync in this thread is executed before pt_sync
 * in Server_Body so that the Server_Body pt_sync_mod is used in connenction with 
 * second pt_sync in Data_Threads
 */
					if(round == 0){
						Sem_post(c->psem);
				
/*
 * set return value to 1 and increase number of total Data_Threads
 */
						Pthread_mutex_lock(c->plock);
							*c->pretval = 1;
							(*c->prcounter)++;
						Pthread_mutex_unlock(c->plock);
					}
/*
 * sync all threads, ie. Setvet_Body + all Data_Threads
 * because tere is a new DATA_Thread add temporarily 1 and then increase number of synced threads for next round
 * the psync->incrm was set to 1 by Server_Body
 */
					pt_sync_mod(c->psync, 1, 1);
					break;

				case 200:
/*
 * delete thread
 *
 * check how many threads already arrived
 */
// 				if(n_avail_loc_theads < n_rec_proc + 1){
// 					some processes already arrived
//					this can be done by checking local_cntr, if not 0, some sets already arrived
// 				}
				
					Pthread_mutex_lock(c->plock);
					if(*Thread_Status == 0 && *c->pretval == 0){
						len1 = strlen(c->pname_of_data_set);
						if(len1 == len && strncmp(c->pname_of_data_set,local_set_name, len) == 0){
/*
 * set SR_mode to T as terminate
 */
						for(i=0; i < n_rec_proc + 1; i++)
							SR_Threads->SR_mode[i] = 'T';
/*
 * this thread is to be removed
 */
							(*c->prcounter)--;
							*c->pretval = 1;
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
							Sem_post(&loc_sem);
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
							pt_sync_mod(c->psync,1, 0);
							goto END;
						}
					}
					Pthread_mutex_unlock(c->plock);
					pt_sync_mod(c->psync,1, 0);
					break;

				default:
					Error("SR_Data_Threads: Wrong mode - check KeepAlive and ATDT specifications");
				break;
			}

/*
 * indicate that thread went through the identfication 
 * loop at least once. 
 * This variable is used to identify newly added threads
 */
			round = 1;

		}while(n_avail_loc_theads != 0);  /* all connecting thread arrivied, ie. one Sender and n_rec_proc Receivers */
		
		n_avail_loc_theads = n_rec_proc + 1;
/*
 * SR_hub sem_wait(c->psem) for this semaphore 
 */
			Sem_post(&loc_sem);
/*
 * now this thread signalled its own SR_hub that all connections arrived and SR_hub start synchronizing all 
 * SR_Data_Threads which are taking care of actual data transfer from S to R threads
 * 
 * This thread does not need to wait for it and goes to while(1) loop again and waits on synchronization 
 * point pt_sync(c->psync). The thread is classified as taken (*Thread_Status == 1).
 * The reason why it is done so is that the number of threads being synchronized has to be still the same.
 * Thats why the synchronization of the S-R transfer is done by SR_hub instead of this thread.
 * Once the SR_hub is finished with all data transfer it sets *Thread_Status = 0 (in SR_hub: *c->pThread_Status = 0)
 * so that this thread can be again considered as being available.
 * 
 * NOTE: before this thread took case of syncyng SR_Data_Threads instead of SR_hub. The problem was that it needed to 
 * decrement a number of synchronized threads which (the value used in pt_sync(c->psync).
 * Some of the threads were already waiting and it gave rise to dead-lock
 */
	}
END:
/*
 * join SR_hub and release memory
 * join this thread before SR_Threads, it is because SR_Hub is waiting for final semaphore 
 * from SR_Data_Threads. By joining SR_Hub_Thread after the SR_Threads were freed
 * caused SR_Hub using already unuloated sempahore. By freeing malloced memory 
 * after joinign thread, this problem is avoided
 */
	if( pthread_join(SR_Hub_Thread->data_thread[0], NULL) != 0)
		Error(" Joining thread failed");
/*
 * join SR_Threads and release memory
 */
	for(i=0; i< n_avail_loc_theads; i++){
		if( pthread_join(SR_Threads->data_threads[i], NULL) != 0)
			Error(" Joining thread failed");
	}
	
	Pthread_mutex_destroy(&SR_Threads->lock);
	Pthread_cond_destroy(&SR_Threads->dcond);
	Sem_destroy(&SR_Threads->sem);

	free(SR_Threads->data_threads);
	free(SR_Threads->SR_mode);
	free(SR_Threads->ATDT_mode);
	free(SR_Threads->KA_mode);
	free(SR_Threads->mode);
	free(SR_Threads->thr_cntr);
	free(SR_Threads->sockfd);
	free(SR_Threads->buffer);
	free(SR_Threads->R_availth_counter);
	free(SR_Threads->R_remainth_counter);
	free(SR_Threads->ngotten);
	free(SR_Threads->EofBuff);
	free(SR_Threads->sync);
	free(SR_Threads->status_run);
	
	free(SR_Threads->sync_loc->nsync);
	free(SR_Threads->sync_loc->nthreads);
	Pthread_mutex_destroy(&SR_Threads->sync_loc->mutex);
	Pthread_mutex_destroy(&SR_Threads->sync_loc->block);
	Pthread_cond_destroy(&SR_Threads->sync_loc->condvar);
	Pthread_cond_destroy(&SR_Threads->sync_loc->last);
	free(SR_Threads->sync_loc);

	free(SR_Threads);
/*
 * free local semaphore
 */
	Sem_destroy(&loc_sem);
	
	free(SR_Hub_Thread->data_thread);
	free(SR_Hub_Thread);
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c->pData_Str);
	free(c);

	return NULL;
}