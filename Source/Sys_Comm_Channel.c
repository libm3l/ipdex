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
#include "Sys_Comm_Channel.h"

lmint_t Sys_Comm_Channel(node_t *RecNode, data_thread_str_t *Data_Threads)
{
/*
 * function handles communication between client and server via SERVER_SYS_LINK channel.
 * This channel is used to process requests sent by clients to the server.
 * Some of the stask managen through this channel are
 * 	-	add/remove data channel
 * 	-	leave Data_Thread (SR_Hub) for KA = Y
 * 	-	increase/decrease number of S_R threads for a particular Data_Thread
 * 	-	change KA and/or ATDRT mode
 */
// 	if( (SFounds = m3l_Locate(Gnode, "/Buffer/Channel", "/*/*", (lmchar_t *)NULL)) != NULL){
// 		
// 		Data_Thread->n_data_threads = m3l_get_Found_number(SFounds);
// 		
// 		if(Data_Thread->n_data_threads == 0){
// 			Error("Server: did not find any Data_set");
// 			m3l_DestroyFound(&SFounds);
// 		}
// 	}
// 	else
// 	{
// 		printf("Server: did not find any Data_set\n");
// 		exit(0);
// 	}
// 	
// 	
// 	
// 	
// 	
// 	
// 			if( (SFounds = m3l_Locate(c->Node, "/Buffer/Channel/Name_of_Channel", "./*/*",  (lmchar_t *)NULL)) != NULL){
// 			
// 			if( m3l_get_Found_number(SFounds) != 1)
// 				Error("Data_Thread: Only one Name_of_Channel per Channel allowed");
// /* 
//  * pointer to list of found nodes
//  */
// 				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
// 					Error("Data_Thread: NULL Name_of_Channel");
// 			
// 				data_set_name = m3l_get_data_pointer(List);
// 				if( (len = m3l_get_List_totdim(List)-1) < 1)
// 					Error("Data_Thread: too short name of data set");
// 				if( snprintf(local_set_name, MAX_NAME_LENGTH,"%s",data_set_name) < 0)
// 					Perror("snprintf");
// 				local_set_name[len] ='\0';
// /* 
//  * free memory allocated in m3l_Locate
//  */
// 			m3l_DestroyFound(&SFounds);
// 		}
// 		else
// 		{
// 			Error("Data_Thread: Name_of_Channel not found\n");
// 		}

}