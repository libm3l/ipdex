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
// 	int retval;
// AGAIN:
// 	if (  (retval = pthread_mutex_destroy(lock)) != 0){
// 		if(retval == 16) goto AGAIN;
// 		Perror("pthread_mutex_destroy()");
// 	}
 	if ( pthread_mutex_destroy(lock) != 0){
		Perror("pthread_mutex_destroy()");
	}
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
	if(  (TmpNode = m3l_Mklist("Channel", "C", 1, dim, &Gnode, "/Sender_Ident", "./", "--no_malloc", (char *)NULL)) == 0)
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

/*
 * this is a synchronizer programmed according to " POSIX Threads Tutorial  by Mark Hays, www. http://math.arizona.edu/~swig/documentation/pthreads/
 */

lmint_t pt_sync(pt_sync_t *sync)
{
/*
 * function implemented according to : "POSIX Threads Tutorial by Mark Hays: http://math.arizona.edu/~swig/documentation/pthreads/"
 * function pt_gate_init:
 * 
 * 
 * First, let's look at the data structure:
*
* 231:  typedef struct _pt_gate_t_ {
* 232:    int ngate;              
* 233:    int nthreads;           
* 234:    pthread_mutex_t mutex;  
* 235:    pthread_mutex_t block;  
* 236:    pthread_cond_t condvar; 
* 237:    pthread_cond_t last;    
* 238:  } pt_gate_t;
* 239:  
* v240:  extern void pt_gate_init(pt_gate_t *gate,int nthreads);
* 241:  extern void pt_gate_destroy(pt_gate_t *gate);
* 242:  extern void pt_gate_sync(pt_gate_t *gate);
* 
* The main data type, pt_gate_t, contains several pthreads objects. There are functions to initialize and destroy this compound object, and a * * function to perform the behavior specified above. The roles of the various structure members will become clear in a moment.
* 
* Now let's look at the C source:
* 
* 
* 68: void pt_gate_init(pt_gate_t *gate,int nthreads)
* 69: {
* 70:   gate->ngate=0; gate->nthreads=nthreads;
* 71:   pt_mutex_init(  &gate->mutex, "gate: init mutex");
* 72:   pt_mutex_init(  &gate->block, "gate: init block");
* 73:   pt_cond_init (&gate->condvar, "gate: init condvar");
* 74:   pt_cond_init (   &gate->last, "gate: init last");
* 75: }
* 76: 
* 77: *************************************************
* 78:  * destroy a gate variable
* 79:  *
* 80: void pt_gate_destroy(pt_gate_t *gate)
* 81: {
* 82:   gate->ngate=gate->nthreads=0;
* 83:   pt_mutex_destroy(  &gate->mutex, "gate: destroy mutex");
* 84:   pt_mutex_destroy(  &gate->block, "gate: destroy block");
* 85:   pt_cond_destroy (&gate->condvar, "gate: destroy condvar");
* 86:   pt_cond_destroy (   &gate->last, "gate: destroy last");
* 87: }
* 88: 
* 89: *************************************************
* 90:  * enter the gate
* 91:  *
* 92: void pt_gate_sync(pt_gate_t *gate)
* 93: {
* 94:   if (gate->nthreads<2) return;           * trivial case            *
* 95:   pt_mutex_lock(&gate->block,             * lock the block -- new   *
* 96:            "gate: lock block");           *   threads sleep here   *
* 97:   pt_mutex_lock(&gate->mutex,             * lock the mutex          *
* 98:            "gate: lock mutex");
* 99:   if (++(gate->ngate) < gate->nthreads) { * are we the last one in? *
* 100:     pt_mutex_unlock(&gate->block,         * no, unlock block and    *
* 101:                "gate: unlock block 1");
* 102:     pt_cond_wait(&gate->condvar,          *   go to sleep           *
* 103:             &gate->mutex,
* 104:             "gate: wait condvar");
* 105:   } else {                                * yes, we're last         *
* 106:     pt_cond_broadcast(&gate->condvar,     * wake everyone up and    *
* 107:                  "gate: bcast condvar");
* 108:     pt_cond_wait(&gate->last,&gate->mutex,  * go to sleep til they're *
* 109:             "gate: wait last");             * all awake... then       *
* 110:     pt_mutex_unlock(&gate->block,           * release the block       *
* 111:                "gate: unlock block 2");
* 112:   }
* 113:   if (--(gate->ngate)==1) {               * next to last one out?   *
* 114:     pt_cond_broadcast(&gate->last,        * yes, wake up last one   *
* 115:                  "gate: bcast last");
* 116:   }
* 117:   pt_mutex_unlock(&gate->mutex,           * release the mutex       *
* 118:              "gate: unlock mutex");
* 119: }
*
*
* The gate_init() function simply initializes the members of the gate_t structure. It takes two arguments: a pointer to the gate_t being * * * * initialized and N, the number of threads the gate is supposed to synchronize.
* 
* Gate_destroy() frees the resources associated with a gate_t structure.
* 
* The behavior we descibed earlier occurs in pt_gate_sync() and is implemented in lines 92-119. Ignore all statements involving gate->block 
* for a moment. The mutex gate->mutex simply protects access to gate->ngate. The calling thread locks this mutex in line 97. The member 
* gate->nthreads does not need protection: it's value is only updated by the gate_init() call. In line 99, the ngate member is incremented 
* and compared to nthreads. If the calling thread is not the last one, pt_cond_wait is called. This puts the thread to sleep until the last
* thread calls pt_gate_sync().
* 
* The last thread executes the else clause on line 105. First it calls pthread_cond_broadcast() (once this thread releases gate->mutex, the 
* sleeping threads will begin waking up). Next, it goes to sleep on the gate->last condition variable (whose role will become clearer below),
* which has the side effect of unlocking gate->mutex. At this point, the other threads wake up one at a time (after locking gate->mutex). Each
* thread decrements gate->ngate, releases gate->mutex, and finally leaves pt_gate_sync(). Except for the last thread to wake up: it wakes up the
* thread sleeping on gate->last before exiting pt_gate_sync().
* 
* In order to understand the role of gate->block, consider the following code fragment:
* 
* 
* 
*       gate_t gate;
* 
*         while (condition) {
*           [do something]
*           enter_gate(&gate);
*           [do something else]
*         }
* 
* If the computations in square brackets do not take very long (for example, you're doing a small problem because you're debugging your
* algorithm), it would be possible for a thread that just woke up to almost instantly re-enter the gate and goof things up for the threads that
* are still asleep. This happens more often than you would think, and results in your code mysteriously hanging up. The extra mutex prevents this
* pathology at fairly low cost. In my opinion, tracking down these cases is the most time consuming part of threads programming; such bugs are
* inherently intermittent and depend on the system load average, disk activity, etc.
* 
* Since the threads wake up from pthread_cond_wait() one at a time, it is necessary to restrict access to the gate until all the newly awakened
* threads actually leave the gate (so that ngate==0 and the incoming thread does the right thing at the if in line 99). The code locks access to
* the gate via the gate->block mutex; in fact, it is the first thing locked on entry to pt_gate_sync(). If a thread goes to sleep in line 102, it
* must first release the block so that other threads can get in and open the gate. The last thread wakes the sleeping threads up with the gate
* blocked. Thus, no other threads can enter the gate while the wakeup call occurs. The last thread to leave the gate unlocks the block mutex,
* thus allowing other threads in. Why do we need the gate->last condition variable? Because, as far as I can tell, it is illegal for a thread to
* unlock a mutex held by another thread.
* 
* 
* 
*/
	lmint_t retval;
	retval = 1;
/*   
 *	*sync->pnthreads contains the values of number of threads which will be synchronized
 *	this value must be specified before this function is invoked
 */
	if (*sync->pnthreads<2) {
		return 0;};           /* trivial case            */
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
		retval = 0;
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
	return retval;
}




lmint_t pt_sync_mod(pt_sync_t *sync, lmsize_t addjob, lmsize_t incrm)
{
/*   
 *	*sync->pnthreads contains the values of number of threads which will be synchronized
 *	this value must be specified before this function is invoked
 * 
 * 	in addition to pt_sync() this function enables 
 * 	modification of *sync->pnthreads by incrm which is done when
 * 	the last therad leavs the synchronizer
 */
	lmsize_t n_actual_sync_jobs, retval;
	retval = 1;
/*
 * if number of synced jobs is larger then pnthreads by incrm add it
 */
	n_actual_sync_jobs = *sync->pnthreads + incrm;
	
	if (n_actual_sync_jobs<2)
		return 0;           /* trivial case    */
/*
 * lock the block and mutex
 */
	Pthread_mutex_lock(sync->pblock);

	Pthread_mutex_lock(sync->pmutex);
/*
 * find if the job is last or not  NOTE: *sync->pnsync has to be intialized to 0 before 
 * syncing starts
 */
	if (++(*sync->pnsync) < n_actual_sync_jobs) { 
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
 */	
	else 
  	{
/*
 * modify number of jobs which are synced
 */
		if(addjob > 0){
			if(  (*sync->pnthreads = *sync->pnthreads + *sync->incrm) < 0) *sync->pnthreads = 0;
			*sync->incrm = 0;
		}
/*
 * wake up all waiting processes
 */
		Pthread_cond_broadcast(sync->pcondvar);
/* 
 * got to sleep till they are all awake, then release block
 */
		Pthread_cond_wait(sync->plast,sync->pmutex);
	
		Pthread_mutex_unlock(sync->pblock);
		retval = 0;
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
	
	return retval;
}


lmint_t get_exchange_channel_mode(lmchar_t ATDTMode, lmchar_t KeepAlive_Mode){
/*
 * function returns mode of the socket based on the values of ATDTMode and KeepAlive_Mode
 */
	lmint_t retval;

	if(ATDTMode == 'D' && KeepAlive_Mode == 'N')  /* Direct transfer, close socket */
		retval = 1;
	else if(ATDTMode == 'A' && KeepAlive_Mode == 'N')  /* Alternate transfer, close socket */
		retval = 2;
// 	else if(ATDTMode == 'D' && KeepAlive_Mode == 'C')  /* Direct transfer, close socket after client request it*/
// 		retval = 3;
// 	else if(ATDTMode == 'A' && KeepAlive_Mode == 'C')  /* Alternate transfer, close socket after client request it*/
// 		retval = 4;
	else if(ATDTMode == 'D' && KeepAlive_Mode == 'Y')  /* Direct transfer, do not close socket*/
		retval = 5;
	else if(ATDTMode == 'A' && KeepAlive_Mode == 'Y')  /* Alternate transfer, do not close socket*/
		retval = 6;
	else
		retval = -1;
	
	return retval;
}

lmlint_t Make_ID_Number(lmint_t sockfd){
/* 
 * function creates unique ID for a client
 * the ID consists of 8 bytes of HW address + PID number of the server process + random number
 * Unless specified otherwise, eth0 HWaddress is used
 */	
	lmchar_t buf[8192], macp[60], ip[INET6_ADDRSTRLEN];
	struct ifconf ifc={0};
	struct ifreq *ifr, *interf;
	lmint_t nInterfaces, i, r;
	struct sockaddr *addr;
 	lmlint_t ID;
	
	nInterfaces = 0;
	buf[0] = '\0'; 
	ifr = NULL;
	ip[0] = '\0';
	macp[0] = '\0';
/*
 * Search available interfaces 
 */
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if(ioctl(sockfd, SIOCGIFCONF, &ifc) < 0) {
		Error("Make_ID_Number: ioctl(SIOCGIFCONF)");
		return -1;
	}
/*
 * Loop through the list of interfaces 
 */
	ifr = ifc.ifc_req;
	nInterfaces = ifc.ifc_len / sizeof(struct ifreq);

	for(i = 0; i < nInterfaces; i++) {
		interf = &ifr[i];
		addr = &(interf->ifr_addr);
/*
 * check if device is eth0
 */		
		if( strncmp(interf->ifr_name, "eth0", 4) == 0){
// /*
//  * get back with IP address
//  */
// 			if(ioctl(sockfd, SIOCGIFADDR, interf) < 0) 
// 				Error("Make_ID_Number: ioctl(OSIOCGIFADDR)");
// /*
//  * get back with address of interface
//  */
// 			 if (inet_ntop(AF_INET, &(((struct sockaddr_in *)addr)->sin_addr), ip, sizeof ip) == NULL){
// 				Error("Make_ID_Number: inet_ntop");
// 				continue;
// 			}
/*
 * get back with MAC (HWaddr)
 */
			if(ioctl(sockfd, SIOCGIFHWADDR, interf) < 0) {
				Error("Make_ID_Number: ioctl(SIOCGIFHWADDR)");
				return -1;
			}
			break;
		}
	}
/* 
 * seed random generator numbers
 */  
	srand(time(NULL));
/* 
 * generate rundom number
 */
	r = rand()%1000000;
/*
 * write up together first eight bytes of HWaddress + PID number + random number
 * HWaddr written in upper case
 */
	if (snprintf(macp, 60, "%02X%02X%02X%02X%d%d",
		(unsigned lmchar_t)interf->ifr_hwaddr.sa_data[0],
		(unsigned lmchar_t)interf->ifr_hwaddr.sa_data[1],
		(unsigned lmchar_t)interf->ifr_hwaddr.sa_data[2],
		(unsigned lmchar_t)interf->ifr_hwaddr.sa_data[3],
//     		(unsigned lmchar_t)interf->ifr_hwaddr.sa_data[4],
//     		(unsigned lmchar_t)interf->ifr_hwaddr.sa_data[5],
		getpid(), r
		) < 0)
		Error("Make_ID_Number: snprintf");	
		
	ID = hex2dec(macp);
	return ID;
}



