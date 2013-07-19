
#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"
#include "SR_Data_Threads.h"

static inline lmssize_t Read(lmint_t, lmchar_t *, lmint_t);
static inline lmssize_t Write(lmint_t, lmchar_t *, lmsize_t);

void *SR_Data_Threads(void *arg)
{
	SR_thread_args_t *c = (SR_thread_args_t *)arg;

	lmchar_t SR_mode, prevbuff[EOBlen+1];
	lmint_t sockfd, eofbuffcond, R_done, last;
	lmsize_t n;


	pthread_t  MyThreadID;

	opts_t *Popts, opts;
	
	opts.opt_linkscleanemptlinks = '\0';  // clean empty links
	opts.opt_nomalloc = '\0'; // if 'm', do not malloc (used in Mklist --no_malloc
	opts.opt_linkscleanemptrefs = '\0'; // clean empty link references
	opts.opt_tcpencoding = 't'; // serialization and encoding when sending over TCP/IP
	opts.opt_MEMCP = 'S';  // type of buffering
	
	Popts = &opts;

	MyThreadID = pthread_self();
/*
 * wait for signal broadcast
 */

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

		if(SR_mode == 'R'){
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
 * do in only of ATDT mode is D
 */
// 			m3l_Receive_tcpipsocket((const lmchar_t *)NULL, sockfd, "--encoding" , "IEEE-754", "--REOB",  (lmchar_t *)NULL);

			if(*c->pATDT_mode == 'D'){
				opts.opt_REOBseq = 'G'; // send EOFbuff sequence only
				if( m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts) < 0){
					Error("SR_Data_Threads: Error when receiving  REOB\n");
					return NULL;
				}
			}
// /*
//  * close socket, and if last partition, unlock semaphore so that Thead_Prt can continue
//  */
// 			if( close(sockfd) == -1)
// 				Perror("close");
// 
/*
 * synck before letting SR_hub to close sockets
 */
// 			printf(" ATDT_R mode is %c\n", *c->pATDT_mode);
			pt_sync(c->psync_loc);

			if(last == 1)
				Sem_post(c->psem_g);
		}
		else if(SR_mode == 'S'){
/*
 * Sender thread
 */
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

			if(*c->pATDT_mode == 'D'){
				opts.opt_EOBseq = 'E'; // send EOFbuff sequence only	
				if( m3l_send_to_tcpipsocket((node_t *)NULL, (const lmchar_t *)NULL, sockfd, Popts) < 0){
					Error("SR_Data_Threads: Error when sending  SEOB\n");
					return NULL;
				}
			}
			
// 			printf(" ATDT mode is %c\n", *c->pATDT_mode);
// 
// 			if( close(sockfd) == -1)
// 				Perror("close");
// 
/*
 * synck before letting SR_hub to close sockets
 */
			pt_sync(c->psync_loc);
		}
		else{
			Error("SR_Data_Threads: Wrong SR_mode");
		}
	}
	
	free(c);
	return NULL;
}

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