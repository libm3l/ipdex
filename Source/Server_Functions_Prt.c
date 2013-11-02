/*
 *     Copyright (C) 2012  Adam Jirasek
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *     
 *     contact: libm3l@gmail.com
 * 
 */




/*
 *     Function Server_Functions_Prt.c
 *
 *     Date: 2013-07-04
 * 
 * 
 *     Description:
 * 
 *
 *     Input parameters:
 * 
 *
 *     Return value:
 * 
 * 
 *
 *     Modifications:
 *     Date		Version		Patch number		CLA 
 *
 *
 *     Description
 * 
 */




#include "libm3l.h"
#include "lsipdx_header.h"
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





node_t *sender_identification(lmchar_t *Sender_data_set, lmchar_t RWmode)
{
	node_t *Gnode, *TmpNode;
	size_t *dim;
	
	if(  (Gnode = m3l_Mklist("Sender_Ident", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
		Perror("m3l_Mklist");
	
	dim = (size_t *) malloc( 1* sizeof(size_t));
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

/*
 * this is a synchronizer programmed according to " POSIX Threads Tutorial  by Mark Hays, www. http://math.arizona.edu/~swig/documentation/pthreads/
 */

void pt_sync(pt_sync_t *sync)
{
/*   
 *	*sync->pnthreads contains the values of number of threads which will be synchronized
 *	this value must be specified before this function is invoked
 */
	if (*sync->pnthreads<2) {
		return;};           /* trivial case            */
/*
 * lock the block and mutex
 */
	Pthread_mutex_lock(sync->pblock);

	Pthread_mutex_lock(sync->pmutex);
/*
 * find if the job is last or not  NOTE: *sync->pnsync has to be intialized to 0 before 
 * syncing starts
 */
	if (++(*sync->pnsync) < *sync->pnthreads) { 
/*
 * no, unlock block and 
 */
	Pthread_mutex_unlock(sync->pblock);
/*
 * wait for condvar
 */
	Pthread_cond_wait(sync->pcondvar, sync->pmutex);

	} 
/*
 * last process
 */	else 
  	{	
/*
 * wake up all waiting processes
 */
	Pthread_cond_broadcast(sync->pcondvar);
/* 
 * got to sleep till they are all awake, then release block
 */
	Pthread_cond_wait(sync->plast,sync->pmutex);
	Pthread_mutex_unlock(sync->pblock);
	}
/*
 * if next to last one out, wake up the last one
 */
	if (--(*sync->pnsync) == 1){
// 		Pthread_cond_broadcast(sync->plast);
 		Pthread_cond_signal(sync->plast);
	}
/*
 * release mutex
 */
	Pthread_mutex_unlock(sync->pmutex);
}



lmint_t get_exchange_channel_mode(lmchar_t ATDTMode, lmchar_t KeepAllive_Mode){
/*
 * function returns mode of the socket based on the values of ATDTMode and KeepAllive_Mode
 */
	lmint_t retval;

	     if(ATDTMode == 'D' && KeepAllive_Mode == 'N')  /* Direct transfer, close socket */
		retval = 1;
	else if(ATDTMode == 'A' && KeepAllive_Mode == 'N')  /* Alternate transfer, close socket */
		retval = 2;
	else if(ATDTMode == 'D' && KeepAllive_Mode == 'C')  /* Direct transfer, close socket after client request it*/
		retval = 3;
	else if(ATDTMode == 'A' && KeepAllive_Mode == 'C')  /* Alternate transfer, close socket after client request it*/
		retval = 4;
	else if(ATDTMode == 'D' && KeepAllive_Mode == 'Y')  /* Direct transfer, do not close socket*/
		retval = 5;
	else if(ATDTMode == 'A' && KeepAllive_Mode == 'Y')  /* Alternate transfer, do not close socket*/
		retval = 6;
	else
		retval = -1;
	
	return retval;

}