

#include "libm3l.h"
#include "Server_Header.h"
#include "Data_Thread.h"
#include "Thread_Prt.h"


data_thread_str_t *Data_Thread(node_t *Gnode){

	lmsize_t i;
	lmint_t rcbarr;
	find_t *SFounds;
	node_t  *Tmp = NULL, *LocNode;
	data_thread_str_t *Data_Thread;
	data_thread_args_t *DataArgs;   //, *Data_Glob_Args;	

	
	
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
 * malloc strucutre
 */
	
	if( (Data_Thread->data_threads = (pthread_t *)malloc(sizeof(pthread_t) * Data_Thread->n_data_threads)) == NULL)
		Perror("Data_Thread: Data_Thread->data_threads malloc");
	
	if( (Data_Thread->Data_Glob_Args = (data_thread_args_t *)malloc(sizeof(data_thread_args_t))) == NULL)
		Perror("Data_Thread: Data_Glob_Args malloc");
	
/*
 * initialize mutex, barrier and condition variable
 */

	Pthread_mutex_init(&Data_Thread->Data_Glob_Args->lock);
	Pthread_barrier_init(&Data_Thread->Data_Glob_Args->barr,  Data_Thread->n_data_threads + 1);
	Pthread_cond_init(&Data_Thread->Data_Glob_Args->cond);
// 	if( pthread_cond_init(&Data_Thread->Data_Glob_Args->cond, NULL) != 0)
// 		Perror("Data_Thread: pthread_cond_init()");
/*
 * spawn threads
 */	
	for(i=0; i < Data_Thread->n_data_threads; i++){
		if( (DataArgs = (data_thread_args_t *)malloc(sizeof(data_thread_args_t))) == NULL)
			Perror("Data_Thread: DataArgs malloc");	
		
		DataArgs->Node = m3l_get_Found_node(SFounds, i);
		DataArgs->plock = &Data_Thread->Data_Glob_Args->lock;	
// 		DataArgs->plockC = &Data_Thread->Data_Glob_Args->lockC;	
		DataArgs->pbarr = &Data_Thread->Data_Glob_Args->barr;	
		DataArgs->pbarrC = &Data_Thread->Data_Glob_Args->barrC;	
		DataArgs->pcond = &Data_Thread->Data_Glob_Args->cond;	
		DataArgs->pcondition = &Data_Thread->Data_Glob_Args->condition;	
		DataArgs->PVARIABLE = &Data_Thread->Data_Glob_Args->VARIABLE;	
/*
 * create thread
 */
		if ( pthread_create(&Data_Thread->data_threads[i], NULL, &Data_Threads,  DataArgs) != 0){
// 			if(errno = EAGAIN) /* If Interrupted system call, restart - back to while ()  UNP V1 p124  */
// 				continue;
// 		else
			Perror("pthread_create()");
		}
		printf(" Created thread ID is %lu\n", Data_Thread->data_threads[i]);

/*
 * create a node
 */
	}
	
	
	printf(" Waiting on barrier\n");
	Pthread_barrier_wait(&Data_Thread->Data_Glob_Args->barr);
	m3l_DestroyFound(&SFounds);

// 	if (pthread_mutex_destroy(&Data_Thread->Data_Glob_Args->lock) != 0)
// 		Perror("Data_Thread: pthread_mutex_destroy()");
// 	
// 	if( pthread_barrier_destroy(&Data_Thread->Data_Glob_Args->barr) != 0)
// 		Perror("Data_Thread: pthread_barrier_destroy()");
//	
	
	printf(" Waiting on barrier over\n");

	
	return Data_Thread;
}