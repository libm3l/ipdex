#include "libm3l.h"
#include "Server_Header.h"
#include "Data_Thread.h"
#include "Server_Functions_Prt.h"

#include "Server_Body.h"


lmint_t Server_Body(node_t *Gnode){
	
	lmsize_t i;
	int sockfd, newsockfd, portno;
	struct sockaddr_in cli_addr;
	data_thread_str_t *Data_Threads;
	
	socklen_t clilen;

/*
 * spawn all threads
 */
	Data_Threads = Data_Thread(Gnode);
 /*
 * create, bind and listen socket
 */
	if ( (sockfd = m3l_server_openbindlistensocket(portno, (char *)NULL) ) < 0 )
		Perror("Open_Bind_Listen");
	
	while(1){

		clilen = sizeof(cli_addr);

		if ( (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0){
			if(errno == EINTR) /* If Interrupted system call, restart - back to while ()  UNP V1 p124  */
				continue;
			else
				Perror("accept()");
		}
/*
 * loop over and send variable
 */
		Pthread_mutex_lock(&Data_Threads->Data_Glob_Args->lock);
/*
 * save socket descriptor
 */
		Data_Threads->Data_Glob_Args->socket =  newsockfd;
/*
 * set the counter to number of available threads
 */
		*Data_Threads->data_threads_status_counter  =  Data_Threads->n_data_threads;
 /*
  * at the beginning the coutner of remainign threads is equal to 
  * number of available threads
  * this coutner is used to synchronize all threads at the end when they went on each other
  * it is reset every iterational step
  */
		*Data_Threads->data_threads_remain_counter  = *Data_Threads->data_threads_status_counter;
/* 
 * this is identification iof the thread
 */
		Data_Threads->Data_Glob_Args->VARIABLE     =  Data_Threads->data_threads[0];
/*
 * wait for barrier, indicating all threads in Data_Thread were created
 * the _wait on this barrier is the second_wait call in Data_Thread for each thread and this is the last one
 * makes sure we do not start Data_Thread before some of the data which are needed are filled abd mutex is locked - see 
 * fours lines above
 */
		Pthread_barrier_wait(&Data_Threads->Data_Glob_Args->barr);
/*
 * give condition signal to Data_Thread that they can start - see part of code in Data_Thread
 *              -   while (*c->prcounter == 0)
 *		- 	Pthread_cond_wait(c->pcond, c->plock);
 */
		Pthread_cond_broadcast(&Data_Threads->Data_Glob_Args->cond);


		for(i=0; i< Data_Threads->n_data_threads; i++){
			
			if(i > 0) {
				Pthread_mutex_lock(&Data_Threads->Data_Glob_Args->lock);
/* 
 * this is identification iof the thread
 */				Data_Threads->Data_Glob_Args->VARIABLE    =  Data_Threads->data_threads[i];
/*
  * at the beginning the coutner of remainign threads is equal to 
  * number of available threads
  * this coutner is used to synchronize all threads at the end when they went on each other
  * it is reset every iterational step
  */
				*Data_Threads->data_threads_remain_counter = *Data_Threads->data_threads_status_counter;
/*
 * once all necessary data are set, send signal to all threads to start
 */
				Pthread_cond_broadcast(&Data_Threads->Data_Glob_Args->cond);
			}
			
			Pthread_mutex_unlock(&Data_Threads->Data_Glob_Args->lock);
/* 
 * when all Data_Thread are finished, - the identification part, the threads are waiting on each other. 
 * the last thread unlock the semaphore so that the next loop can start
 */
			Sem_wait(&Data_Threads->Data_Glob_Args->sem);
		}
/*
 * join threads and release memmory
 */
		for(i=0; i< Data_Threads->n_data_threads; i++)
			pthread_join(Data_Threads->data_threads[i], NULL);
		
		
	}  /* end of while(1) */
		
		Pthread_mutex_destroy(&Data_Threads->Data_Glob_Args->lock);
		Pthread_barrier_destroy(&Data_Threads->Data_Glob_Args->barr);
		Pthread_cond_destroy(&Data_Threads->Data_Glob_Args->cond);
		Pthread_cond_destroy(&Data_Threads->Data_Glob_Args->dcond);
		Sem_destroy(&Data_Threads->Data_Glob_Args->sem);
		
		free(Data_Threads->data_threads);
		free(Data_Threads->data_threads_status_counter);
		free(Data_Threads->data_threads_remain_counter);
		free(Data_Threads->Data_Glob_Args);
		free(Data_Threads);

	
	return 1;
}