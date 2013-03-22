
#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"
#include "Start_SR_Threads.h"

#include "Thread_Prt.h"

void *Data_Threads(void *arg)
{
	data_thread_args_t *c = (data_thread_args_t *)arg;
	lmint_t local_cntr;
	node_t *List;
	
	lmsize_t len, *data_rec_proc, n_rec_proc, n_avail_loc_theads, i;
	
	lmchar_t *data_set_name, local_set_name[MAX_NAME_LENGTH];
	find_t *SFounds;
	
	pthread_t  MyThreadID;
	
	SR_thread_str_t *SR_Threads;
/*
 * get my thread ID
 */
	MyThreadID = pthread_self();
	
	Pthread_mutex_lock(c->plock);
// 		printf("\n\n In THREAD %lu\n\n", MyThreadID);
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
 * all ST_threads were spawend, destroy senmaphore
 */
	Sem_wait(&SR_Threads->sem);
	Sem_destroy(&SR_Threads->sem);
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
 */
	Pthread_barrier_wait(c->pbarr);
	
	
	while(1){
		
		printf(" THREAD %s in while loop\n", local_set_name);
		local_cntr = 0;
/*
 * start identifying threads once identified, leave do loop
 */
	do{
/*
 * if already went through do loop, wait here at sync point until all threads are here
 */
			Pthread_mutex_lock(c->plock);
/*
 * wait for data sent by main thread
 */
			while (*c->prcounter == 0)
				Pthread_cond_wait(c->pcond, c->plock);
 /* 
  * decrement counter of thread  which will check condition, used for syncing all threads before 
  * going back to caller function
  */
			(*c->prcounter)--; 

			if(strncmp(c->pname_of_data_set,local_set_name, len) == 0){
/*
 * save socket number and mode of the jobe (S, R) and increase increment
 */				
				SR_Threads->sockfd[local_cntr]  = *c->psocket;
				SR_Threads->SR_mode[local_cntr] = *c->pSR_mode;
				local_cntr++;
/* 
 * when the thread is positively identified, decrement counter of available thread for next round of identification, 
 * once n_avail_loc_theads == 0 all SR threads arrived, leave do - while loop and decrement (*c->pcounter)--
 * ie. next arriving threads will not use this thread because it is alrady used
 */
				n_avail_loc_theads--;
			}
/*
 * synchronized all threads at the end, the last thread will broadcast
 */	
			if(*c->prcounter == 0){
/* 	
 * the last thread, broadcast
 * indicate this is the last thread
 */
				pthread_cond_broadcast(c->pdcond);
				Sem_post(c->psem);
/* 
 * unlock semaphore in the main program so that another loop can start
 */
			}
			else{
/*
 * still some threads working, wait for them
 * indicate this is waiting thread
 */
// 			while (*c->prcounter != 0)
				Pthread_cond_wait(c->pdcond, c->plock);
			}
			
			Pthread_mutex_unlock(c->plock);	
		
		}while(n_avail_loc_theads != 0);  /* all connecting thread arrivied, ie. one Sender and n_rec_proc Receivers */

		
		printf(" -------------------------------   Thread %lu named as '%s' received its SOCKET\n", MyThreadID , local_set_name);
		printf("                                   Thread name is '%s' SM_mode is '%c'\n", c->pname_of_data_set, *c->pSR_mode);
/*
 * once all R-W threads are taken decrement counter of data_threads ie. Data_Thread->data_threads_availth_counter
 */
		Pthread_mutex_lock(c->plock);	
			(*c->pcounter)--;
// 			SR_Data_Thread->R_remainth_counter = n_rec_proc; 
		Pthread_mutex_unlock(c->plock);
/* 
 * unlock semaphore in the main program so that another loop can start
 */
		
		
		
		
/*
 * NOTE implement R-W thread sharing data
 */

/*
 * once the data transfer is finished increase increment of available data_threads
 */
		n_avail_loc_theads = n_rec_proc + 1;
		Pthread_mutex_lock(c->plock);	
			(*c->pcounter)++;
// 			printf(" COUUNTER OF AVAILABLE THREADS IS %d    %d   %s\n", (*c->pcounter), n_avail_loc_theads, local_set_name);
		Pthread_mutex_unlock(c->plock);
/* 
 * set the counter 0
 * this counter will be used by each SR_Thread to get the values of the socket and SR_mode
 */		
		*SR_Threads->thr_cntr=0;
		
		
	}
	
	
	printf(" Leaving WHILE \n");
	
	
	
	/*
 * join threads and release memmory
 */
		for(i=0; i< n_avail_loc_theads; i++)
			if( pthread_join(SR_Threads->data_threads[i], NULL) != 0)
				Error(" Joining thread failed");
				
		Pthread_mutex_destroy(&SR_Threads->lock);
 		Pthread_barrier_destroy(&SR_Threads->barr);
		Pthread_cond_destroy(&SR_Threads->cond);
		Pthread_cond_destroy(&SR_Threads->dcond);
// 		Sem_destroy(&SR_Threads->sem);
		
		free(SR_Threads->data_threads);
		free(SR_Threads->SR_mode);
		free(SR_Threads->thr_cntr);
		free(SR_Threads->sockfd);
// 		free(SR_Threads->R_availth_counter);
		free(SR_Threads->R_remainth_counter);
		free(SR_Threads);

/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);
		
 	return NULL;
}