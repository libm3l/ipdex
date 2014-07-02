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
 *     Function Allocate_DataBuffer.c
 *
 *     Date: 2013-07-16
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
#include "Allocate_DataBuffer.h"

node_t *Allocate_DataBuffer(node_t *Gnode){
/*
 * this routine allocates a structure containign 
 * information about transfered data sets and number of threads 
 * each data set is going to be accessed by
 */
	lmsize_t i, n_data_threads, *dim;
	find_t *SFounds=NULL;
	node_t *BuffNode=NULL, *TmpNode=NULL;
	
	if(Gnode == NULL){
		Warning("Allocate_DataBuffer: NULL Gnode");
		return NULL;
	}
	
// 	Data_Thread->nall_data_threads = 0;
// 	Data_Thread->n_data_threads = 0;
/*
 * find how many data sets - defines how many data_threads will be required
 */
	if( (SFounds = m3l_Locate(Gnode, "/COMM_DEF/Channels/Channel", "/*/*/*", (lmchar_t *)NULL)) != NULL){
		
		n_data_threads = m3l_get_Found_number(SFounds);
// 		Data_Thread->nall_data_threads = n_data_threads;
		
		if(n_data_threads == 0){
			Error("Allocate_DataBuffer: did not find any /COMM_DEF/Channels/Channe");
			m3l_DestroyFound(&SFounds);
		}
	}
	else
	{
		n_data_threads = 0;
	}
/*
 * make buffer structure
 */
	if(  (BuffNode = m3l_Mklist("Buffer", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
		Perror("Allocate_DataBuffer: Mklist");

	if( (dim = (size_t *) malloc( 1* sizeof(lmsize_t))) == NULL)
		Error("Allocate_DataBuffer: Malloc error");
/*
 * move Channel lists to buffer 
 * and add Thread_Status integer
 */
	for(i=0; i < n_data_threads; i++){
		TmpNode = m3l_get_Found_node(SFounds, i);
		
		if( m3l_Mv(&TmpNode,  "./Channel", "./*", &BuffNode, "/Buffer", "/*", (lmchar_t *)NULL) == -1)
			Error("Allocate_DataBuffer: Mv");
		
		if( Additional_Data2Buffer(&TmpNode) != 1)
			Error("Allocate_DataBuffer: Additional_Data2Buffer");
	}

	free(dim);
	
	m3l_DestroyFound(&SFounds);
	return BuffNode;
}


lmint_t Additional_Data2Buffer(node_t **TmpNode){
/*
 * add additional data
 * Thread_Status 0 - thread is free
 *               1 - thread is occupied
 * S_Status      0 - S channel is free
 *               1 - S channel is occupied
 * R_Status      < number of allocated R_channels
 *                 channels are free
 *               == number of allocated R_channels - all R channels are occupied
 */
	lmsize_t dim[1];
	lmint_t *tmpint, retval;
	lmsize_t *tmpszt;
	node_t *TmpNode1;
	
	retval = 0;
	
	dim[0] = 1;
	if(  (TmpNode1 = m3l_Mklist("Thread_Status", "I", 1, dim, TmpNode, "./Channel", "./", (char *)NULL)) == 0)
		Error("Allocate_DataBuffer:m3l_Mklist");
	tmpint = (lmint_t *)m3l_get_data_pointer(TmpNode1);
	tmpint[0] = 0;
	if(  (TmpNode1 = m3l_Mklist("S_Status", "I", 1, dim, TmpNode, "./Channel", "./", (char *)NULL)) == 0)
		Error("Allocate_DataBuffer:m3l_Mklist");
	tmpint = (lmint_t *)m3l_get_data_pointer(TmpNode1);
	tmpint[0] = 0;
	if(  (TmpNode1 = m3l_Mklist("R_Status", "ST", 1, dim, TmpNode, "./Channel", "./", (char *)NULL)) == 0)
		Error("Allocate_DataBuffer:m3l_Mklist");
	tmpszt = (lmsize_t *)m3l_get_data_pointer(TmpNode1);
	tmpszt[0] = 0;
	
	retval = 1;
	return retval;
}