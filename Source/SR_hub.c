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
#include "Server_Header.h"
#include "Server_Functions_Prt.h"

#include "SR_hub.h"

static lmint_t valueOfSharedVariable(SR_hub_thread_str_t *);

void *SR_hub(void *arg)
{
/*
 * function signals SR_Threads that all requests (one Sender and n_rec_proc Receivers) arrived and 
 * that it can start data transfer
 */
	lmsize_t i;
	lmint_t IT;
	node_t *List;
	find_t *SFounds;
	lmchar_t *ATDTMode, *KeepAllive_Mode;
/*
 * NOTE in previuous version, this function was run after the while(n_avail_loc_theads != 0);  // all connecting thread arrivied, ie. one Sender and n_rec_proc Receivers 
 * in Thread_Prt ended. It did not work when more then one data set arrived and pt_sync in Thread_Prt in do loop was not behaving properly
 * due to the fact that the number os synced threads was changing. To avoid that problem the process of signaling SR_threads are run in parallel 
 * with Thread_Prt
 */
	SR_hub_thread_str_t *c = (SR_hub_thread_str_t *)arg;

// 		if(m3l_Cat(c->pList, "--all", "-L", "-P", "*",   (lmchar_t *)NULL) != 0)
// 			Warning("CatData");
/*
 * find AT-DT mode
 * find KEEP_CONN_ALIVE_Mode
 * when working with c->pList, lock it, it is shared between
 * all threads and access it without lock protection causes SIGSEV
 */
	Pthread_mutex_lock(c->plock);

		if( (SFounds = m3l_Locate(c->pList, "./Data_Set/CONNECTION/ATDT_Mode", "./*/*/*",  (lmchar_t *)NULL)) != NULL){
			
			if( m3l_get_Found_number(SFounds) != 1)
				Error("SR_hub: Only one CONNECTION/ATDT_Mode per Data_Set allowed");
/* 
 * pointer to list of found nodes
 */
				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
					Error("Thread_Prt: NULL ATDT_Mode");
			
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
		}

		if( (SFounds = m3l_Locate(c->pList, "./Data_Set/CONNECTION/KEEP_CONN_ALIVE_Mode", "./*/*/*",  (lmchar_t *)NULL)) != NULL){
			
			if( m3l_get_Found_number(SFounds) != 1)
				Error("SR_hub: Only one CONNECTION/KEEP_CONN_ALIVE_Mode per Data_Set allowed");
/* 
 * pointer to list of found nodes
 */
				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
					Error("SR_hub: NULL KEEP_CONN_ALIVE_Mode");
			
				KeepAllive_Mode = (lmchar_t *)m3l_get_data_pointer(List);
				*c->pKA_mode 	= *KeepAllive_Mode;
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			Error("SR_hub: CONNECTION/KEEP_CONN_ALIVE_Mode not found\n");
		}

	Pthread_mutex_unlock(c->plock);
/*
 * if ATDT == A(lternate) the communication includes one S(ender) and once R(eceiver)
 * if more R(eceivers) are included, give error message and quit
 */
	if( *ATDTMode == 'A' && *c->pn_rec_proc > 1)
		Error("SR_hub - ATDT mode can be A only if the communication is established between one Sender and one Receiver");
/*
 * determine mode number
 */
	if ( (*c->pSRh_mode = get_exchange_channel_mode(*ATDTMode, *KeepAllive_Mode)) == -1)
		Error("SR_hub: Wrong transfer mode");
/*
 * start loop for transfer
 */
	while(1){
/*
 * wait for semaphore from Thread_Prt that 
 * all requests arrived
 */
		Sem_wait(c->psem);
			
			switch(*c->pSRh_mode){
				case 1:
/*
 * wait until all SR_threads reach barrier, then start actual transfer of the data from S to R(s)
 */
					Pthread_barrier_wait(c->pbarr);
/*
 * once the data transfer is finished wait until all data is tranferred and S and R threads close their socket
*/
					Sem_wait(c->psem_g);
				break;
				
				case 2:
					
					IT = 2;
					do{
/*
 * wait until all SR_threads reach barrier, then start actual transfer of the data from S to R(s)
 */
						Pthread_barrier_wait(c->pbarr);
/*
 * once the data transfer is finished wait until all data is tranferred and S and R threads close their socket
 */
						Sem_wait(c->psem_g);
						
					}while(--IT == 0);
				break;
			
				case 3:
					do{
						Pthread_barrier_wait(c->pbarr);
						Sem_wait(c->psem_g);   
// 					}while(*c->pEOFC_ENDh == 1);  /* this value is not protected by mtex
// 							at this point should be accessed only by this process */
					}while(valueOfSharedVariable(c) == 1);

				case 4:
					do{
						IT = 2;
						do{
							Pthread_barrier_wait(c->pbarr);
							Sem_wait(c->psem_g);  
						}while(--IT == 0);
// 					}while(*c->pEOFC_ENDh == 1);  /* this value is not protected by mtex
// 							at this point should be accessed only by this process */
					}while(valueOfSharedVariable(c) == 1);
				break;
			
				case 5:
				case 6:
					while(1){
						IT = 2;
						do{
							Pthread_barrier_wait(c->pbarr);
							Sem_wait(c->psem_g);
						}while(--IT == 0);
					}
				break;
			}

		Pthread_mutex_lock(c->plock);
/*
 * set the number of available threads for SR transfer to S + R(s) number of threads
 * counter used in Thread_Prt function to determine if all threads arrived
 * the pn_avail_loc_theads is decremented in Thread_Prt every time the Thread_Prt identifies 
 * arriving data set
 */
			*c->pn_avail_loc_theads = *c->pn_rec_proc + 1;
/*
 * close sockets (moved to SR_Data_Thread)
 * 
// 			for(i=0; i<*c->pn_avail_loc_theads; i++){
// 				if( close(c->psockfd[i]) == -1)
// 					Perror("close");
// 			}
/*
 * increase ncount of available Data_Threads
 */
			(*c->prcounter)++;
/*
 * release thread, ie. set Thread_Status = 0
 */
			*c->pThread_Status = 0;
/*
 * if all threads were occupied, ie *Data_Threads->data_threads_availth_counter == *c->pcounter == 0
 * the server is waiting for signal before the continuing with data process identification. 
 * This is done in Server_Body before syncing with data threads
 * If this happens, signal Server_Body that at least one data_thread is avaiable 
 */
			if(*c->pcounter == 1)
				Pthread_cond_signal(c->pcond);
		
		Pthread_mutex_unlock(c->plock);
	}
/*
 * release borrowed memory, malloced before starting thread in Data_Thread()
 */
	free(c);

	return NULL;

}


lmint_t valueOfSharedVariable(SR_hub_thread_str_t *c)
{
	lmint_t status;
	lmint_t result;

	Pthread_mutex_lock(c->plock);
	result = *c->pEOFC_ENDh;

	Pthread_mutex_unlock(c->plock);
	return result;

}
