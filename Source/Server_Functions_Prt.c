
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




void Sem_init(sem_t *sem, lmuint_t val){
	if(sem_init(sem, 0, val) < 0)
		Perror("sem_init()");
}

void Sem_wait(sem_t *sem){
	if( sem_wait(sem) < 0)
		Perror("sem_init()");
}

void Sem_post(sem_t *sem){
	if( sem_post(sem) < 0)
		Perror("sem_post()");
}

void Sem_destroy(sem_t *sem){
	if( sem_destroy(sem) < 0)
		Perror("sem_destroy()");
}

lmint_t Sem_getvalue(sem_t *sem){
	lmint_t val;
	if( sem_getvalue(sem, &val) < 0)
		Perror("sem_getvalue()");
	return val;
}





node_t *sender_identification(lmchar_t *Sender_Name, lmchar_t *Sender_data_set, lmchar_t RWmode)
{
	node_t *Gnode, *TmpNode;
	size_t *dim;
	
	if(  (Gnode = m3l_Mklist("Sender_Ident", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
		Perror("m3l_Mklist");
	
	dim = (size_t *) malloc( 1* sizeof(size_t));
/*
 * add sender name
 */
	dim[0] = strlen(Sender_Name)+1;
	if(  (TmpNode = m3l_Mklist("Sender_Name", "C", 1, dim, &Gnode, "/Sender_Ident", "./", "--no_malloc", (char *)NULL)) == 0)
		Error("m3l_Mklist");
	TmpNode->data.c = Sender_Name;
/*
 * add data set name
 */
	dim[0] = strlen(Sender_data_set)+1;
	if(  (TmpNode = m3l_Mklist("Data_Set", "C", 1, dim, &Gnode, "/Sender_Ident", "./", "--no_malloc", (char *)NULL)) == 0)
		Error("m3l_Mklist");
	TmpNode->data.c = Sender_data_set;	
	free(dim);
/*
 * add if reader or sender
 */
	dim[0] = 2;
	if(  (TmpNode = m3l_Mklist("Sender_Mode", "C", 1, dim, &Gnode, "/Sender_Ident", "./", "--no_malloc", (char *)NULL)) == 0)
		Error("m3l_Mklist");
	*TmpNode->data.c = RWmode;	
	free(dim);	
 	return Gnode;
	
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