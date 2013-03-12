#include "libm3l.h"
#include "Server_Header.h"
#include "Data_Thread.h"
#include "Server_Functions_Prt.h"
#include "Identify_Data_Thread.h"
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
// 	if ( (sockfd = m3l_server_openbindlistensocket(portno, (char *)NULL) ) < 0 )
// 		Perror("Open_Bind_Listen");
	
// 	while(1){

// 		clilen = sizeof(cli_addr);
// 
// 		if ( (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0){
// 			if(errno == EINTR) /* If Interrupted system call, restart - back to while ()  UNP V1 p124  */
// 				continue;
// 			else
// 				Perror("accept()");
// 		}
/*
 * loop over and send variable
 */
		if( Identify_Data_Thread(&Data_Threads) != 0)
			Perror("Identify_Data_Thread problem");
/*
 * join threads and release memmory
 */
		for(i=0; i< Data_Threads->n_data_threads; i++)
			pthread_join(Data_Threads->data_threads[i], NULL);
		
		
// 	}  /* end of while(1) */
		
		Pthread_mutex_destroy(&Data_Threads->Data_Glob_Args->lock);
		Pthread_barrier_destroy(&Data_Threads->Data_Glob_Args->barr);
		Pthread_cond_destroy(&Data_Threads->Data_Glob_Args->cond);
		Pthread_cond_destroy(&Data_Threads->Data_Glob_Args->dcond);
		Sem_destroy(&Data_Threads->Data_Glob_Args->sem);
		
		free(Data_Threads->data_threads);
		free(Data_Threads->data_threads_availth_counter);
		free(Data_Threads->data_threads_remainth_counter);
		free(Data_Threads->Data_Glob_Args);
		free(Data_Threads);

	
	return 1;
}