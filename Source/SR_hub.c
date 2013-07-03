
#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"

#include "SR_hub.h"

void *SR_hub(void *arg)
{
/*
 * function signals SR_Threads that all requests (one Sender and n_rec_proc Receivers) arrived and 
 * that it can start data transfer
 */

/*
 * NOTE in previuous version, this function was run after the while(n_avail_loc_theads != 0);  // all connecting thread arrivied, ie. one Sender and n_rec_proc Receivers 
 * in Thread_Prt ended. It did not work when more then one data set arrived and pt_sync in Thread_Prt in do loop was not behaving properly
 * due to the fact that the number os synced threads was changing. To avoid that problem the process of signaling SR_threads are run in parallel 
 * with Thread_Prt
 */
	SR_hub_thread_str_t *c = (SR_hub_thread_str_t *)arg;
	
	while(1){
/*
 * wait for semaphore from Thread_Prt that 
 * all requests arrived
 */
// 		printf(" -------------------------------   SR_Hub waiting for semaphor  %d\n", *c->pcounter);
		Sem_wait(c->psem);
/*
 * confirm succesfull Sem_wait()
 */
// 		Sem_post(c->sem)

/*
 * go to last barrier, all others are already taken by SR_Threads, Once
 * the thread arrives on this barried, the data tranfer between Sender and all Receivers start
 */
// 		printf(" -------------------------------   SR_Hub got green light  %d\n", *c->pcounter);
/*
 * once all R-W threads are taken decrement counter of data_threads ie. Data_Thread->data_threads_availth_counter
 */		
// 		for (ii = 0; ii< n_rec_proc + 1; ii++)
// 			printf("job %d, socket %d, mode %s  %c\n", ii, SR_Threads->sockfd[ii],  local_set_name, *c->pcounter, SR_Threads->SR_mode[ii]);
/*
 * wait until all SR_threads reach barrier, then start actual transfer of the data from S to R(s)
 */
// 		printf("Thread_Prt: after unlock 1\n");
		Pthread_barrier_wait(c->pbarr);
// 		printf("Thread_Prt: Waiting on semaphore \n");
/*
 * once the data transfer is finished wait until all data is tranferred and S and R threads close their socket
*/
		Sem_wait(c->psem_g);
// 		printf("TEST_... TRANFER FINISHED\n\n\n");

		*c->pn_avail_loc_theads = *c->pn_rec_proc + 1;

// 		printf("Thread_Prt: %s lock 2\n", local_set_name);

		Pthread_mutex_lock(c->plock);
/*
 * release thread, ie. set Thread_Status = 0
 */
		*c->pThread_Status = 0;
// 		(*c->pcounter)++;
/*
 * if all threads were occupied, ie *Data_Threads->data_threads_availth_counter == *c->pcounter == 0
 * the server is waiting for signal before the continuing with data process identification. 
 * This is done in Server_Body before syncing with data threads
 * If this happens, signal Server_Body that at least one data_thread is avaiable 
 */
		if(*c->pcounter == 1)
			Pthread_cond_signal(c->pcond);
		
// 			printf("Thread_Prt: %s unlock 2\n", local_set_name);
		Pthread_mutex_unlock(c->plock);
// 			printf("Thread_Prt: after unlock 2\n");

		printf("SR_HUB    -    GOING TO NEXT LOOP   %d \n\n\n",  *c->pcounter);
	}
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);

	return NULL;

}