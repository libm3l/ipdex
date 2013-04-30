#include "libm3l.h"
#include "Server_Header.h"
#include "Data_Thread.h"
#include "Server_Functions_Prt.h"
#include "Server_Body.h"
#include "arpa/inet.h"

lmint_t Server_Body(node_t *Gnode, lmint_t portno){
	
	lmsize_t i, len;
	lmint_t sockfd, newsockfd, cycle;
	struct sockaddr_in cli_addr;
	data_thread_str_t *Data_Threads;
	lmchar_t *name_of_required_data_set, *SR_mode;
	
	socklen_t clilen;
	find_t *SFounds;
	node_t *RecNode, *List, *DataBuffer;

	char str[100];
	
	cycle=0;
/*
 * spawn all threads
 */
	if(  (Data_Threads = Data_Thread(Gnode)) == NULL)
		Perror("Server_Body: Data_Threads error");
/*
 * allocate buffer for received data sets
 */
// 	if( (DataBuffer = Allocate_DataBuffer(Gnode)
/*
 * fill the initial data to data_thread_str before threads start
 */	
	Pthread_mutex_lock(&Data_Threads->lock);
/*
 * set the counter to number of available threads
 */
	*Data_Threads->data_threads_availth_counter  =  Data_Threads->n_data_threads;
/*
 * at the beginning the coutner of remainign threads is equal to 
 * number of available threads
 * this coutner is used to synchronize all threads at the end when they went on each other
 * it is reset every iterational step
 */
	*Data_Threads->data_threads_remainth_counter  = *Data_Threads->data_threads_availth_counter;
/*
 * this counter is used in gate syncing data threads and server_body 
 * at the beginning it should be set to number of threads + 1 (n_threads + 1 Server_Body)
 */
	*Data_Threads->sync->nthreads  		      = *Data_Threads->data_threads_availth_counter + 1;
/*
 * wait for barrier, indicating all threads in Data_Thread were created
 * the _wait on this barrier is the second_wait call in Data_Thread for each thread and this is the last one
 * makes sure we do not start Data_Thread before some of the data which are needed are filled abd mutex is locked - see 
 * fours lines above
 */
	Pthread_barrier_wait(&Data_Threads->barr);
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

// 		inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);
//    		printf("	CONNECTION --------------------   : %s:%d\n",str, ntohs(cli_addr.sin_port)); 


// 	exit(0);
/*
 * receive header with solver and data set information
 */
		if( (RecNode = m3l_Receive_tcpipsocket((const char *)NULL, newsockfd, "--encoding" , "IEEE-754", (char *)NULL)) == NULL)
			Error("Error during reading data from socket");
		
		if(m3l_Cat(RecNode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
			Error("CatData");
		
//  		if( m3l_Send_to_tcpipsocket(NULL, (const char *)NULL, newsockfd, "--encoding" , "IEEE-754", "--SEOB",  (char *)NULL) < 1)
//  					Error("Error during reading data from socket");
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
				
				if(*SR_mode == 'S'){
/*
 * if process is sender, indicate Sender header was received before receiving payload
 * - not needed if process is Receiver
 */
					if( m3l_Send_to_tcpipsocket(NULL, (const char *)NULL, newsockfd, "--encoding" , "IEEE-754", "--SEOB",  (char *)NULL) < 1)
						Error("Error during reading data from socket");
				}
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
 * loop over - identify thread correspoding to required data thread.
 * this thread spanws n SR threads (1 Sending thread and n-1 Reading threads) which take care of data transfer,
 * so once the data_thread is identified n-times, the thread is taken away from 
 * pool of available data threads (ie. decrement  (*Data_Threads->data_threads_availth_counter)--)
 * Once the data transfer is finished, add the data thread to the pool of available data threads
 * (ie. increment  (*Data_Threads->data_threads_availth_counter)++)
 */
// printf(" Before IF cycle\n");

/*
 * if already in cycle, you need to lock mutex here
 */
		if(cycle > 0)
			Pthread_mutex_lock(&Data_Threads->lock);
// printf(" Here 1\n");
/*
 * set number of tested threads to number of available threads
 * save name of data set from header, SM_Mode of the process and socket number to thread data structure
 */		
/*
 * if no data threads are available, wait until at least one of them is available
 * this can happen when all threads are occupied with transferrign the data
 */
		if(*Data_Threads->data_threads_availth_counter == 0){
			while(*Data_Threads->data_threads_availth_counter == 0)
				Pthread_cond_wait(&Data_Threads->cond, &Data_Threads->lock);
		}
/*
 * at least one data thread is availble:
 *  -  set number of remainign data threads equalt to available data threads
 *  (this values is used for syncig, ie. one the data thread is checked the coutner is decremented
 * -  set number of syncing threads to number of available threads + 1 (this is used to sync all processes - both this process and 
 * data threads are synced so that they all start at one point, Server_Body waits until all data threads arraive at syncing point before signaling 
 * them to analyze the data
 * - set data_set name, SR_Mode and socket number so that data_thread processes can start identification
 */

		*Data_Threads->data_threads_remainth_counter = *Data_Threads->data_threads_availth_counter;	
// printf(" Here 2 -- %d  %d\n", *Data_Threads->data_threads_remainth_counter, *Data_Threads->data_threads_availth_counter);

		*Data_Threads->sync->nthreads  		      = *Data_Threads->data_threads_availth_counter + 1;

		if( snprintf(Data_Threads->name_of_data_set, MAX_NAME_LENGTH,"%s",name_of_required_data_set) < 0)
			Perror("snprintf");
		*Data_Threads->SR_mode = *SR_mode;
		*Data_Threads->socket = newsockfd;
			
//  		printf(" Before Broadcasting SOCKET number is %d   %c\n", *Data_Threads->socket, *Data_Threads->SR_mode);


		Pthread_mutex_unlock(&Data_Threads->lock);
/*
 * once all necessary data are set, send signal to all threads to start unloc mutex
 * and release borrowed memory
 */
// 		printf(" Waiting for gate - MAIN \n");
		pt_sync(Data_Threads->sync);
// 		printf(" After gate - MAIN \n");

// 		Pthread_cond_broadcast(&Data_Threads->cond);
		
// printf(" Here 3\n");
			
// 		Pthread_mutex_unlock(&Data_Threads->lock);
// printf(" Here 4\n");
		
		if( m3l_Umount(&RecNode) != 1)
			Perror("m3l_Umount");
// printf(" Here 5\n");

/* 
 * when all Data_Thread are finished, - the identification part, the threads are waiting on each other. 
 * the last thread unlock the semaphore so that the next loop can start
 */		
		Sem_wait(&Data_Threads->sem);
// printf(" Here 6\n");

		cycle = 1;

	}      /* end of while(1) */
/*
 * join threads and release memmory
 */
		for(i=0; i< Data_Threads->n_data_threads; i++)
			if( pthread_join(Data_Threads->data_threads[i], NULL) != 0)
				Error(" Joining thread failed");
				
		Pthread_mutex_destroy(&Data_Threads->lock);
		Pthread_barrier_destroy(&Data_Threads->barr);
		Pthread_cond_destroy(&Data_Threads->cond);
		Pthread_cond_destroy(&Data_Threads->dcond);
		Sem_destroy(&Data_Threads->sem);
		
		free(Data_Threads->data_threads);
		free(Data_Threads->name_of_data_set);
		free(Data_Threads->SR_mode);
		free(Data_Threads->data_threads_availth_counter);
		free(Data_Threads->data_threads_remainth_counter);
		free(Data_Threads->socket);
		
		free(Data_Threads->sync->nsync);
		free(Data_Threads->sync->nthreads);
		Pthread_mutex_destroy(&Data_Threads->sync->mutex);
		Pthread_mutex_destroy(&Data_Threads->sync->block);
		Pthread_cond_destroy(&Data_Threads->sync->condvar);
		Pthread_cond_destroy(&Data_Threads->sync->last);
		
		free(Data_Threads->sync);
		free(Data_Threads->sync_loc);

	
		free(Data_Threads);

	
	return 1;
}