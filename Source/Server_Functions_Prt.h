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
 *     Header file Server_Functions_Prt.h
 *
 *     Date: 2013-07-04
 * 
 *
 *     Modifications:
 *     Date		Version		Patch number		CLA 
 *
 *
 *     Description
 * 
 */




#ifndef __SERVER_FUNCTION_PRT_H__
#define __SERVER_FUNCTION_PRT_H__

extern void Pthread_barrier_init(pthread_barrier_t *, lmint_t);
extern void Pthread_barrier_destroy(pthread_barrier_t *);
extern void Pthread_barrier_wait(pthread_barrier_t *);

extern void Pthread_mutex_init(pthread_mutex_t *);
extern void Pthread_mutex_lock(pthread_mutex_t *);
extern void Pthread_mutex_unlock(pthread_mutex_t *);
extern void Pthread_mutex_destroy(pthread_mutex_t *);

extern void Pthread_cond_init(pthread_cond_t *);
extern void Pthread_cond_destroy(pthread_cond_t *);
extern void Pthread_cond_signal(pthread_cond_t *);
extern void Pthread_cond_broadcast(pthread_cond_t *);
extern void Pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);


extern void Sem_init(sem_t *, lmuint_t);
extern void Sem_wait(sem_t *);
extern void Sem_post(sem_t *);
extern void Sem_destroy(sem_t *);
extern lmint_t Sem_getvalue(sem_t *);

extern node_t *sender_identification(lmchar_t *, lmchar_t );
extern void pt_sync(pt_sync_t *sync);

// extern void Sync(lmint_t);

#endif