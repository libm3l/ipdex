#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"

void Pthread_barrier_init(pthread_barrier_t *Barr, lmint_t Nval){
	if( pthread_barrier_init(Barr,  NULL, Nval) != 0)
		Perror("Data_Thread: pthread_barrier_init()");
}

void Pthread_barrier_destroy(pthread_barrier_t *Barr){
	if( pthread_barrier_destroy(Barr) != 0)
		Perror("Data_Thread: pthread_barrier_destroy()");
}

void Pthread_barrier_wait(pthread_barrier_t *Barr){
	
	lmint_t rcbarr;
	
	rcbarr = pthread_barrier_wait(Barr);
	
	if(rcbarr != 0 && rcbarr != PTHREAD_BARRIER_SERIAL_THREAD){
		Perror("Data_Threads: pthread_barrier_wait()\n");
	}
}




void Pthread_cond_destroy(pthread_cond_t *cond){
	if( pthread_cond_destroy(cond) != 0)
		Perror("Data_Thread: pthread_cond_destroy()");
}

void Pthread_cond_init(pthread_cond_t *cond){
	if( pthread_cond_init(cond, NULL) != 0)
		Perror("Data_Thread: pthread_cond_init()");
}






void Pthread_mutex_init(pthread_mutex_t *lock){
	if (pthread_mutex_init(lock, NULL) != 0)
		Perror("Data_Thread: pthread_mutex_init()");
}

void Pthread_mutex_destroy(pthread_mutex_t *lock){
	if (pthread_mutex_destroy(lock) != 0)
		Perror("Data_Thread: pthread_mutex_destroy()");
}

void Pthread_mutex_lock(pthread_mutex_t *lock){
	if( pthread_mutex_lock(lock) != 0)
		Perror("Data_Thread: pthread_mutex_lock()");
}

void Pthread_mutex_unlock(pthread_mutex_t *lock){
	if( pthread_mutex_unlock(lock) != 0)
		Perror("Data_Thread: pthread_mutex_unlock()");
}