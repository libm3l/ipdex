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
 *     Function Associate_Data_Thread.c
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
#include "Associate_Data_Thread.h"


data_thread_args_t *Associate_Data_Thread(node_t *List, data_thread_str_t *Data_Thread, lmsize_t i, lmint_t yes){
/*
 * function allocate DataArgs and associate it with 
 * Data_Thread data set
 * invoked from Start_Data_Thread and Stat_SysCom_Thread
 * 
 * if yes larger then 0 malloc additional array DataArgs->pData_Str which stores name of channel and status_run
 */	
		data_thread_args_t *DataArgs;
		
		if( (DataArgs = (data_thread_args_t *)malloc(sizeof(data_thread_args_t))) == NULL)
			Perror("Associate_Data_Thread: DataArgs malloc");
		DataArgs->Node  		=  List;

		DataArgs->plock 		= &Data_Thread->lock;	
		DataArgs->pcond 		= &Data_Thread->cond;	
		DataArgs->psocket    		=  Data_Thread->socket;	
		DataArgs->pretval    		=  Data_Thread->retval;	
		DataArgs->pcounter    		=  Data_Thread->n_data_threads;
		DataArgs->prcounter    		=  Data_Thread->data_threads_remainth_counter;
		DataArgs->pname_of_data_set   	=  Data_Thread->name_of_data_set;
		DataArgs->pSR_mode	    	=  Data_Thread->SR_mode;
		DataArgs->pcheckdata 		=  Data_Thread->checkdata;
		DataArgs->psem	 		= &Data_Thread->sem;

		DataArgs->psync 		=  Data_Thread->sync;
		DataArgs->psync->pnsync 	=  Data_Thread->sync->nsync;
		DataArgs->psync->pnthreads 	=  Data_Thread->sync->nthreads;
		DataArgs->psync->pmutex 	= &Data_Thread->sync->mutex;
		DataArgs->psync->pblock		= &Data_Thread->sync->block;
		DataArgs->psync->pcondvar	= &Data_Thread->sync->condvar;
		DataArgs->psync->plast		= &Data_Thread->sync->last;
		DataArgs->psync->pincrm		=  Data_Thread->sync->incrm;
/*
 * malloc pData_Str and associate with Data_Str
 * only if specified
 */
		if(yes > 0){
			if( (DataArgs->pData_Str = (data_thread_int_str_t *)malloc(sizeof(data_thread_int_str_t))) == NULL)
				Perror("Associate_Data_Thread: DataArgs->pData_Str malloc");	
			DataArgs->pData_Str->name_of_channel = Data_Thread->Data_Str[i]->name_of_channel;
// 			DataArgs->pData_Str->status_run      = Data_Thread->Data_Str[i]->status_run;
		}
		else
			DataArgs->pData_Str = NULL;
		
		return DataArgs;
}