#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"

void Pthread_barrier_init(pthread_barrier_t *Barr, lmint_t Nval){
	if( pthread_barrier_init(Barr,  NULL, Nval) != 0)
		Perror(" pthread_barrier_init()");
}

void Pthread_barrier_destroy(pthread_barrier_t *Barr){
	if( pthread_barrier_destroy(Barr) != 0)
		Perror(" pthread_barrier_destroy()");
}


void Pthread_barrier_wait(pthread_barrier_t *Barr){
	
	lmint_t rcbarr;
	
	rcbarr = pthread_barrier_wait(Barr);
	
	if(rcbarr != 0 && rcbarr != PTHREAD_BARRIER_SERIAL_THREAD){
		Perror("pthread_barrier_wait()\n");
	}
}




void Pthread_cond_destroy(pthread_cond_t *cond){
	if( pthread_cond_destroy(cond) != 0)
		Perror("pthread_cond_destroy()");
}

void Pthread_cond_init(pthread_cond_t *cond){
	if( pthread_cond_init(cond, NULL) != 0)
		Perror("pthread_cond_init()");
}

void Pthread_cond_signal(pthread_cond_t *Barr){
	if( pthread_cond_signal(Barr) != 0)
		Perror("pthread_cond_send()");
}

void Pthread_cond_broadcast(pthread_cond_t *Barr){
	if( pthread_cond_broadcast(Barr) != 0)
		Perror("pthread_cond_broadcast()");
}

void Pthread_cond_wait(pthread_cond_t *Barr, pthread_mutex_t *lock){
	if( pthread_cond_wait(Barr, lock) != 0)
		Perror("pthread_cond_wait()");
}




void Pthread_mutex_init(pthread_mutex_t *lock){
	if (pthread_mutex_init(lock, NULL) != 0)
		Perror("pthread_mutex_init()");
}

void Pthread_mutex_destroy(pthread_mutex_t *lock){
	if (pthread_mutex_destroy(lock) != 0)
		Perror("pthread_mutex_destroy()");
}

void Pthread_mutex_lock(pthread_mutex_t *lock){
	if( pthread_mutex_lock(lock) != 0)
		Perror("pthread_mutex_lock()");
}

void Pthread_mutex_unlock(pthread_mutex_t *lock){
	if( pthread_mutex_unlock(lock) != 0)
		Perror("pthread_mutex_unlock()");
}


// void Sync(lmint_t SYNC_MAX_COUNT)
// {
//         /* use static variables to ensure initialization */
//         static mutex_t sync_lock = PTHREAD_MUTEX_INITIALIZER;
//         static cond_t  sync_cond = PTHREAD_COND_INITIALIZER;
//         static int sync_count = 0;
// 
//         /* lock the access to the count */
//         pthread_mutex_lock(&sync_lock);
// 
//         /* increment the counter */
//         sync_count++;
// 
//         /* check if we should wait or not */
//         if (sync_count < SYNC_MAX_COUNT)
// 
//              /* wait for the others */
//              pthread_cond_wait(&sync_cond, &sync_lock);
// 
//         else
// 
//             /* broadcast that everybody reached the point */
//             pthread_cond_broadcast(&sync_cond);
// 
//         /* unlocks the mutex - otherwise only one thread
//                 will be able to return from the routine! */
//         pthread_mutex_unlock(&sync_lock);
// }