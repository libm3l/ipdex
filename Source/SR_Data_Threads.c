
#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"
#include "SR_Data_Threads.h"

static inline lmssize_t Read(lmint_t, lmchar_t *, lmint_t);
static inline lmssize_t Write(lmint_t, lmchar_t *, lmsize_t);

static lmint_t R_KAN(SR_thread_args_t *, lmint_t, lmint_t, lmint_t);
static lmint_t S_KAN(SR_thread_args_t *, lmint_t, lmint_t);

void *SR_Data_Threads(void *arg)
{
	SR_thread_args_t *c = (SR_thread_args_t *)arg;

	lmchar_t SR_mode;
	lmint_t sockfd, mode;
/* 
 * get SR_mode and socket number, unlock so that other SR_threads can get ther
  * increase counter so that next job can grab it.
*/
	while(1){
/*
 * wait until all requests (all Receiver + Sender) for a particular data_set arrived 
 * the last invoking of Pthread_barrier_wait is done in Thread_Prt.c
 */
		*c->pthr_cntr = 0;

		Pthread_barrier_wait(c->pbarr);
/*
 * get SR_mode and socket number of each connected processes
 * protext by mutex
 */
		Pthread_mutex_lock(c->plock);
			
			SR_mode =  c->pSR_mode[*c->pthr_cntr];
			sockfd  =  c->psockfd[*c->pthr_cntr];
			(*c->pthr_cntr)++; 
	
		Pthread_mutex_unlock(c->plock);
		
// 		switch(*c->pKA_mode){
// 			
// 		case 'N':
/*
 * decide which mode is used; depends on KeepAlive and ATDT option
 * the value of mode set in SR_hub.c
 */
		switch(*c->pSRt_mode){
			
		case 1:
/*
 * do not keep socket allive, ie. open and close secket every time the data transfer occurs
 */
// 			if(*c->pATDT_mode == 'D'){ /* *c->pSRt_mode == 1 */

				mode = 1;

				if(SR_mode == 'R'){
/*
 * R(eceivers)
 */
					if( R_KAN(c, sockfd, mode, 1) != 1) return NULL;
				}
				else if(SR_mode == 'S'){
/*
 * S(ender)
 */
					if( S_KAN(c, sockfd, mode) != 1) return NULL;
				}
				else{
					Error("SR_Data_Threads: Wrong SR_mode");
				}
// 			}
		break;
		
		case 2:
// 			else if(*c->pATDT_mode == 'A'){/* *c->pSRt_mode == 2 */

				mode = 0;
/*
 * ATDT mode == A, the Receiver will receive the data and then send 
 * back to Sender, Sender will first send the data and then receive from Receiver
 * works only for 1 R process
 */
				
				if(SR_mode == 'R'){
/*
 * R(eceivers)
 * when finishing with R, do not signal SR_hub to go to another loop, 
 * the Receiver process will now send the data 
 */
					if( R_KAN(c, sockfd, mode, 0) != 1) return NULL;
					mode = 1; /* send REOB */
					if( S_KAN(c, sockfd, mode) != 1)    return NULL;
				}
				else if(SR_mode == 'S'){
/*
 * S(ender), after finishing sending, receive the data
 * after that signal SR_hhub that SR operation is finished and it can do 
 * another loop
 */
					if( S_KAN(c, sockfd, mode) != 1)    return NULL;
					mode = 1; /*require REOB */
					if( R_KAN(c, sockfd, mode, 1) != 1) return NULL;
				}
				else{
					Error("SR_Data_Threads: Wrong SR_mode");
				}
				
// 			}
// 			else
// 				Error(" SR_Data_Thread: Wrong ATDT mode");
		break;
		
		case 3:
/*
 * keep socket allive, clients decide when to close it
 */
			Error("SR_Data_Threads: KA_mode == C not implemented yet");
			exit(0);

// 			if(*c->pATDT_mode == 'D'){/* *c->pSRt_mode == 3 */
				mode = 3;

				if(SR_mode == 'R'){
/*
 * R(eceivers)
 */
				}
				else if(SR_mode == 'S'){
/*
 * S(ender)
 */
				}
				else{
					Error("SR_Data_Threads: Wrong SR_mode");
				}
				
// 			}
		break;
		
		case 4:
// 			else if(*c->pATDT_mode == 'A'){/* *c->pSRt_mode == 4 */
				mode = 4;
				
				if(SR_mode == 'R'){
/*
 * R(eceivers)
 */
				}
				else if(SR_mode == 'S'){
/*
 * S(ender)
 */
				}
				else{
					Error("SR_Data_Threads: Wrong SR_mode");
				}
// 			}
// 			else
// 				Error(" SR_Data_Thread: Wrong ATDT mode");

		break;
		
		default:
			Error("SR_Data_Threads: Wrong KA_mode");
		break;
		}
	}
	free(c);
	return NULL;
}


/*
 * socket read and write function prototypes
 */
lmssize_t Write(lmint_t sockfd,  lmchar_t *buffer, lmsize_t size){
/*
 * write buffer to socket
 */
	lmssize_t total, n;	
	total = 0;
	lmchar_t *buff;

	buff = buffer;
	
	while(size > 0) {
		
		if ( (n = write(sockfd,buff,size)) < 0){
			if (errno == EINTR) continue;
			return (total == 0) ? -1 : total;
		}
 		buff  += n;
		total += n;
		size  -= n;
	}

	return total;
}


lmssize_t Read(lmint_t descrpt , lmchar_t *buff, lmint_t n)
{
	lmsize_t ngotten;

	if (  (ngotten = read(descrpt,buff,n)) == -1){
		Perror("read here");
		return -1;
	}
	buff[ngotten] = '\0';

	return ngotten;
}


/*
 * Recevier function, ATDT A,D  KeepAllive N
 */
lmint_t R_KAN(SR_thread_args_t *c, lmint_t sockfd, lmint_t mode, lmint_t send_sem){

	lmint_t  R_done, last;
	opts_t *Popts, opts;
	lmssize_t n;
	Popts = &opts;
	
	opts.opt_linkscleanemptlinks = '\0';  // clean empty links
	opts.opt_nomalloc = '\0'; // if 'm', do not malloc (used in Mklist --no_malloc
	opts.opt_linkscleanemptrefs = '\0'; // clean empty link references
	opts.opt_tcpencoding = 't'; // serialization and encoding when sending over TCP/IP
	opts.opt_MEMCP = 'S';  // type of buffering
/*
 * Receiver threads, set R_done = 0, once the 
 * transfer of entire message is done (ie. Sender sends EOMB sequence
 * set R_done = 1
 */
	R_done = 0;
/*
 * thread reads the data from buffer and send over TCP/IP to client
 */
	do{
/*
 * last thread will set last = 1 
 */
		last = 0;
/*
 * gate syncing all threads, syncing for Sender is done after reading from socket
 * after sycning, check that the Sender received EOFbuff, if yes, set R_done = 1
 */
		pt_sync(c->psync_loc);
				
		if(*c->pEofBuff != 0){
			R_done = 1;}
		else{
			R_done = 0;}
/*
 * the mutex was locked here to protect writing to each individual sockets
 * but I think it is  not needed, moved lock after 
 */
		if ( (n = Write(sockfd,c->pbuffer, *c->pngotten)) < *c->pngotten)
			Perror("write()");

		Pthread_mutex_lock(c->plock);
			(*c->prcounter)--;
			*c->psync = 0;
					
		if(*c->prcounter == 0){
/*
 * if this is the last Receiver thread, set last to 1
 */
			last = 1;
/* 	
 * the last thread, broadcast
 * set number of remaining threads equal to number of reading threads (only if reading will be repeated, otherwise keep it 0)
 * indicate this is the last thread
 */
			*c->psync == 1;
			Pthread_cond_broadcast(c->pdcond);
/*
 * clean buffer
 */
			bzero(c->pbuffer, MAXLINE+1);
			*c->pngotten = 0;
/*
 * signal Sender that all Receivers are ready for next 
 * round of transmition
 */
			Sem_post(c->psem);
/* 
 * unlock semaphore in the main program so that another loop can start
 */
		}
		else{
/*
 * still some threads working, wait for them
 * indicate this is waiting thread
 */
			while (*c->psync != 0)
				Pthread_cond_wait(c->pdcond, c->plock);
		}

		Pthread_mutex_unlock(c->plock);

	}while(R_done == 1);
/*
 * EOFbuff received, transmition is finished
 * 
 * Reading process sends signal that the it received all data (ie. 
 * 	----- m3l_Send_to_tcpipsocket(NULL,(char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB", (char *)NULL);
 * it is just to make sure all processes are done with transfer
 * do in only ff ATDT mode is D
 */
// 	m3l_Receive_tcpipsocket((const lmchar_t *)NULL, sockfd, "--encoding" , "IEEE-754", "--REOB",  (lmchar_t *)NULL);

// 	if(*c->pATDT_mode == 'D'){


	switch(mode){
		case 1:
			opts.opt_REOBseq = 'G'; // send EOFbuff sequence only
			if( m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts) < 0){
				Error("SR_Data_Threads: Error when receiving  REOB\n");
				return -1;
			}
/*
 * close the socket 
 */
			if( close(sockfd) == -1)
				Perror("close");
// 		}
		break;
		
		case 3:
			
		break;
	}
// /*
//  * close socket, and if last partition, unlock semaphore so that Thead_Prt can continue (closing socket moved to SR_hub)
//  */
// 	if( close(sockfd) == -1)
// 		Perror("close");
// 
/*
 * synck before letting SR_hub to close sockets
 */
	pt_sync(c->psync_loc);
/*
 * if specified, signal the SR_hub and it can do another cycle
 */
	if(send_sem == 1){
		if(last == 1)
			Sem_post(c->psem_g);
	}
	
	return 1;
}


/*
 * Sender function, ATDT A,D  KeepAllive N
 */
lmint_t S_KAN(SR_thread_args_t *c, lmint_t sockfd, lmint_t mode){

	lmchar_t prevbuff[EOBlen+1];
	lmint_t eofbuffcond;
	opts_t *Popts, opts;
	Popts = &opts;

	opts.opt_linkscleanemptlinks = '\0';  // clean empty links
	opts.opt_nomalloc = '\0'; // if 'm', do not malloc (used in Mklist --no_malloc
	opts.opt_linkscleanemptrefs = '\0'; // clean empty link references
	opts.opt_tcpencoding = 't'; // serialization and encoding when sending over TCP/IP
	opts.opt_MEMCP = 'S';  // type of buffering

	bzero(prevbuff, EOBlen+1);
/*
 * thread reads data from TCP/IP socket sent by client and 
 * write them to buffer
 */	
	eofbuffcond = 0;
	do{
/*
 * set counter of Receiving threads to number of R_threads (used in synchronizaiton of R_Threads)
 */
		*c->prcounter = *c->pcounter-1;
		*c->pEofBuff = 1;

		bzero(c->pbuffer,MAXLINE+1);
		if (  (*c->pngotten = Read(sockfd, c->pbuffer, MAXLINE)) == -1){
			Perror("read");
			free(c);
			return;
		}

		eofbuffcond = Check_EOFbuff(c->pbuffer,prevbuff, strlen(c->pbuffer), EOBlen, EOFbuff);
/*
 * The buffer has been red from socket, send broadcast signal to all R_threads to go on
 * then unlock mutex and wait for semaphore
 */
		if(eofbuffcond == 1)
			*c->pEofBuff = 0;
/*
 * wait on synchronization point, the syncing for Receivers is done before writing the 
 * bffer to socket
 */
		pt_sync(c->psync_loc);
/*
 * wait until all Receivers sent the data to the socket
 */
		Sem_wait(c->psem);
/*
 * if end of buffer reached, leave do cycle
 */
	}while(eofbuffcond != 1);
/*
 * sender sent payload, before closign socket send back acknowledgement --SEOB, Sender receives --REOB
 * do it only if ATDT mode == D
 */
// 			if( m3l_Send_to_tcpipsocket((node_t *)NULL, (const lmchar_t *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB",  (lmchar_t *)NULL) < 1)
// 				Error("Error during reading data from socket");
// 	if(*c->pATDT_mode == 'D'){

	switch(mode){
		case 1:
			opts.opt_EOBseq = 'E'; // send EOFbuff sequence only	
			if( m3l_send_to_tcpipsocket((node_t *)NULL, (const lmchar_t *)NULL, sockfd, Popts) < 0){
				Error("SR_Data_Threads: Error when sending  SEOB\n");
				return -1;
			}
/*
 * close the socket 
 */
			if( close(sockfd) == -1)
				Perror("close");
// 		}
		break;
		
		case 3:
			
			
		break;
	}
// 
// 	if( close(sockfd) == -1)  (closing socket moved to SR_hub)
// 		Perror("close");
/*
 * synck before letting SR_hub to close sockets
 */
	pt_sync(c->psync_loc);
	
	return 1;
}
