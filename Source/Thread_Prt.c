
#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"

#include "Thread_Prt.h"

void *Data_Threads(void *arg)
{
	data_thread_args_t *c = (data_thread_args_t *)arg;
	lmint_t rcbarr;
	lmint_t received;
	node_t *List;
	
	lmsize_t len, *data_rec_proc, n_rec_proc, n_avail_loc_theads;
	
	lmchar_t *data_set_name;
	find_t *SFounds;
	
	pthread_t  MyThreadID;
/*
 * get my thread ID
 */
	MyThreadID = pthread_self();
	
	Pthread_mutex_lock(c->plock);
		printf("\n\n In THREAD %lu\n\n", MyThreadID);
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
 * find number of processes which will read the data set
 * there is only one writing processes
 */
		if( (SFounds = m3l_Locate(c->Node, "./Data_Set/Receiving_Processes", "./*/*",  (lmchar_t *)NULL)) != NULL){
			
			if( m3l_get_Found_number(SFounds) != 1)
				Error("Thread_Prt: Only one Receiving_Processes per Data_Set allowed");
/* 
 * pointer to list of found nodes
 */
				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
					Error("NULThread_Prt: L Receiving_Processes");
			
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
		
		printf(" Name of thread is %s, length is %ld, nrec is %d\n", data_set_name, len, n_rec_proc);
		
	Pthread_mutex_unlock(c->plock);
/*
 * set number of available local thread equal to number of readers + 1 writing
 */
		n_avail_loc_theads = n_rec_proc;
	
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

	
	
/*
 * start identifying threads once identified, leave do loop
 */
	do
	{
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
				
		if(*c->PVARIABLE == MyThreadID){
			(*c->pcounter)--;   /* if the thread is positively identified, decrement counter of available thread for next round of identification */
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
	
	
	
/* 
 * unlock semaphore in the main program so that another loop can start
 */
		Sem_post(c->psem);

	printf(" -------------------------------------------------   Thread %lu received its SOCKET\n", MyThreadID );
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);
		
 	return NULL;
}