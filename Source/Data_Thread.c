

#include "libm3l.h"
#include "Server_Header.h"
#include "Data_Thread.h"
#include "Thread_Prt.h"
#include "Server_Functions_Prt.h"


data_thread_str_t *Data_Thread(node_t *Gnode){

	lmsize_t i;
	lmint_t rcbarr, pth_err;
	find_t *SFounds;
	node_t  *Tmp = NULL, *LocNode;
	data_thread_str_t *Data_Thread;
	data_thread_args_t *DataArgs;   //, *Data_Glob_Args;lmchar
	
	if(Gnode == NULL){
		Warning("Data_Thread: NULL Gnode");
		return NULL;
	}
/*
 * malloc the main node
 */
	if( (Data_Thread = (data_thread_str_t *)malloc(sizeof(data_thread_str_t))) == NULL)
		Perror("Data_Thread: Data_Thread malloc");
/*
 * found how many data sets - defines how many data_threads to do
 */

	if( (SFounds = m3l_Locate(Gnode, "/COMM_DEF/Data_Sets/Data_Set", "/*/*/*", (lmchar_t *)NULL)) != NULL){
		
		Data_Thread->n_data_threads = m3l_get_Found_number(SFounds);
		
		if(Data_Thread->n_data_threads == 0){
			Error("Server: did not find any Data_set");
			m3l_DestroyFound(&SFounds);
		}
	}
	else
	{
		printf("Server: did not find any Data_set\n");
		exit(0);
	}
/* 
 * malloca data in heap
 */
	if( (Data_Thread->data_threads = (pthread_t *)malloc(sizeof(pthread_t) * Data_Thread->n_data_threads)) == NULL)
		Perror("Data_Thread: Data_Thread->data_threads malloc");	
	if( (Data_Thread->Data_Glob_Args = (data_thread_args_t *)malloc(sizeof(data_thread_args_t))) == NULL)
		Perror("Data_Thread: Data_Glob_Args malloc");
	if( (Data_Thread->data_threads_availth_counter = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Data_Thread: Data_Thread->data_threads_availth_counter");
	if( (Data_Thread->data_threads_remainth_counter = (lmsize_t *)malloc(sizeof(lmsize_t))) == NULL)
		Perror("Data_Thread: Data_Thread->data_threads_remainth_counter");	
	*Data_Thread->data_threads_availth_counter = 0;
	*Data_Thread->data_threads_remainth_counter = 0;
/*
 * initialize mutex, barrier and condition variable
 */
	Pthread_mutex_init(&Data_Thread->Data_Glob_Args->lock);
	Pthread_barrier_init(&Data_Thread->Data_Glob_Args->barr,  Data_Thread->n_data_threads + 1);
	Pthread_cond_init(&Data_Thread->Data_Glob_Args->cond);
	Pthread_cond_init(&Data_Thread->Data_Glob_Args->dcond);
	Sem_init(&Data_Thread->Data_Glob_Args->sem, 0);
/*
 * spawn threads
 */	
	for(i=0; i < Data_Thread->n_data_threads; i++){
		if( (DataArgs = (data_thread_args_t *)malloc(sizeof(data_thread_args_t))) == NULL)
			Perror("Data_Thread: DataArgs malloc");	
		
		DataArgs->Node  	= m3l_get_Found_node(SFounds, i);
		DataArgs->plock 	= &Data_Thread->Data_Glob_Args->lock;	
		DataArgs->psem 		= &Data_Thread->Data_Glob_Args->sem;	
		DataArgs->pbarr 	= &Data_Thread->Data_Glob_Args->barr;	
		DataArgs->pcond 	= &Data_Thread->Data_Glob_Args->cond;	
		DataArgs->pdcond 	= &Data_Thread->Data_Glob_Args->dcond;	
		DataArgs->PVARIABLE  	= &Data_Thread->Data_Glob_Args->VARIABLE;	
		DataArgs->psocket    	= &Data_Thread->Data_Glob_Args->socket;	
		DataArgs->pcounter    	=  Data_Thread->data_threads_availth_counter;
		DataArgs->prcounter    	=  Data_Thread->data_threads_remainth_counter;
/*
 * create thread
 */
//         if ( pthread_create(&Data_Thread->data_threads[i], NULL, &Data_Threads,  DataArgs) != 0 ){
//             if(errno == EAGAIN) /* If Interrupted system call, restart - back to while ()  UNP V1 p124  */
//                 continue;
//         else
//             Perror("pthread_create()");
//         }

		while ( ( pth_err = pthread_create(&Data_Thread->data_threads[i], NULL, &Data_Threads,  DataArgs)) != 0 && errno == EAGAIN);
		if(pth_err != 0)
		Perror("pthread_create()"); 

/*
 * create a node
 */
	}
/*
 * wait on this barrier until all threads are created - the barriers are _waited on in Data_Threads and this is the last one
 * makes sure we leave the function after all threads are created
 */
	Pthread_barrier_wait(&Data_Thread->Data_Glob_Args->barr);
	m3l_DestroyFound(&SFounds);
	
	return Data_Thread;
}