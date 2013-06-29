#include "libm3l.h"
#include "Server_Header.h"
#include "Data_Thread.h"
#include "Server_Functions_Prt.h"
#include "Server_Body.h"
#include "arpa/inet.h"
#include "Allocate_DataBuffer.h"
#include "Check_Request.h"
#include "ACK.h"


lmint_t Server_Body(node_t *Gnode, lmint_t portno){
	
	lmsize_t i, len;
	lmint_t sockfd, newsockfd, cycle;
	struct sockaddr_in cli_addr;
	data_thread_str_t *Data_Threads;
	lmchar_t *name_of_required_data_set, *SR_mode;
	
	socklen_t clilen;
	find_t *SFounds, *Tqst_SFounds;
	node_t *RecNode, *List, *DataBuffer, *TmpNode, *RR_POS, *RR_NEG;
	lmsize_t dim[1];	

	opts_t *Popts, opts;
	Popts = &opts;

	char str[100];
	
	opts.opt_linkscleanemptlinks = '\0';  // clean empty links
	opts.opt_nomalloc = '\0'; // if 'm', do not malloc (used in Mklist --no_malloc
	opts.opt_linkscleanemptrefs = '\0'; // clean empty link references
	opts.opt_tcpencoding = 'I'; // serialization and encoding when sending over TCP/IP
	opts.opt_MEMCP = 'S';  // type of buffering
	
	cycle=0;
/*
 * create positive and negative return receipt
 */
	RR_POS = ret_receipt(1);
	RR_NEG = ret_receipt(0);
/*
 * create buffer structure for buffering recevied data requests if needed
 */
	if( (DataBuffer = Allocate_DataBuffer(Gnode)) == NULL)
		Error("Buffering problem");
/*
 * spawn all threads
 */
	if(  (Data_Threads = Data_Thread(DataBuffer)) == NULL)
		Perror("Server_Body: Data_Threads error");
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
 * set return value to 0
 */
	*Data_Threads->retval = 0;
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

/*
 * if already in cycle, you need to lock mutex here
 */
		if(cycle > 0){
			Pthread_mutex_lock(&Data_Threads->lock);
		}
		
		if(*Data_Threads->data_threads_availth_counter == 0){
					
			while(*Data_Threads->data_threads_availth_counter == 0)
				Pthread_cond_wait(&Data_Threads->cond, &Data_Threads->lock);
		}

		Pthread_mutex_unlock(&Data_Threads->lock);
	
		clilen = sizeof(cli_addr);

// 		printf(" \n\nWaiting for new socket \n");
		
		if ( (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0){
			if(errno == EINTR) /* If Interrupted system call, restart - back to while ()  UNP V1 p124  */
				continue;
			else
				Perror("accept()");
		}

// 		printf(" ... .arrived \n");

// 		inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);
//    		printf("	CONNECTION --------------------   : %s:%d\n",str, ntohs(cli_addr.sin_port)); 
/*
 * receive header with solver and data set information
 */
// 		if( (RecNode = m3l_Receive_tcpipsocket((const char *)NULL, newsockfd, "--encoding" , "IEEE-754", (char *)NULL)) == NULL)
// 			Error("Error during reading data from socket");

			opts.opt_REOBseq = '\0'; // send EOFbuff sequence only
			opts.opt_EOBseq = '\0'; // send EOFbuff sequence only

		if( (RecNode = m3l_receive_tcpipsocket((const char *)NULL, newsockfd, Popts)) == NULL)
			Error("Error during reading data from socket");


		dim[0] = 1;

		if(  (TmpNode = m3l_Mklist("socket_nr", "I", 1, dim, &RecNode, "/Header", "./", (char *)NULL)) == 0)
			Error("m3l_Mklist");
		TmpNode->data.i[0] = newsockfd;

// 		printf(" Received DATA set from Socket:\n");
// 		if(m3l_Cat(RecNode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
// 			Error("CatData");
/* 
 * find Name_of_Data_Set
 * make sure the ./ path is used instead of /
 * if the RecNode comes from socket it starts with /Header, if it comes from buffer
 * it is a subset
 */
		if( (SFounds = m3l_Locate(RecNode, "./Header/Name_of_Data_Set", "./*/*",  (lmchar_t *)NULL)) != NULL){
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
		if( (SFounds = m3l_Locate(RecNode, "./Header/SR_mode", "./*/*",  (lmchar_t *)NULL)) != NULL){
			
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
 * loop over - identify thread correspoding to required data thread.
 * this thread spanws n SR threads (1 Sending thread and n-1 Reading threads) which take care of data transfer,
 * so once the data_thread is identified n-times, the thread is taken away from 
 * pool of available data threads (ie. decrement  (*Data_Threads->data_threads_availth_counter)--)
 * Once the data transfer is finished, add the data thread to the pool of available data threads
 * (ie. increment  (*Data_Threads->data_threads_availth_counter)++)
 */

 		Pthread_mutex_lock(&Data_Threads->lock);

		switch ( Check_Request(DataBuffer, name_of_required_data_set, SR_mode, name_of_required_data_set)) {
			case 0:


// 				printf(" Case 0 --- %s   %c\n", name_of_required_data_set, *SR_mode);

/* 
 * Legal request, not in buffer, data_thread available 
 */			
				if(*SR_mode == 'S'){
/*
 * if process is sender, indicate Sender that header was received before receiving payload
 * if process is Receiver send acknowledgment and get back REOB
 */
// 					if( m3l_Send_to_tcpipsocket(RR_POS, (const char *)NULL, newsockfd, "--encoding" , "IEEE-754",  (char *)NULL) < 1)
// 						Error("Error during sending data from socket");
					
					opts.opt_REOBseq = '\0'; // send EOFbuff sequence only
					opts.opt_EOBseq = '\0'; // send EOFbuff sequence only
					if( m3l_send_to_tcpipsocket(RR_POS, (const char *)NULL, newsockfd, Popts) < 1)
						Error("Error during sending data from socket");
				}
				else if(*SR_mode == 'R'){
// 					m3l_Send_receive_tcpipsocket(RR_POS, (const char *)NULL, newsockfd, "--encoding" , "IEEE-754", "--REOB", (char *)NULL);

					opts.opt_REOBseq = 'G'; // send EOFbuff sequence only
					opts.opt_EOBseq = '\0'; // send EOFbuff sequence only	
					if( m3l_send_receive_tcpipsocket(RR_POS, (const lmchar_t *)NULL, newsockfd, Popts) < 0){
						Error(" PROBLEM HERE \n");
						return -1;
					}
				}
				else
					Error("Wrong SR mode\n");
/*
 * at least one data thread is available:
 *  -  set number of remainign data threads equalt to available data threads
 *  (this values is used for syncig, ie. one the data thread is checked the coutner is decremented
 * -  set number of syncing threads to number of available threads + 1 (this is used to sync all processes - both this process and 
 * data threads are synced so that they all start at one point, Server_Body waits until all data threads arraive at syncing point before signaling 
 * them to analyze the data
 * - set data_set name, SR_Mode and socket number so that data_thread processes can start identification
 * - set the return value to 0, once the thread is identified, the value is set to 1
 */

// 				printf(" Case 0 setting values --- %s   %c\n", name_of_required_data_set, *SR_mode);



				*Data_Threads->data_threads_remainth_counter 	= *Data_Threads->data_threads_availth_counter;	
				*Data_Threads->sync->nthreads				= *Data_Threads->data_threads_availth_counter + 1;
				*Data_Threads->retval = 0;
				
				if( snprintf(Data_Threads->name_of_data_set, MAX_NAME_LENGTH,"%s",name_of_required_data_set) < 0)
					Perror("snprintf");
				*Data_Threads->SR_mode = *SR_mode;
				*Data_Threads->socket  = newsockfd;
/*
 * before entering the synchronization, make sure the data_thread which was busy 
 * and is about to finish, will not increase the number of available 
 * data_threads once some of the data_threads or Server_body enters synchronizer.
 * If at least one of these processes is entering the synchronizer, the 
 * Data_Thread which is about to increase a number of available Data_Threads and will become available
 * wait until this synchronziation is over.
 * 
 * NOTE: The problem was that some of the processes entered the synchronizer and the 
 * number of syncrhonized jobs were then increased. Dead-lock
 */
*Data_Threads->t_sync_protect = 1;

				Pthread_mutex_unlock(&Data_Threads->lock);
/*
 * once all necessary data are set, send signal to all threads to start unloc mutex
 * and release borrowed memory
 */
				printf(" Case 0 syncing --- %s   %c   socket %d\n", name_of_required_data_set, *SR_mode, newsockfd);
				pt_sync(Data_Threads->sync, 1, "Ser_B");

Pthread_mutex_lock(&Data_Threads->lock);
	if(*Data_Threads->t_sync_protect == 1){
		Pthread_cond_signal(&Data_Threads->t_sync_cond_protect);
		*Data_Threads->t_sync_protect = 0;
	}	
Pthread_mutex_unlock(&Data_Threads->lock);
/* 
 * when all Data_Thread are finished, - the identification part, the threads are waiting on each other. 
 * the last thread unlock the semaphore so that the next loop can start
 */		
// 				printf(" Case 0 sem wait --- %s   %c\n", name_of_required_data_set, *SR_mode);
				Sem_wait(&Data_Threads->sem);
		
				if(*Data_Threads->retval == 1){
/*
 * data set was identified
 */
// 				printf(" Case 0 retval (%d)  --- %s   %c\n", *Data_Threads->retval, name_of_required_data_set, *SR_mode);


					if( m3l_Umount(&RecNode) != 1)
						Perror("m3l_Umount");
				}
				else{
					printf(" Case 0 retval (%d)  --- %s   %c\n", *Data_Threads->retval, name_of_required_data_set, *SR_mode);
					Warning("Server_Body: Not valid data set");
					if( m3l_Umount(&RecNode) != 1)
						Perror("m3l_Umount");
					continue;
				}
			
			break;
			
			case 1:
				printf(" Case 1 --- %s   %c\n", name_of_required_data_set, *SR_mode);
			
				if(*SR_mode == 'S'){
/*
 * if process is sender, indicate Sender that header was received before receiving payload
 * if process is Receiver send acknowledgment and get back REOB
 */
// 				if( m3l_Send_to_tcpipsocket(RR_NEG, (const char *)NULL, newsockfd, "--encoding" , "IEEE-754",  (char *)NULL) < 1)
// 					Error("Error during sending data from socket");
					opts.opt_REOBseq = '\0'; // send EOFbuff sequence only
					opts.opt_EOBseq = '\0'; // send EOFbuff sequence only
					if( m3l_send_to_tcpipsocket(RR_NEG, (const char *)NULL, newsockfd, Popts) < 1)
						Error("Error during sending data from socket");
					
				}
				else if(*SR_mode == 'R'){
// 					m3l_Send_to_tcpipsocket(RR_NEG, (const char *)NULL, newsockfd, "--encoding" , "IEEE-754", (char *)NULL);
					opts.opt_REOBseq = '\0'; // send EOFbuff sequence only
					opts.opt_EOBseq = '\0'; // send EOFbuff sequence only
					if( m3l_send_to_tcpipsocket(RR_NEG, (const char *)NULL, newsockfd, Popts) < 1)
						Error("Error during sending data from socket");
				}
				else{
					Error("Wrong SR mode\n");
				}
				
				
					if( close(newsockfd) == -1)
						Perror("close");
/*
 * data_thread is occupied let the process know it and close socket
 * process will attempt to establish connection later
 */
				Pthread_mutex_unlock(&Data_Threads->lock);
			
			break;
		}
			
			printf(" after handshake \n");
		
/*
 * initial stage was completed, server is running in while(1) loop, set cycle to 1
 * needed for propper locking of mutex
 */
		cycle = 1;
/*
 * if the loop is done for the first time after receiving request from socket
 * the RecNode (received data set request) was checked. 
 * Loop over the buffer and check sets which are temporarily stored there
 */
	}      /* end of while(1) */
	
	if(Tqst_SFounds != NULL) m3l_DestroyFound(&Tqst_SFounds);
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
	free(Data_Threads->retval);
	free(Data_Threads->t_sync_protect);
	
	free(Data_Threads->sync->nsync);
	free(Data_Threads->sync->nthreads);
	Pthread_mutex_destroy(&Data_Threads->sync->mutex);
	Pthread_mutex_destroy(&Data_Threads->sync->block);
	Pthread_cond_destroy(&Data_Threads->sync->condvar);
	Pthread_cond_destroy(&Data_Threads->sync->last);
	Pthread_cond_destroy(&Data_Threads->t_sync_cond_protect);
	
	free(Data_Threads->sync);
	free(Data_Threads->sync_loc);

	free(Data_Threads);
	
	if(m3l_Umount(&DataBuffer) != 1)
		Perror("m3l_Umount DataBuffer");


	return 1;
}