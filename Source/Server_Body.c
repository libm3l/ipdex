#include "libm3l.h"
#include "Server_Header.h"
#include "Data_Thread.h"
#include "Server_Functions_Prt.h"
// #include "Identify_Data_Thread.h"
#include "Server_Body.h"


lmint_t Server_Body(node_t *Gnode, lmint_t portno){
	
	lmsize_t i, len;
	lmint_t sockfd, newsockfd, cycle;
	struct sockaddr_in cli_addr;
	data_thread_str_t *Data_Threads;
	lmchar_t *name_of_required_data_set, *SR_mode;
	
	socklen_t clilen;
	find_t *SFounds;
	node_t *RecNode, *List;
	
	cycle=0;
/*
 * spawn all threads
 */
	Data_Threads = Data_Thread(Gnode);
/*
 * fill the initial data to data_thread_str before threads start
 */	
	Pthread_mutex_lock(&Data_Threads->Data_Glob_Args->lock);
/*
 * set the counter to number of available threads
 */
	*Data_Threads->data_threads_availth_counter  =  Data_Threads->n_data_threads;
	
// printf(" TOTAL NUMBER OF THREADS IS %ld\n", *Data_Threads->data_threads_availth_counter);
/*
 * at the beginning the coutner of remainign threads is equal to 
 * number of available threads
 * this coutner is used to synchronize all threads at the end when they went on each other
 * it is reset every iterational step
 */
	*Data_Threads->data_threads_remainth_counter  = *Data_Threads->data_threads_availth_counter;
/*
 * wait for barrier, indicating all threads in Data_Thread were created
 * the _wait on this barrier is the second_wait call in Data_Thread for each thread and this is the last one
 * makes sure we do not start Data_Thread before some of the data which are needed are filled abd mutex is locked - see 
 * fours lines above
 */
	Pthread_barrier_wait(&Data_Threads->Data_Glob_Args->barr);
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
 * receive header with solver and data set information
 */
		if( (RecNode = m3l_Receive_tcpipsocket((const char *)NULL, newsockfd, "--encoding" , "IEEE-754", (char *)NULL)) == NULL)
			Error("Error during reading data from socket");
		
		if(m3l_Cat(RecNode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
			Error("CatData");
		
 		if( m3l_Send_to_tcpipsocket(NULL, (const char *)NULL, newsockfd, "--encoding" , "IEEE-754", "--SEOB",  (char *)NULL) < 1)
 					Error("Error during reading data from socket");
/* 
 * find Name_of_Data_Set
 */
		if( (SFounds = m3l_Locate(RecNode, "/Header/Name_of_Data_Set", "/*/*",  (lmchar_t *)NULL)) != NULL){
			if( m3l_get_Found_number(SFounds) != 1)
				Error("Server_Body: Only one Name_of_Data_Set per Data_Set allowed");
			if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
				Error("Server_Body: NULL Name_of_Data_Set");
			name_of_required_data_set = m3l_get_data_pointer(List);
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			Error("Server_Body: Name_of_Data_Set not found\n");
		}
/*
 * find type of process SR_Mode  S-sender, R-receiver
 */
		if( (SFounds = m3l_Locate(RecNode, "/Header/SR_mode", "./*/*",  (lmchar_t *)NULL)) != NULL){
			
			if( m3l_get_Found_number(SFounds) != 1)
				Error("Server_Body: Only one SR_mode per Data_Set allowed");
/* 
 * pointer to list of found nodes
 */
				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
					Error("NULThread_Prt: Missing S_mode");
			
				SR_mode = (lmchar_t *)m3l_get_data_pointer(List);
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			printf("Server_Body: Receiving_Processes not found\n");
		}
		
/*
 * loop over and send variable
 */
		if(cycle > 0)
			Pthread_mutex_lock(&Data_Threads->Data_Glob_Args->lock);
/*
 * set number of tested threads to number of available threads
 * save name of data set from header, SM_Mode of the process and socket number to thread data structure
 */		
		*Data_Threads->data_threads_remainth_counter = *Data_Threads->data_threads_availth_counter;	
		if( snprintf(Data_Threads->name_of_data_set, MAX_NAME_LENGTH,"%s",name_of_required_data_set) < 0)
			Perror("snprintf");
		*Data_Threads->SR_mode = *SR_mode;
		*Data_Threads->socket = newsockfd;
			
// 		printf(" Before Broadcasting SOCKET number is %d\n", *Data_Threads->socket);
/*
 * once all necessary data are set, send signal to all threads to start unloc mutex
 * and release borrowed memory
 */			
		Pthread_cond_broadcast(&Data_Threads->Data_Glob_Args->cond);
			
		Pthread_mutex_unlock(&Data_Threads->Data_Glob_Args->lock);
		
		if( m3l_Umount(&RecNode) != 1)
			Perror("m3l_Umount");
/* 
 * when all Data_Thread are finished, - the identification part, the threads are waiting on each other. 
 * the last thread unlock the semaphore so that the next loop can start
 */		
		Sem_wait(&Data_Threads->Data_Glob_Args->sem);
		
		cycle = 1;

	}      /* end of while(1) */
/*
 * join threads and release memmory
 */
		for(i=0; i< Data_Threads->n_data_threads; i++)
			if( pthread_join(Data_Threads->data_threads[i], NULL) != 0)
				Error(" Joining thread failed");
				
		Pthread_mutex_destroy(&Data_Threads->Data_Glob_Args->lock);
		Pthread_barrier_destroy(&Data_Threads->Data_Glob_Args->barr);
		Pthread_cond_destroy(&Data_Threads->Data_Glob_Args->cond);
		Pthread_cond_destroy(&Data_Threads->Data_Glob_Args->dcond);
		Sem_destroy(&Data_Threads->Data_Glob_Args->sem);
		
		free(Data_Threads->data_threads);
		free(Data_Threads->name_of_data_set);
		free(Data_Threads->SR_mode);
		free(Data_Threads->data_threads_availth_counter);
		free(Data_Threads->data_threads_remainth_counter);
		free(Data_Threads->Data_Glob_Args);
		free(Data_Threads->socket);
		free(Data_Threads);

	
	return 1;
}