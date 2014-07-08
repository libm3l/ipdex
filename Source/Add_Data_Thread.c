/*
 *     Copyright (C) 2014  Adam Jirasek
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
 *     Function Add_Data_Thread.c
 *
 *     Date: 2014-06-18
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
#include "Add_Data_Thread.h"
#include "Data_Thread.h"
#include "Server_Functions_Prt.h"
#include "Associate_Data_Thread.h"
#include "Allocate_DataBuffer.h"

lmsize_t Add_Data_Thread(node_t *Gnode, data_thread_str_t *Data_Thread, node_t **Buffer){
/*
 * function spawns additional Data_Thread
 * 
 * Allocated data set in this function are freed when threads are finished (ie. satement free(c) in Data_Thread.c) 
 * and the main data structure is freed in function invoking this function (ie. Server_Body.c)
 * 
 * the heap memery was allocated in Allocate_Data_Thread_DataSet
 */
	lmsize_t newnum;
	lmint_t pth_err;
	find_t *SFounds;
	data_thread_args_t *DataArgs;
	node_t *List;
	data_thread_int_str_t **Tmp;
	
	if(Gnode == NULL){
		Warning("Add_Data_Thread: NULL Gnode");
		return -1;
	}
/*
 * find how many data sets - defines how many data_threads to spawn
 */
	if( (SFounds = m3l_Locate(Gnode, "/_sys_comm_/Channel", "/*/*", (lmchar_t *)NULL)) == NULL){
		printf("Add_Data_Thread: did not find any Channel data\n");
		return -1;
	}
	
	Pthread_mutex_lock(&Data_Thread->lock);
/*
 * new thread is going to be added to the exiting threads
 */
	if(  (newnum = Data_Thread->nall_data_threads+1) < 1)
		return 0;
/* 
 * malloc data Data_Thread->Data_Str, will be used to store data specific to each Data_Thread (ie. PID, name of channel etc.)
 * the data is then freed in Data_Thread.c function
 */
	if(Data_Thread->Data_Str != NULL){
		Tmp = NULL;
		
		if( ( Tmp = (data_thread_int_str_t **)realloc( Data_Thread->Data_Str, 
			newnum*sizeof(data_thread_int_str_t *))) == NULL){

			Perror("Add_Data_Thread: realloc error");
		}
		Data_Thread->Data_Str = Tmp;
	}
	else{
/*
 * this situation can happen only if the Server is started without any 
 * preopened channels
 */
		if( (Data_Thread->Data_Str = (data_thread_int_str_t **)malloc(sizeof(data_thread_int_str_t *))) == NULL)
			Perror("Start_Data_Thread: Data_Thread->Data_Str malloc");
	}
	newnum--;
/*
 * allocate Data_Str - Same as in Start_Data_Threads
 */
	if( (Data_Thread->Data_Str[newnum] = (data_thread_int_str_t *)malloc(sizeof(data_thread_int_str_t))) == NULL)
		Perror("Add_Data_Thread: Data_Thread->Data_Str[] malloc");
/*
 * malloc for PID of the thread
 */
	if( (Data_Thread->Data_Str[newnum]->data_threadPID = (pthread_t *)malloc(sizeof(pthread_t))) == NULL)
		Perror("Add_Data_Thread: Data_Thread->Data_Str->data_threads malloc");
/*
 * malloc variable for its name (connection.channel name). This variable
 * will be then defined in Add_Data_Thread where each individual thread 
 * gets its own connection
 */
	if( (Data_Thread->Data_Str[newnum]->name_of_channel = (lmchar_t *)malloc(MAX_NAME_LENGTH* sizeof(lmchar_t))) == NULL)
		Perror("Add_Data_Thread: Data_Thread->Data_Str->name_of_channel malloc");
/*
 * malloc status_run variable and set it to 1; if 0 thread will be terminated
 */
	if( (Data_Thread->Data_Str[newnum]->status_run = (lmint_t *)malloc(MAX_NAME_LENGTH* sizeof(lmint_t))) == NULL)
		Perror("Add_Data_Thread: Data_Thread->Data_Str->lmint_t malloc");
	*Data_Thread->Data_Str[newnum]->status_run = 1;
/*
 * increase number of threads by 1
 * number of synced threads (*Data_Thread->sync->nthreads) will be increased
 * in pt_sync_mod() in either Data_Thread or Sever_Body
 */
	*Data_Thread->n_data_threads     =  *Data_Thread->n_data_threads + 1;
	Data_Thread->nall_data_threads  =  Data_Thread->nall_data_threads + 1;
/*
 * set Node pointer to data set in /_sys_comm_/Channel
 */
	List = m3l_get_Found_node(SFounds, 0);
/*
 * dettach the list from the main tree so that you can later move it to Buffer
 * the remaining of the list will be freed in Server_Body
 */
	if( m3l_detach_list(0, &List, (opts_t *)NULL) < 0)
		Error("Add_Data_Thread: m3l_detach_list error");
/*
 * add additional data (Thread_Status etc.)
 */
	if( Additional_Data2Buffer(&List) != 1)
		Error("Add_Data_Thread: Additional_Data2Buffer");
/*
 * associate data with Data_Thread
 * as a counter of the Channel use 1 as there is always one Channel
 * in _sys_comm_ request
 */
	if(  (DataArgs = Associate_Data_Thread(List, Data_Thread, newnum, 1)) == NULL)
		Error("Add_Data_Thread: DataArgs NULL pointer");
/*
 * create thread
 */
	while ( ( pth_err = pthread_create(Data_Thread->Data_Str[newnum]->data_threadPID, NULL, &Data_Threads,  DataArgs)) != 0 && errno == EAGAIN);
	if(pth_err != 0)
		Perror("pthread_create()");

	m3l_DestroyFound(&SFounds);
/*
 * add List to Buffer
 */
	if( m3l_Mv(&List,  "./Channel", "./*", Buffer, "/Buffer", "/*", (lmchar_t *)NULL) == -1)
		Error("Add_Data_Thread: Mv");

	Pthread_mutex_unlock(&Data_Thread->lock);

	return 1;
}