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
 *     Function SR_hub.c
 *
 *     Date: 2013-09-08
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

#include "SR_hub.h"

static void terminal_loop_sequence(SR_hub_thread_str_t *c);


//      mode 1: ATDTMode == 'D' && KeepAlive_Mode == 'N'  /* Direct transfer, close socket */
//      mode 2: ATDTMode == 'A' && KeepAlive_Mode == 'N'  /* Alternate transfer, close socket */
//      mode 3: ATDTMode == 'D' && KeepAlive_Mode == 'C'  /* Direct transfer, close socket after client request it*/
//      mode 4: ATDTMode == 'A' && KeepAlive_Mode == 'C'  /* Alternate transfer, close socket after client request it*/
//      mode 5: ATDTMode == 'D' && KeepAlive_Mode == 'Y'  /* Direct transfer, do not close socket*/
//      mode 6: ATDTMode == 'A' && KeepAlive_Mode == 'Y'  /* Alternate transfer, do not close socket*/

void *SR_hub(void *arg)
{
/*
 * function signals SR_Threads that all requests (one Sender and n_rec_proc Receivers) arrived and 
 * that it can start data transfer
 */
	lmint_t IT;
	node_t *List;
	find_t *SFounds;
	lmchar_t *ATDTMode, *KeepAlive_Mode;
	
	opts_t *Popts, opts;

	Popts = &opts;
	m3l_set_Find(&Popts);
/*
 * NOTE in previuous version, this function was run after the while(n_avail_loc_theads != 0);  // all connecting thread arrivied, ie. one Sender and n_rec_proc Receivers 
 * in Data_Thread ended. It did not work when more then one data set arrived and pt_sync in Data_Thread in do loop was not behaving properly
 * due to the fact that the number os synced threads was changing. To avoid that problem the process of signaling SR_threads are run in parallel 
 * with Data_Thread
 */
	SR_hub_thread_str_t *c = (SR_hub_thread_str_t *)arg;
/*
 * find AT-DT mode
 * find KEEP_CONN_ALIVE_Mode
 * when working with c->pList, lock it, it is shared between
 * all threads and access it without lock protection causes SIGSEV
 */
	Pthread_mutex_lock(c->plock);

		if( (SFounds = m3l_locate(c->pList, "./Channel/CONNECTION/ATDT_Mode", "./*/*/*",  Popts)) != NULL){

			if( m3l_get_Found_number(SFounds) != 1){
				Error("SR_hub: Only one CONNECTION/ATDT_Mode per Channel allowed");
				free(c);
				return NULL;
			}
/* 
 * pointer to list of found nodes
 */
			if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
				Error("SR_hub: NULL ATDT_Mode");
			
			ATDTMode = (lmchar_t *)m3l_get_data_pointer(List);
			*c->pATDT_mode = *ATDTMode;
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			Error("SR_hub: CONNECTION/ATDT_Mode not found\n");
			free(c);
			return NULL;
		}

		if( (SFounds = m3l_locate(c->pList, "./Channel/CONNECTION/KEEP_CONN_ALIVE_Mode", "./*/*/*",  Popts)) != NULL){

			if( m3l_get_Found_number(SFounds) != 1){
				Error("SR_hub: Only one CONNECTION/KEEP_CONN_ALIVE_Mode per Channel allowed");
				free(c);
				return NULL;
			}
/* 
 * pointer to list of found nodes
 */
			if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
				Error("SR_hub: NULL KEEP_CONN_ALIVE_Mode");
			
			KeepAlive_Mode = (lmchar_t *)m3l_get_data_pointer(List);
			*c->pKA_mode 	= *KeepAlive_Mode;
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			Error("SR_hub: CONNECTION/KEEP_CONN_ALIVE_Mode not found\n");
			free(c);
			return NULL;
		}
/*
 * if ATDT == A(lternate) the communication includes one S(ender) and once R(eceiver)
 * if more R(eceivers) are included, give error message and quit
 */
	if( *ATDTMode == 'A' && *c->pn_rec_proc > 1){
		Error("SR_hub - ATDT mode can be A only if the communication is established between one Sender and one Receiver");
		free(c);
		return NULL;
	}
/*
 * determine mode number
 */
	if ( (*c->pSRh_mode = get_exchange_channel_mode(*ATDTMode, *KeepAlive_Mode)) == -1){
		Error("SR_hub: Wrong transfer mode");
		free(c);
		return NULL;
	}
	Pthread_mutex_unlock(c->plock);
/*
 * sync all SR_Threads and SR_hub so that Data_Thread goes further once they 
 * are all spawned. Without that there were problems with case 200 where Data_Thread 
 * sometimes deleted List before SR_Hub started, upon start SR_hub needs to identify 
 * some values from the list.
 * 
 * The value of processes which are synced on this pt_sync_mod is increased by 2, ie.
 * number of SR_Data_Threads + SR_Hub + Data_Thread
 */
	pt_sync_mod(c->psync_loc, 0, 2);

/*
 * start loop for transfer
 */
	switch(*c->pSRh_mode){
		case 1:
			while(1){
/*
 * wait for semaphore from Data_Thread that 
 * all requests arrived
 */
				Sem_wait(c->psem);
/*
 * wait until all SR_threads reach pt_sync, then start actual transfer of the data from S to R(s)
 * becasue the internal counter of synced jobs is set to S+R, we have to add 1 so that SR_Hub is 
 * synced too
 */
				pt_sync_mod(c->psync_loc, 0, 1);
/*
 * once the data transfer is finished wait until all data is tranferred and S and R threads close their socket
*/
				Sem_wait(c->psem_g);
/* 
 * if connection required to be closed, terminate while loop
 */
				if(*c->pstatus_run_h != 1) break;
				terminal_loop_sequence(c);
			}
		break;

		case 2:
			while(1){
/*
 * wait for semaphore from Data_Thread that 
 * all requests arrived
 */
				Sem_wait(c->psem);
				IT = 2;
/*
 * wait until all SR_threads reach syn, then start actual transfer of the data from S to R(s)
 */
/*
 * wait until all SR_threads reach pt_sync, then start actual transfer of the data from S to R(s)
 * becasue the internal counter of synced jobs is set to S+R, we have to add 1 so that SR_Hub is 
 * synced too
 */
				pt_sync_mod(c->psync_loc, 0, 1);
/*
 * do 2 loops (ie. Sender-to-receiver   and   Recevier-to-Sender) and then continue
 */
				while(--IT != 0);
/*
 * once the data transfer is finished wait until all data is tranferred and S and R threads close their socket
 */
				Sem_wait(c->psem_g);
/* 
 * if connection required to be closed, terminate while loop
 */
				if(*c->pstatus_run_h != 1) break;
				terminal_loop_sequence(c);
			}
		break;
			
		case 5:
		case 6:
			while(1){
/*
 * wait for semaphore from Data_Thread that 
 * all requests arrived
 */
				Sem_wait(c->psem);

/*
 * wait until all SR_threads reach pt_sync, then start actual transfer of the data from S to R(s)
 * becasue the internal counter of synced jobs is set to S+R, we have to add 1 so that SR_Hub is 
 * synced too
 */
				pt_sync_mod(c->psync_loc, 0, 1);
				if(*c->pstatus_run_h != 1) break;

				terminal_loop_sequence(c);
			}
		break;
	}
/*
 * Post semaphore, Data_Thread is vaiting for it so that it can start joinign 
 * all SR_Data_Threads and SR_Hub threads
 */
// 	Sem_post(c->psem);
/*
 * join the thread in  Data_Thread, let Data_Thread free borrowed memory
 */
// pthread_exit(NULL);
return NULL;
}

void terminal_loop_sequence(SR_hub_thread_str_t *c){
  
	Pthread_mutex_lock(c->plock);
/*
 * set the number of available threads for SR transfer to S + R(s) number of threads
 * counter used in Data_Thread function to determine if all threads arrived
 * the pn_avail_loc_theads is decremented in Data_Thread every time the Data_Thread identifies 
 * arriving data set
 */
		*c->pn_avail_loc_theads = *c->pn_rec_proc + 1;
/*
 * increase ncount of available Data_Threads
 */
		(*c->prcounter)++;
/*
 * release thread, ie. set Thread_Status = 0, S_Status and R_Status
 * these values are used in "buffer" and are used during identification 
 * process
 */
		*c->pThread_Status   = 0;	/* thread can be used again */
		*c->pThread_S_Status = 0;	/* number of connected S processes is 0 */
		*c->pThread_R_Status = 0;	/* number of connected R processes is 0 */
/*
 * indicate this thread is empty. ie. all its hubs are free 
 * in case at least one process for this Dat_Thread arrives, the Data_Thread
 * sets it to 2, this is done to prevent closing thread while at least one client
 * opens up connection
 */
		*c->pstatus_run_DataThr_h = 1; 
/*
 * if all threads were occupied, ie *Data_Threads->n_data_threads == *c->pcounter == 0
 * the server is waiting for signal before the continuing with data process identification. 
 * This is done in Server_Body before syncing with data threads
 * If this happens, signal Server_Body that at least one data_thread is avaiable 
 * 
 * This statement is valid is "fixed" specified upon start of the server (see 
 * sequence in Server_Body:
 * 
 * 		"if( Popts_SB->opt_f == 'f'){
 *			if(*Data_Threads->n_data_threads == 0){"
 */
		if(*c->pcounter == 1)
			Pthread_cond_signal(c->pcond);
		
	Pthread_mutex_unlock(c->plock);
}

