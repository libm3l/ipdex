
#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"
#include "SR_Data_Threads.h"

void *SR_Data_Threads(void *arg)
{
	SR_thread_args_t *c = (SR_thread_args_t *)arg;

	lmchar_t SR_mode;
	lmint_t sockfd;
/*
 * wait for signal broadcast
 */

/* 
 * get SR_mode and socket number, unlock so that other SR_threads can get ther
  * increase counter so that next job can grab it.
*/
	Pthread_mutex_lock(c->plock);
		
		SR_mode =  c->pSR_mode[*c->pthr_cntr];
		sockfd      =  c->psockfd[*c->pthr_cntr++];

	Pthread_mutex_unlock(c->plock);

	if(SR_mode == 'R'){
/*
 * thread reads the data from buffer and send over TCP/IP to client
 */
	}
	else if(SR_mode == 'S'){
/*
 * thread reads data from TCP/IP socket sent by client and 
 * write them to buffer
 */

/*
 * set counter of Receiving threads to number of R_threads (used in synchronizaiton of R_Threads)
 */
 	*c->prcounter = *c->pcounter


	
// 	if(SR_mode == 'R')/* Reading process, reads buffer and send to socket */
// 	else if(SR_mode == 'S')  /* sender - reads socket and writes to buffer */
// 	else  /* wrong option */	
// 
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