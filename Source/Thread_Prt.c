
#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"

#include "Thread_Prt.h"

void *Data_Threads(void *arg)
{
	data_thread_args_t *c = (data_thread_args_t *)arg;
	lmint_t local_socket;
	lmint_t received;
	node_t *List;
	
	lmsize_t len, *data_rec_proc, n_rec_proc, n_avail_loc_theads;
	
	lmchar_t *data_set_name, local_set_name[MAX_NAME_LENGTH];
	find_t *SFounds;
	
	pthread_t  MyThreadID;
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
 * NOTE imlement R_W data thread startup
 */
	
/*
 * wait on this barrier until all threads are started
 * the barrier is called n-times (n=number of Data_Threads + 1) where the last call is made
 * from Data_Thread which spawns this thread
 */	
	Pthread_barrier_wait(c->pbarr);
/*
 * wait on this barrier until main thread sets value of counter and lock c->plock
 * the last call to _wait() is done in the main function after returning back from Data_Threads = Data_Thread(Gnode)
 */
	Pthread_barrier_wait(c->pbarr);
	
	
	while(1){
		
		printf(" THREAD %s in while loop\n", local_set_name);
/*
 * start identifying threads once identified, leave do loop
 */
	do{
/*
 * if already went through do loop, wait here at sync point until all threads are here
 */
			received = 0;		
			Pthread_mutex_lock(c->plock);
/*
 * wait for data sent by main thread
 */
			while (*c->prcounter == 0)
				Pthread_cond_wait(c->pcond, c->plock);

			(*c->prcounter)--;   /* decrement counter of thread  which will check condition*/

			if(strncmp(c->pname_of_data_set,local_set_name, len) == 0){ 
				
				local_socket = *c->psocket;
/*
 * NOTE: implement taking thread
 */




/* 
 * when the thread is positively identified, decrement counter of available thread for next round of identification, 
 * omce n_avail_loc_theads == 0 decrement (*c->pcounter)--
 */
				n_avail_loc_theads--;
				received = 1;        /* indicate thread received connection */
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
/* 
 * unlock semaphore in the main program so that another loop can start
 */
// 			Sem_post(c->psem);
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
		
		}while(received != 1);

		printf(" -------------------------------   Thread %lu named as '%s' received its SOCKET\n", MyThreadID , local_set_name);
		printf("                                   Thread name is '%s' SM_mode is '%c'\n", c->pname_of_data_set, *c->pSR_mode);
/*
 * once all R-W threads are taken decrement counter of data_threads ie. Data_Thread->data_threads_availth_counter
 */
		if (n_avail_loc_theads == 0){
			Pthread_mutex_lock(c->plock);	
			(*c->pcounter)--;
// 			printf(" COUUNTER OF AVAILABLE THREADS IS UUUU  %d    %d  %s\n", (*c->pcounter), n_avail_loc_theads, local_set_name);

			Pthread_mutex_unlock(c->plock);
		}
/* 
 * unlock semaphore in the main program so that another loop can start
 */
		Sem_post(c->psem);
		
		
		
		
/*
 * NOTE implement R-W thread sharing data
 */

/*
 * once the data transfer is finished increase increment of available data_threads
 */
		if (n_avail_loc_theads == 0){
			
			
			
			
			
			n_avail_loc_theads = n_rec_proc;
			Pthread_mutex_lock(c->plock);	
				(*c->pcounter)++;
// 			printf(" COUUNTER OF AVAILABLE THREADS IS %d    %d   %s\n", (*c->pcounter), n_avail_loc_theads, local_set_name);
			Pthread_mutex_unlock(c->plock);	
		}
		
		
	}
	
	
	printf(" Leaving WHILE \n");
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);
		
 	return NULL;
}