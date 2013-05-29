
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
	lmint_t sockfd, eofbuffcond;
	lmsize_t n;
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

// 		printf(" READER \n");
/*
 * thread reads the data from buffer and send over TCP/IP to client
 */
			do{
/*
 * gate syncing all threads
 */
				pt_sync(c->psync_loc);
				
// 				printf(" RECEIVER after syncing '%s'\n", c->pbuffer);
				
				if ( (n = Write(sockfd,c->pbuffer, *c->pngotten)) < *c->pngotten)
					Perror("write()");

// 				printf(" RECEIVER SENT DATA  %d\n ", n);

				Pthread_mutex_lock(c->plock);
				(*c->prcounter)--;
				*c->psync == 0;
				
				if(*c->prcounter == 0){
/* 	
 * the last thread, broadcast
 * set number of remaining threads equal to number of reading threads (only if reading will be repeated, otherwise keep it 0)
 * indicate this is the last thread
 */
					*c->psync == 1;
					Pthread_cond_broadcast(c->pdcond);
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

// 				printf(" REDER after syncing %d\n", *c->pEofBuff );
			
				Pthread_mutex_unlock(c->plock);

			}while(*c->pEofBuff != 0);
			
// 			printf("READER finished, reading SEOB \n");
/*
 * EOFbuff received, transmition is finished
 * 
 * Reading process sends signal that the it received all data (ie. 
 * 	----- m3l_Send_to_tcpipsocket(NULL,(char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB", (char *)NULL);
 * it is just to make sure all processes are done with transfer
 */
			m3l_Receive_tcpipsocket((const char *)NULL, sockfd, "--encoding" , "IEEE-754", "--REOB",  (char *)NULL);
/*
 * close socket, and if last partition, unlock semaphore so that Thead_Prt can continue
 */
// 			printf("READER closing socket after reading SEOB \n");
			if( close(sockfd) == -1)
				Perror("close");
			if(*c->prcounter == 0)
				Sem_post(c->psem_g);
// 			printf("READER after Semaphore \n");

		}
		else if(SR_mode == 'S'){

// 		printf(" Sender  send SEOB \n");
/*
 * sender sent its Header, before sending other data, send back acknowledgement
 */
// 			printf(" SR_Data_Threads1 : Send_to_tcp\n");
			if( m3l_Send_to_tcpipsocket((node_t *)NULL, (const char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB",  (char *)NULL) < 1)
				Error("Error during reading data from socket");
// 			printf(" SR_Data_Threads1 : Send_to_tcp -- DONE\n");
/*
 * thread reads data from TCP/IP socket sent by client and 
 * write them to buffer
 */	
			eofbuffcond = 0;
			do{
				Pthread_mutex_lock(c->plock);
/*
 * set counter of Receiving threads to number of R_threads (used in synchronizaiton of R_Threads)
 */ 		
				*c->prcounter = *c->pcounter-1;
// 				printf(" Sender  %d  \n", *c->prcounter);
				*c->pEofBuff = 1;

// 		printf(" Sender  READING \n");
				bzero(c->pbuffer,MAXLINE+1);
				if (  (*c->pngotten = Read(sockfd, c->pbuffer, MAXLINE)) == -1){
					Perror("read");
					free(c);
					return;
				}

// 				printf(" SENDER  buffer   %d  '%s' \n", *c->pngotten, c->pbuffer);

				eofbuffcond = Check_EOFbuff(c->pbuffer,prevbuff, strlen(c->pbuffer), EOBlen, EOFbuff);
// 				eofbuffcond = Check_EOFbuff(c->pbuffer,prevbuff, *c->pngotten, EOBlen, EOFbuff);
// 				printf(" Sender  after EOFBUFF check %d \n", eofbuffcond);
				
				if(eofbuffcond == 1)*c->pEofBuff = 0;
/*
 * The buffer has been red from socket, send broadcast signal to all R_threads to go on
 * then unlock mutex and wait for semaphore
 */			
				Pthread_mutex_unlock(c->plock);
				pt_sync(c->psync_loc);
				Sem_wait(c->psem);
/*
 * if end of buffer reached, leave do cycle
 */
			}while(eofbuffcond != 1);

// 		printf(" SENDER leaving while\n");

/*
 * sender sent payload, before closign socket send back acknowledgement
 */
// 			printf(" SR_Data_Threads2 : Send_to_tcp\n");
			if( m3l_Send_to_tcpipsocket((node_t *)NULL, (const char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB",  (char *)NULL) < 1)
				Error("Error during reading data from socket");
// 			printf(" SR_Data_Threads2 : Send_to_tcp -- DONE\n");

			if( close(sockfd) == -1)
				Perror("close");
		}
		else{
			Error("Wrong option");
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

// 	printf(" WRITE %d  '%s'\n", sockfd, buff);
	
	while(size > 0) {
		
		if ( (n = write(sockfd,buff,size)) < 0){
			if (errno == EINTR) continue;
			return (total == 0) ? -1 : total;
		}
 		buff  += n;
		total += n;
		size  -= n;
	}
/*
 * buffer was sent, nullify it
 */
	bzero(buffer, sizeof(buffer));
	return total;
}

// 		bzero(buff,sizeof(buff));
// 		if (  (ngotten = Read(descrpt, MAXLINE-1)) == -1){
// 			Perror("read");
// 			return -1;
// 		}

lmssize_t Read(lmint_t descrpt , lmchar_t *buff, lmint_t n)
{
	lmsize_t ngotten;

	if (  (ngotten = read(descrpt,buff,n)) == -1){
		Perror("read");
		return -1;
	}
	buff[ngotten] = '\0';
// 				printf(" GOT   %d  '%s'   %d \n", ngotten, buff, descrpt);

	return ngotten;
}