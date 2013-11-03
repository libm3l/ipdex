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
 *     Function Thread_Prt.c
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
#include "Thread_Prt.h"

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
 * Number of all active Thread_Prt == number of transferred data sets + 1. The values is set in 
 * Data_Thread (*Data_Thread->sync->nthreads = Data_Thread->n_data_threads + 1;
 */
	data_thread_args_t *c = (data_thread_args_t *)arg;
	lmint_t local_cntr;
	node_t *List, *TmpNode;
	
	lmsize_t len, len1, *data_rec_proc, n_rec_proc, n_avail_loc_theads, i;
	
	lmchar_t *data_set_name, local_set_name[MAX_NAME_LENGTH];
	find_t *SFounds, *THRStat_SFounds;
	
	pthread_t  MyThreadID;
	
	lmint_t ii, *Thread_Status, *Thread_S_Status;
	lmsize_t *Thread_R_Status;
	
	SR_thread_str_t *SR_Threads;
	SR_hub_thread_str_t  *SR_Hub_Thread;

	sem_t	loc_sem;
	lmint_t pth_err;
/*
 * get my thread ID
 */
	MyThreadID = pthread_self();
	
	Pthread_mutex_lock(c->plock);
// 		if(m3l_Cat(c->Node, "--all", "-L", "-P", "*",   (lmchar_t *)NULL) != 0)
// 			Warning("CatData");
/*
 * find name of data set
 * if it does not exist or more then one data set is found, give error message
 */
		if( (SFounds = m3l_Locate(c->Node, "./Data_Set/Name_of_Data_Set", "./*/*",  (lmchar_t *)NULL)) != NULL){
			
			if( m3l_get_Found_number(SFounds) != 1)
				Error("Thread_Prt: Only one Name_of_Data_Set per Data_Set allowed");
/* 
 * pointer to list of found nodes
 */
				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
					Error("Thread_Prt: NULL Name_of_Data_Set");
			
				data_set_name = m3l_get_data_pointer(List);
				if( (len = m3l_get_List_totdim(List)-1) < 1)
					Error("Thread_Prt: too short name of data set");
				if( snprintf(local_set_name, MAX_NAME_LENGTH,"%s",data_set_name) < 0)
					Perror("snprintf");
				local_set_name[len] ='\0';
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			Error("Thread_Prt: Name_of_Data_Set not found\n");
		}
/*
 * find name of process which will read the data set
 * there is only one writing processes
 */
		if( (SFounds = m3l_Locate(c->Node, "./Data_Set/Receiving_Processes", "./*/*",  (lmchar_t *)NULL)) != NULL){

			if( m3l_get_Found_number(SFounds) != 1)
				Error("Thread_Prt: Only one Receiving_Processes per Data_Set allowed");
/* 
 * pointer to list of found nodes
 */
				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
					Error("NULThread_Prt: Missing Receiving_Processes");

				data_rec_proc = (lmsize_t *)m3l_get_data_pointer(List);
				if(  ( n_rec_proc = data_rec_proc[0]) < 1){
					printf("Thread_Prt - name of data set is %s\n", local_set_name);
					Error("Thread_Prt - number of receiving processes too low");
				}
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			printf("Thread_Prt: Receiving_Processes not found\n");
		}
/*
 * set number of available local thread equal to number of readers + 1 writing
 */
		n_avail_loc_theads = n_rec_proc + 1;

		if( (THRStat_SFounds = m3l_Locate(c->Node, "./Data_Set/Thread_Status", "/*/*", (lmchar_t *)NULL)) == NULL){
			printf("Thread_Prt: did not find any Thread_Status\n");
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
		
		if( (THRStat_SFounds = m3l_Locate(c->Node, "./Data_Set/S_Status", "/*/*", (lmchar_t *)NULL)) == NULL){
			printf("Thread_Prt: did not find any S_Status\n");
			m3l_DestroyFound(&THRStat_SFounds);
			exit(0);
		}

		TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
		Thread_S_Status = (lmint_t *)m3l_get_data_pointer(TmpNode);
		*Thread_S_Status = 0;
		m3l_DestroyFound(&THRStat_SFounds);

		if( (THRStat_SFounds = m3l_Locate(c->Node, "./Data_Set/R_Status", "/*/*", (lmchar_t *)NULL)) == NULL){
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
		Perror("Thread_Prt: Start_SR_Threads error");
/*
 * all ST_threads were spawend
 */
	Sem_wait(&SR_Threads->sem_g);
	Sem_destroy(&SR_Threads->sem_g);

	*SR_Threads->R_availth_counter = n_rec_proc+1;
/*
 * start SR_hub thread, do it before signaling the Server body signals that it is ready 
 * to go on. Before that, initialize local semaphore
 */

// 	Pthread_mutex_lock(c->plock);

	Sem_init(&loc_sem, 0);

// 	if(  (SR_Hub_Thread = Start_SR_HubThread(SR_Threads, c, &n_avail_loc_theads, &n_rec_proc, Thread_Status, loc_sem)) == NULL)
// 		Perror("Thread_Prt: Start_SR_HubThreads error");
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
	SR_Hub_Thread->pbarr 	= &SR_Threads->barr;		/* wait until all SR_threads reach barrier, then start actual transfer of the data from S to R(s) */
	SR_Hub_Thread->psem 	= &loc_sem;
	SR_Hub_Thread->psem_g	= &SR_Threads->sem_g;	/* once the data transfer is finished increase increment of available data_threads */
	SR_Hub_Thread->plock	= c->plock;
	SR_Hub_Thread->pcond	= c->pcond;
	SR_Hub_Thread->pcounter	= c->pcounter;
	SR_Hub_Thread->pn_avail_loc_theads	= &n_avail_loc_theads;
	SR_Hub_Thread->pn_rec_proc		= &n_rec_proc;
	SR_Hub_Thread->pThread_Status 	= Thread_Status;
	SR_Hub_Thread->pThread_S_Status	= Thread_S_Status;
	SR_Hub_Thread->pThread_R_Status = Thread_R_Status;
	SR_Hub_Thread->prcounter 	= c->prcounter;
	SR_Hub_Thread->psockfd		= SR_Threads->sockfd;
	SR_Hub_Thread->pList		= c->Node;
	SR_Hub_Thread->pATDT_mode	= SR_Threads->ATDT_mode;  /* associate pointer, the values will be filled in SR_hub */
	SR_Hub_Thread->pKA_mode		= SR_Threads->KA_mode;    /* associate pointer, the values will be filled in SR_hub */
	SR_Hub_Thread->pSRh_mode	= SR_Threads->mode;    /* associate pointer, the values will be filled in SR_hub */
	SR_Hub_Thread->pEOFC_ENDh	= SR_Threads->EOFC_END;    /* if KeepAlive == C, use this value to signal the 
								socket is closed */

	while ( (pth_err = pthread_create(&SR_Hub_Thread->data_thread[0], NULL, &SR_hub,  SR_Hub_Thread)) != 0 && errno == EAGAIN);
	if(pth_err != 0)
		Perror("pthread_create()"); 

	Pthread_mutex_unlock(c->plock);
/*
 * wait on this barrier until all threads are started
 * the barrier is called n-times (n=number of Data_Threads + 1) where the last call is made
 * from Data_Thread which spawns this thread
 * Once this barrier is reached, the main threads initializes some variables
 * and waits on another barrier
 */	
	Pthread_barrier_wait(c->pbarr);
/*
 * wait on this barrier until main thread *Server_Body) sets value of counter and lock c->plock
 * the last call to _wait() is done in the main function after returning back from Data_Threads = Data_Thread(Gnode)
 * Once this barrier is reached, the main threads starts accepting requests from client
 * This barrier makes sure the initial start is properly done
 */
	Pthread_barrier_wait(c->pbarr);
	
	while(1){
		
		local_cntr = 0;
/*
 * start identifying threads once identified, leave do loop
 */
		do{
/*
 * if already went through do loop, wait here at sync point until all threads are here
 * set the value of for syncing thread to number of data sets + 1 (it. sync all Thread_Prt (n_data_threads) + 
 * 1 for Server_Body (one synchronization point is in Server_Body
 */
			pt_sync(c->psync);

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
 * if thread status is S, set Thread_S_Status = 1 to block any other arriving S thread from 
 * being considered until the tranfer is finished (Check_Request.c and SR_Hub.c)
 */
					if( *c->pSR_mode == 'S'){
						*Thread_S_Status = 1;
					}
/*
 * if thread status is R, increment Thread_R_Status. Once the counter reaches value of requested
 * R_Thread any other arriving R_thread will be blocked until the tranfer is finished (Check_Request.c and SR_Hub.c)
 */
					else if(*c->pSR_mode == 'R'){
						*Thread_R_Status++;
					}
					else
						Error("Thread_Prt: Wrong SR_mode");
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
  * decrement counter of thread  which will check condition, used for syncing all threads before 
  * going back to caller function
  */
						(*c->prcounter)--;
					}
				}
			}
/*
 * synchronized all threads at the end, the last thread will broadcast
 */	
			Pthread_mutex_unlock(c->plock);	

			pt_sync(c->psync);

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
/*
 * join SR_Threads and release memory
 */
	for(i=0; i< n_avail_loc_theads; i++)
		if( pthread_join(SR_Threads->data_threads[i], NULL) != 0)
			Error(" Joining thread failed");
				
	Pthread_mutex_destroy(&SR_Threads->lock);
  	Pthread_barrier_destroy(&SR_Threads->barr);
	Pthread_cond_destroy(&SR_Threads->dcond);
 	Sem_destroy(&SR_Threads->sem);
		
	free(SR_Threads->data_threads);
	free(SR_Threads->SR_mode);
	free(SR_Threads->ATDT_mode);
	free(SR_Threads->KA_mode);
	free(SR_Threads->mode);
	free(SR_Threads->EOFC_END);
	free(SR_Threads->thr_cntr);
	free(SR_Threads->sockfd);
	free(SR_Threads->buffer);
	free(SR_Threads->R_availth_counter);
	free(SR_Threads->R_remainth_counter);
	free(SR_Threads->ngotten);
	free(SR_Threads->EofBuff);
	free(SR_Threads->sync);
		
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
/*
 * join SR_hub and release memory
 */
	if( pthread_join(SR_Hub_Thread->data_thread[0], NULL) != 0)
		Error(" Joining thread failed");
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);

	return NULL;
}