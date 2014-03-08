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
 *     Function Start_SysComm_Thread.c
 *
 *     Date: 2014-03-08
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
#include "Start_SysComm_Thread.h"
#include "Sys_Comm_Channel.h"
#include "Server_Functions_Prt.h"
#include "Associate_Data_Thread.h"

lmsize_t Start_SysComm_Thread(data_thread_str_t *Data_Thread){
/*
 */
	lmsize_t i, retval;
	lmint_t pth_err;
	Server_Comm_DataStr_t *SysCommDatSet;
/*
 * spawn threads
 */	
// 	if( (SysComData = (Server_Comm_DataStr_t *)malloc(sizeof(Server_Comm_DataStr_t))) == NULL)
// 		Perror("Start_SysComm_Thread: SysComData malloc");
// 
// 	if( (SysComData->data_threads = (pthread_t *)malloc(sizeof(pthread_t))) == NULL)
// 		Perror("Start_SysComm_Thread: SysComData->data_threads malloc");
// 	
// 	if( (SysComData->Data_Thread_Pointer = (data_thread_args_t *)malloc(sizeof(data_thread_args_t))) == NULL)
// 		Perror("Start_SysComm_Thread: SysComData->Data_Thread_Pointer malloc");
// 
// 	if( (SysComData->SR_Data_Thread_Pointer = (SR_thread_str_t *)malloc(sizeof(SR_thread_str_t))) == NULL)
// 		Perror("Start_SysComm_Thread: SysComData->SR_Data_Thread_Pointer malloc");
/*
 * set Node pointer to i-th data set in /Buffer/Channel
 * this determines that i-th thread will take care of channel with 
 * name specified in /Buffer/Channel/Name_of_Channel
 */
// 		DataArgs->Node  		= m3l_get_Found_node(SFounds, i);
// 		DataArgs->plock 		= &Data_Thread->lock;	
// 		DataArgs->psem 			= &Data_Thread->sem;	
// 		DataArgs->pcond 		= &Data_Thread->cond;	
// 		DataArgs->psocket    		=  Data_Thread->socket;	
// 		DataArgs->pretval    		=  Data_Thread->retval;	
// 		DataArgs->pcounter    		=  Data_Thread->data_threads_availth_counter;
// 		DataArgs->prcounter    		=  Data_Thread->data_threads_remainth_counter;
// 		DataArgs->pname_of_data_set   	=  Data_Thread->name_of_data_set;
// 		DataArgs->pSR_mode	    	=  Data_Thread->SR_mode;
// 		DataArgs->pcheckdata 		=  Data_Thread->checkdata;
// 
// 		DataArgs->psync 		= Data_Thread->sync;
// 		DataArgs->psync->pnsync 	= Data_Thread->sync->nsync;
// 		DataArgs->psync->pnthreads 	= Data_Thread->sync->nthreads;
// 		DataArgs->psync->pmutex 	= &Data_Thread->sync->mutex;
// 		DataArgs->psync->pblock		= &Data_Thread->sync->block;
// 		DataArgs->psync->pcondvar	= &Data_Thread->sync->condvar;
// 		DataArgs->psync->plast		= &Data_Thread->sync->last;
/*
 * malloc pData_Str and associate with Data_Str
 */
// 		if( (DataArgs->pData_Str = (data_thread_int_str_t *)malloc(sizeof(data_thread_int_str_t))) == NULL)
// 			Perror("Data_Thread: DataArgs->pData_Str malloc");	
// 		DataArgs->pData_Str->name_of_channel = Data_Thread->Data_Str[i]->name_of_channel;
// 		DataArgs->pData_Str->status_run      = Data_Thread->Data_Str[i]->status_run;
/*
 * create thread
 */

// 		while ( ( pth_err = pthread_create(Data_Thread->Data_Str->data_threadPID, NULL, &Data_Threads,  DataArgs)) != 0 && errno == EAGAIN);
// 		if(pth_err != 0)
// 			Perror("pthread_create()");
/*
 * create a node
 */	
	return retval;
}