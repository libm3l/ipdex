
#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"
#include "Start_SR_Threads.h"
#include "Start_SR_HubThread.h"
#include "SR_hub.h"
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
	SR_hub_thread_str_t  *SR_Hub_Thread;

	sem_t 		  	loc_sem;
	lmint_t pth_err;
/*
 * get my thread ID
 */
	MyThreadID = pthread_self();
	
	Pthread_mutex_lock(c->plock);
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
/*
 * set number of available local thread equal to number of readers + 1 writing
 */
	n_avail_loc_theads = n_rec_proc + 1;


		if( (THRStat_SFounds = m3l_Locate(c->Node, "./Data_Set/Thread_Status", "/*/*", (lmchar_t *)NULL)) == NULL){
			printf("Thread_Status: did not find any Thread_Status\n");
			m3l_DestroyFound(&THRStat_SFounds);
			exit(0);
		}
							
		TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
		Thread_Status = (lmint_t *)m3l_get_data_pointer(TmpNode);

		*Thread_Status = 0;
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
 * start SR_hub thread, do it before signaling the Server body signals that it is ready 
 * to go on. Before that, initialize local semaphore
 */
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
	SR_Hub_Thread->psem_g	=&SR_Threads->sem_g;	/* once the data transfer is finished increase increment of available data_threads */
	SR_Hub_Thread->plock	=c->plock;	
	SR_Hub_Thread->pcond	=c->pcond;
	SR_Hub_Thread->pcounter	=c->pcounter;
	SR_Hub_Thread->pn_avail_loc_theads	=&n_avail_loc_theads;
	SR_Hub_Thread->pn_rec_proc		=&n_rec_proc;
	SR_Hub_Thread->pThread_Status 	= Thread_Status;
	SR_Hub_Thread->pThread_Status 	= Thread_Status;


	while ( (pth_err = pthread_create(&SR_Hub_Thread->data_thread[0], NULL, &SR_hub,  SR_Hub_Thread)) != 0 && errno == EAGAIN);
	if(pth_err != 0)
		Perror("pthread_create()"); 





/*
 * wait on this barrier until main thread *Server_Body) sets value of counter and lock c->plock
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
			printf(" THREAD before sync %s in while loop %d  %d  %d  nsync %d socket %d\n", local_set_name, (*c->prcounter), (*c->pcounter), n_avail_loc_theads, *c->psync->pnthreads, *c->psocket);
/*
 * signal to the recently occupied thread which is about to be 
 * back in pool of available threads that it can increase number
 * of available threads
 */
			pt_sync(c->psync, 1, local_set_name);
			
			printf(" THREAD after sync %s in while loop %d  %d  %d  nsync %d\n", local_set_name, (*c->prcounter), (*c->pcounter), n_avail_loc_theads, *c->psync->pnthreads);

			Pthread_mutex_lock(c->plock);
			
			*c->psync_loc = 0;
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
					
					printf("    ===============    ===============  Data_Thread %s char %c is identified - socket # %d \n",c->pname_of_data_set,*c->pSR_mode, *c->psocket  );
/* 
 * when the thread is positively identified, decrement counter of available thread for next round of identification, 
 * once n_avail_loc_theads == 0 all SR threads arrived, leave do - while loop and decrement (*c->pcounter)--
 * ie. next arriving threads will not use this thread because it is alrady used
 */
					n_avail_loc_theads--;
					
// 					printf("Data_Thread %s char %c is identified - socket # %d  %d \n",c->pname_of_data_set,*c->pSR_mode, *c->psocket , n_avail_loc_theads );

					
/*
 * if number of available SR threads is 0, ie. all S and R requests for particular data set arrived
 * decrement number of available data sets
 */
					if(n_avail_loc_theads == 0){
/*
 * decrement number of available Data_Thread sets
 */
						(*c->pcounter)--;

						printf(" -------------------------------   Thread %lu named as '%s' received its SOCKET  %d\n", MyThreadID , local_set_name, *c->pcounter);
/*
 * set number of available processes for SR_Thread to  n_rec_proc+1 = number of Receivers + Sender
 */
						*SR_Threads->R_availth_counter = n_rec_proc+1;
/*
 * set Thread_Status to 1
 * 
 */
// 						if( (THRStat_SFounds = m3l_Locate(c->Node, "./Data_Set/Thread_Status", "/*/*", (lmchar_t *)NULL)) == NULL){
// 							printf("Thread_Status: did not find any Thread_Status\n");
// 							m3l_DestroyFound(&THRStat_SFounds);
// 							exit(0);
// 						}
// 							
// 						TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
// 						Thread_Status = (lmint_t *)m3l_get_data_pointer(TmpNode);
// 
						*Thread_Status = 1;
// 						m3l_DestroyFound(&THRStat_SFounds);
// 
// 						SR_Hub_Thread->pThread_Status 	= Thread_Status;

/*
 * post semaphore, the semaphore will be posted by SR_hub once the dta transfer is finished
 */

						printf(" POSTING SEMAPHORE \n");

						Sem_post(&loc_sem);
/*
 * Wait for succesfull SR_hub Sem_wait()
 */
// 						Sem_wait()
					}
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
				Sem_post(c->psem);  /* later it can be replaced by the same synchronization */
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
				while (*c->psync_loc == 0)
					Pthread_cond_wait(c->pdcond, c->plock);
			}

			Pthread_mutex_unlock(c->plock);	

		}while(n_avail_loc_theads != 0);  /* all connecting thread arrivied, ie. one Sender and n_rec_proc Receivers */

// 		printf(" -------------------------------   Thread %lu named as '%s' received its SOCKET  %d\n", MyThreadID , local_set_name, *c->pcounter);
// /*
//  * once all R-W threads are taken decrement counter of data_threads ie. Data_Thread->data_threads_availth_counter
//  */
// 		
// // 		for (ii = 0; ii< n_rec_proc + 1; ii++)
// // 			printf("job %d, socket %d, mode %s  %c\n", ii, SR_Threads->sockfd[ii],  local_set_name, *c->pcounter, SR_Threads->SR_mode[ii]);
// 
// /*
//  * wait until all SR_threads reach barrier, then start actual transfer of the data from S to R(s)
//  */
// // 		printf("Thread_Prt: after unlock 1\n");
// 		Pthread_barrier_wait(&SR_Threads->barr);
// // 		printf("Thread_Prt: Waiting on semaphore \n");
// /*
//  * once the data transfer is finished increase increment of available data_threads
// */
// 		Sem_wait(&SR_Threads->sem_g);
// // 		printf("TEST_... TRANFER FINISHED\n\n\n");
// 
// 		n_avail_loc_theads = n_rec_proc + 1;
// 
// // 		printf("Thread_Prt: %s lock 2\n", local_set_name);
// 
// 		Pthread_mutex_lock(c->plock);
// /*
//  * release thread, ie. set Thread_Status = 0
//  */
// 			*Thread_Status = 0;
// 			(*c->pcounter)++;
// /*
//  * if all threads were occupied, ie *Data_Threads->data_threads_availth_counter == *c->pcounter == 0
//  * the server is waiting for signal before the continuing with data process identification. 
//  * This is done in Server_Body before syncing with data threads
//  * If this happens, signal Server_Body that at least one data_thread is avaiable 
//  */
// 			if(*c->pcounter == 1)
// 				Pthread_cond_signal(c->pcond);
// 		
// // 			printf("Thread_Prt: %s unlock 2\n", local_set_name);
// 			Pthread_mutex_unlock(c->plock);
// // 			printf("Thread_Prt: after unlock 2\n");

			printf("GOING TO NEXT LOOP   %lu   %s  %d\n\n\n", MyThreadID , local_set_name, *c->pcounter);
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