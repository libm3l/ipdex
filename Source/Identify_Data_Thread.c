#include "libm3l.h"
#include "Server_Header.h"
#include "Identify_Data_Thread.h"
#include "Server_Functions_Prt.h"


lmint_t Identify_Data_Thread(data_thread_str_t **Data_Threads){
	
	lmsize_t i;
	
	Pthread_mutex_lock(&(*Data_Threads)->Data_Glob_Args->lock);
/*
 * save socket descriptor
 */
// 	(*Data_Threads)->Data_Glob_Args->socket =  newsockfd;
/*
 * set the counter to number of available threads
 */
	*(*Data_Threads)->data_threads_availth_counter  =  (*Data_Threads)->n_data_threads;
 /*
  * at the beginning the coutner of remainign threads is equal to 
  * number of available threads
  * this coutner is used to synchronize all threads at the end when they went on each other
  * it is reset every iterational step
  */
	*(*Data_Threads)->data_threads_remainth_counter  = *(*Data_Threads)->data_threads_availth_counter;
/* 
 * this is identification iof the thread
 */
	(*Data_Threads)->Data_Glob_Args->VARIABLE     =  (*Data_Threads)->data_threads[0];
/*
 * wait for barrier, indicating all threads in Data_Thread were created
 * the _wait on this barrier is the second_wait call in Data_Thread for each thread and this is the last one
 * makes sure we do not start Data_Thread before some of the data which are needed are filled abd mutex is locked - see 
 * fours lines above
 */
	Pthread_barrier_wait(&(*Data_Threads)->Data_Glob_Args->barr);
/*
 * give condition signal to Data_Thread that they can start - see part of code in Data_Thread
 *              -   while (*c->prcounter == 0)
 *		- 	Pthread_cond_wait(c->pcond, c->plock);
 */
	Pthread_cond_broadcast(&(*Data_Threads)->Data_Glob_Args->cond);


	for(i=0; i< (*Data_Threads)->n_data_threads; i++){
		
		if(i > 0) {
			Pthread_mutex_lock(&(*Data_Threads)->Data_Glob_Args->lock);
/* 
 * this is identification iof the thread
 */	
			(*Data_Threads)->Data_Glob_Args->VARIABLE    =  (*Data_Threads)->data_threads[i];
/*
  * at the beginning the coutner of remainign threads is equal to 
  * number of available threads
  * this coutner is used to synchronize all threads at the end when they went on each other
  * it is reset every iterational step
  */
			*(*Data_Threads)->data_threads_remainth_counter = *(*Data_Threads)->data_threads_availth_counter;
/*
 * once all necessary data are set, send signal to all threads to start
 */
			Pthread_cond_broadcast(&(*Data_Threads)->Data_Glob_Args->cond);
		}
			
		Pthread_mutex_unlock(&(*Data_Threads)->Data_Glob_Args->lock);
/* 
 * when all Data_Thread are finished, - the identification part, the threads are waiting on each other. 
 * the last thread unlock the semaphore so that the next loop can start
 */
		Sem_wait(&(*Data_Threads)->Data_Glob_Args->sem);
	}

	return 0;
}