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
 *     Function Start_SR_HubThread.c
 *
 *     Date: 2013-07-19
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
#include "Start_SR_HubThread.h"
#include "SR_hub.h"
#include "Server_Functions_Prt.h"
#include "SR_hub.h"
#include "Server_Functions_Prt.h"


SR_hub_thread_str_t *Start_SR_HubThread(SR_thread_str_t *SR_Threads, data_thread_args_t *Data_Thread, lmsize_t *n_avail_loc_theads, lmsize_t  *n_rec_proc, lmint_t *Thread_Status, sem_t *loc_sem, lmint_t *Thread_S_Status, lmsize_t *Thread_R_Status){
/*
 * function starts SR_Hub threads. Invoked from Data_Thread
 */
	lmint_t pth_err;
	SR_hub_thread_str_t  *SR_Hub_Thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
/*
 * malloc the main node
 */
	if( (SR_Hub_Thread = (SR_hub_thread_str_t *)malloc(sizeof(SR_hub_thread_str_t))) == NULL)
		Perror("Start_SR_HubThread: SR_Hub_Thread malloc");
/* 
 * malloc data in heap, will be used to share data between threads
 */
	if( (SR_Hub_Thread->data_thread = (pthread_t *)malloc(sizeof(pthread_t) )) == NULL)
		Perror("Start_SR_HubThread: SR_Hub_Thread->data_thread malloc");
/*
 * associate values used in SR_Hub_Thread 
 */
	SR_Hub_Thread->psem 		= loc_sem;
	SR_Hub_Thread->psem_g		= &SR_Threads->sem_g;	/* once the data transfer is finished increase 
							increment of available data_threads */
	SR_Hub_Thread->plock		= Data_Thread->plock;
	SR_Hub_Thread->pcond		= Data_Thread->pcond;
	SR_Hub_Thread->pcounter		= Data_Thread->pcounter;
	SR_Hub_Thread->pstatus_run_DataThr_h = Data_Thread->pData_Str->status_run;
	
	SR_Hub_Thread->pn_avail_loc_theads = n_avail_loc_theads;
	SR_Hub_Thread->pn_rec_proc	= n_rec_proc;
	SR_Hub_Thread->pThread_Status 	= Thread_Status;
	SR_Hub_Thread->pThread_S_Status	= Thread_S_Status;
	SR_Hub_Thread->pThread_R_Status = Thread_R_Status;
	SR_Hub_Thread->prcounter 	= Data_Thread->prcounter;
	SR_Hub_Thread->psockfd		= SR_Threads->sockfd;
	SR_Hub_Thread->pList		= Data_Thread->Node;
	SR_Hub_Thread->pATDT_mode	= SR_Threads->ATDT_mode;  /* associate pointer, the values will be filled in SR_hub */
	SR_Hub_Thread->pKA_mode		= SR_Threads->KA_mode;    /* associate pointer, the values will be filled in SR_hub */
	SR_Hub_Thread->pSRh_mode	= SR_Threads->mode;       /* associate pointer, the values will be filled in SR_hub */
	SR_Hub_Thread->pstatus_run_h	= SR_Threads->status_run;
								
	SR_Hub_Thread->psync_loc 		= SR_Threads->sync_loc;
	SR_Hub_Thread->psync_loc->pnsync 	= SR_Threads->sync_loc->nsync;
	SR_Hub_Thread->psync_loc->pnthreads	= SR_Threads->sync_loc->nthreads;
	SR_Hub_Thread->psync_loc->pmutex 	= &SR_Threads->sync_loc->mutex;
	SR_Hub_Thread->psync_loc->pblock	= &SR_Threads->sync_loc->block;
	SR_Hub_Thread->psync_loc->pcondvar	= &SR_Threads->sync_loc->condvar;
	SR_Hub_Thread->psync_loc->plast		= &SR_Threads->sync_loc->last;

	while ( (pth_err = pthread_create(&SR_Hub_Thread->data_thread[0], &attr, &SR_hub,  SR_Hub_Thread)) != 0 && errno == EAGAIN);
	if(pth_err != 0)
		Perror("Start_SR_HubThread: pthread_create()"); 

	return SR_Hub_Thread;
}