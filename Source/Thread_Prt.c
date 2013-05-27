
#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"
#include "Start_SR_Threads.h"

#include "Thread_Prt.h"

void *Data_Threads(void *arg)
{
	data_thread_args_t *c = (data_thread_args_t *)arg;
	lmint_t local_cntr;
	node_t *List, *TmpNode;
	
	lmsize_t len, len1, *data_rec_proc, n_rec_proc, n_avail_loc_theads, i;
	
	lmchar_t *data_set_name, local_set_name[MAX_NAME_LENGTH];
	find_t *SFounds, *THRStat_SFounds;
	
	pthread_t  MyThreadID;
	
	lmint_t ii, *Thread_Status;
	
	SR_thread_str_t *SR_Threads;
/*
 * get my thread ID
 */
	MyThreadID = pthread_self();
	
	Pthread_mutex_lock(c->plock);
// 		printf("\n\n In THREAD %lu\n\n", MyThreadID);
		if(m3l_Cat(c->Node, "--all", "-L", "-P", "*",   (lmchar_t *)NULL) != 0)
			Warning("CatData");
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
				len = m3l_get_List_totdim(List)-1;
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
				n_rec_proc = data_rec_proc[0];
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			printf("Thread_Prt: Receiving_Processes not found\n");
		}
		
	Pthread_mutex_unlock(c->plock);
/*
 * set number of available local thread equal to number of readers + 1 writing
 */
	n_avail_loc_theads = n_rec_proc + 1;
/*
 * spawn SR_thread, wait until all ST_threads for this data set are spawned. Use semafore for syncing
 */
	if(  (SR_Threads = Start_SR_Threads(n_avail_loc_theads)) == NULL)
		Perror("Thread_Prt: Start_SR_Threads error");
/*
 * all ST_threads were spawend
 */
	Sem_wait(&SR_Threads->sem_g);
	Sem_destroy(&SR_Threads->sem_g);
/* 
 * set the counter 0
 * this counter will be used by each SR_Thread to get the values of the socket and SR_mode
 */
	*SR_Threads->thr_cntr=0;
/*
 * wait on this barrier until all threads are started
 * the barrier is called n-times (n=number of Data_Threads + 1) where the last call is made
 * from Data_Thread which spawns this thread
 * Once this barrier is reached, the main threads initializes some variables
 * and waits on another barrier
 */	
	Pthread_barrier_wait(c->pbarr);
/*
 * wait on this barrier until main thread sets value of counter and lock c->plock
 * the last call to _wait() is done in the main function after returning back from Data_Threads = Data_Thread(Gnode)
 * Once this barrier is reached, the main threads starts accepting requests from client
 * This barrier makes sure the initial start is properly done
 */
	Pthread_barrier_wait(c->pbarr);
	
	while(1){
		
// 		printf(" THREAD %s in while loop %d  %d  %d\n", local_set_name, (*c->prcounter), (*c->pcounter), n_avail_loc_theads);
		local_cntr = 0;
/*
 * start identifying threads once identified, leave do loop
 */
	do{
/*
 * if already went through do loop, wait here at sync point until all threads are here
 */
// 			printf(" Waiting for gate - child \n");
		
			pt_sync(c->psync);
// 			printf(" After gate - child \n");
			
			Pthread_mutex_lock(c->plock);
			*c->psync_loc = 0;
/*
 * wait for data sent by main thread
 */
// 			while (*c->prcounter == 0)
// 				Pthread_cond_wait(c->pcond, c->plock);

 /* 
  * decrement counter of thread  which will check condition, used for syncing all threads before 
  * going back to caller function
  */
			(*c->prcounter)--;

			if(*c->pretval == 0){
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
 * when the thread is positively identified, decrement counter of available thread for next round of identification, 
 * once n_avail_loc_theads == 0 all SR threads arrived, leave do - while loop and decrement (*c->pcounter)--
 * ie. next arriving threads will not use this thread because it is alrady used
 */
					n_avail_loc_theads--;
				}
			}
/*
 * synchronized all threads at the end, the last thread will broadcast
 */	
			if(*c->prcounter == 0){
/* 	
 * the last thread, broadcast
 * indicate this is the last thread
 */
				*c->psync_loc = 1;
				Pthread_cond_broadcast(c->pdcond);
				
// 				printf(" NAVAIL THREADS %lu %d\n", MyThreadID, n_avail_loc_theads);
/*
 * if number of available SR threads is 0, ie. all S and R requests for particular data set arrived
 * decrement number of available data sets
 */
				if(n_avail_loc_theads == 0)(*c->pcounter)--;
				Sem_post(c->psem);  /* later it can be replaced by the same synchronization */
// 				*c->psync_loc = 0;
/* 
 * unlock semaphore in the main program so that another loop can start
 */
			}
			else{
/*
 * still some threads working, wait for them
 *
 * if number of available SR threads is 0, ie. all S and R requests for particular data set arrived
 * decrement number of available data sets
 */				
				if(n_avail_loc_theads == 0)(*c->pcounter)--;
				while (*c->psync_loc == 0)
					Pthread_cond_wait(c->pdcond, c->plock);
			}
			
			Pthread_mutex_unlock(c->plock);	
			
// 			printf(" %lu Unlocking semaphore \n", MyThreadID);

		
		}while(n_avail_loc_theads != 0);  /* all connecting thread arrivied, ie. one Sender and n_rec_proc Receivers */

		printf(" -------------------------------   Thread %lu named as '%s' received its SOCKET  %d\n", MyThreadID , local_set_name, *c->pcounter);
/*
 * once all R-W threads are taken decrement counter of data_threads ie. Data_Thread->data_threads_availth_counter
 */
		
// 		for (ii = 0; ii< n_rec_proc + 1; ii++)
// 			printf("job %d, socket %d, mode %c\n", ii, SR_Threads->sockfd[ii], SR_Threads->SR_mode[ii]);


		Pthread_mutex_lock(c->plock);	
			*SR_Threads->R_availth_counter = n_rec_proc+1;
/*
 * set Thread_Status to 1
 * 
 */
			if( (THRStat_SFounds = m3l_Locate(c->Node, "./Data_Set/Thread_Status", "/*/*", (lmchar_t *)NULL)) == NULL){
				printf("Thread_Status: did not find any Thread_Status\n");
				m3l_DestroyFound(&THRStat_SFounds);
				exit(0);
			}
		
			TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
			Thread_Status = (lmint_t *)m3l_get_data_pointer(TmpNode);
			*Thread_Status = 1;
			
		Pthread_mutex_unlock(c->plock);
/*
 * wait until all SR_threads reach barrier, then start actual transfer of the data from S to R(s)
 */

		Pthread_barrier_wait(&SR_Threads->barr);
/*
 * once the data transfer is finished increase increment of available data_threads
 */
		Sem_wait(&SR_Threads->sem_g);
		printf("TEST_... TRANFER FINISHED\n\n\n");

		n_avail_loc_theads = n_rec_proc + 1;
		Pthread_mutex_lock(c->plock);
/*
 * release thread, ie. set Thread_Status = 0
 */
			*Thread_Status = 0;
	 		m3l_DestroyFound(&THRStat_SFounds);
		
			(*c->pcounter)++;
// 			printf(" Status of the JOB is %d  %ld   %d\n", *Thread_Status , *c->pcounter, n_avail_loc_theads);
/*
 * if all threads were occupied, ie *Data_Threads->data_threads_availth_counter == *c->pcounter == 0
 * the server is waiting for signal before the continuing with data process identification. 
 * This is done in Server_Body before syncing with data threads
 * If this happens, signal Server_Body that at least one data_thread is avaiable 
 */
			if(*c->pcounter == 1)
				Pthread_cond_signal(c->pcond);

		Pthread_mutex_unlock(c->plock);
/* 
 * set the counter 0
 * this counter will be used by each SR_Thread to get the values of the socket and SR_mode
 */		
		*SR_Threads->thr_cntr=0;
// 		printf("GOING TO NEXT LOOP\n\n\n");
		}
	
	
// 	printf(" Leaving WHILE \n");
	
	
/*
 * join threads and release memmory
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
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);

	return NULL;

}