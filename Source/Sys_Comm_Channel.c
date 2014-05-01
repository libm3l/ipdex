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
 *     Function Sys_Comm_Channel.c
 *
 *     Date: 2014-02-16
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

void *Sys_Comm_Channel(void *arg){

	Server_Comm_DataStr_t *c = (Server_Comm_DataStr_t *)arg;
/*
 * sync the first
 */ 
	lmsize_t incrm;
	
	incrm = 0;

	while(1){
/*
 * initial pt_sync syncing this thread with Server_Body and Data_Thread at the beginning
 * of the arriving requests identification
 */
		pt_sync(c->Data_Thread_Pointer->psync);
		
		if(*c->Data_Thread_Pointer->pcheckdata == 0){
/*
 * request was a usual request
 */
			pt_sync(c->Data_Thread_Pointer->psync);
		}
		else{
/*
 * request was a sys_link request
 * add or delete data set
 */
// 			Sem_post(c->Data_Thread_Pointer->psem);
// 			pt_sync_mod_sem(c->Data_Thread_Pointer->psync, 0,incrm,c->Data_Thread_Pointer->psem);
		}
	}
/*
 * free borrowed memory
 */
	free(c->Data_Thread_Pointer->pData_Str);
	free(c->Data_Thread_Pointer);
	free(c->data_threadsPID);
	free(c);

	return NULL;
}