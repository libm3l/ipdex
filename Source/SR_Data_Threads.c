
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

		printf(" HERE \n");
		
			Pthread_mutex_lock(c->plock_g);
// 			while (*c->prcounter == 0)
				Pthread_cond_wait(c->pcond_g, c->plock_g);		
			Pthread_mutex_unlock(c->plock_g);
			
			Pthread_mutex_lock(c->plock);
		printf(" HERE1 \n");
			
			SR_mode =  c->pSR_mode[*c->pthr_cntr];
			sockfd  =  c->psockfd[*c->pthr_cntr++];
		
		printf("job socket %d, mode %c\n", sockfd,  SR_mode);

// 		Pthread_mutex_unlock(c->plock);


		if(SR_mode == 'R'){
/*
 * thread reads the data from buffer and send over TCP/IP to client
 */
			while(1){
// 				Pthread_mutex_lock(c->plock);
/*
 * wait for data sent by main thread
 */
				while (*c->prcounter == 0)
					Pthread_cond_wait(c->pcond, c->plock);
				
				*c->prcounter--;
				
				if ( (n = Write(sockfd,c->pbuffer, *c->pngotten)) < *c->pngotten)
					Perror("write()");

				
				if(*c->prcounter == 0){
/* 	
 * the last thread, broadcast
 * indicate this is the last thread
 */
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
// 					while (*c->prcounter != 0)
					Pthread_cond_wait(c->pdcond, c->plock);
				}
			
				Pthread_mutex_unlock(c->plock);

				if(*c->pEofBuff == 0){
/*
 *  End of buffer reached, stop tranfering the data 
 */					break;
					if(*c->prcounter == 0)Sem_post(c->psem_g);
				}
			}

		}
		else if(SR_mode == 'S'){
/*
 * thread reads data from TCP/IP socket sent by client and 
 * write them to buffer
 */	
			eofbuffcond = 0;
			do{
// 				Pthread_mutex_lock(c->plock);
/*
 * set counter of Receiving threads to number of R_threads (used in synchronizaiton of R_Threads)
 */ 		
				*c->prcounter = *c->pcounter;
				*c->pEofBuff = 1;

				bzero(c->pbuffer,MAXLINE+1);
				if (  (*c->pngotten = Read(sockfd, c->pbuffer, MAXLINE)) == -1){
					Perror("read");
					free(c);
					return;
				}
				eofbuffcond = Check_EOFbuff(c->pbuffer,prevbuff, strlen(c->pbuffer), EOBlen, EOFbuff);				
// 				eofbuffcond = Check_EOFbuff(c->pbuffer,prevbuff, *c->pngotten, EOBlen, EOFbuff);
				
				if(eofbuffcond == 1)*c->pEofBuff = 0;
/*
 * The buffer has been red from socket, send broadcast signal to all R_threads to go on
 * then unlock mutex and wait for semaphore
 */			
				Pthread_cond_broadcast(c->pcond);
				Pthread_mutex_unlock(c->plock);
				Sem_wait(c->psem);
/*
 * if end of buffer reached, leave do cycle
 */
			}while(eofbuffcond == 0);
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
	
	while(size > 0) {
		
		if ( (n = write(sockfd,buff,size)) < 0){
			if (errno == EINTR) continue;
			return (total == 0) ? -1 : total;
		}
 		buff += n;
		total += n;
		size -= n;
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

	return ngotten;
}