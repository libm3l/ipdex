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
#include "ipdex_header.h"
#include "Allocate_DataBuffer.h"

node_t *Allocate_DataBuffer(node_t *Gnode){
/*
 * this routine allocates a structure containign 
 * information about transfered data sets and number of threads 
 * each data set is going to be accessed by
 */
	lmsize_t i, n_data_threads, *dim;
	find_t *SFounds=NULL;
	node_t *BuffNode=NULL, *TmpNode=NULL, *TmpNode1=NULL;
	lmint_t *tmpint;
	
	if(Gnode == NULL){
		Warning("Data_Thread: NULL Gnode");
		return NULL;
	}
/*
 * find how many data sets - defines how many data_threads will be required
 */
	if( (SFounds = m3l_Locate(Gnode, "/COMM_DEF/Data_Sets/Data_Set", "/*/*/*", (lmchar_t *)NULL)) != NULL){
		
		n_data_threads = m3l_get_Found_number(SFounds);
		
		if(n_data_threads == 0){
			Error("Server: did not find any Data_set");
			m3l_DestroyFound(&SFounds);
		}
	}
	else
	{
		printf("Server: did not find any Data_set\n");
		exit(0);
	}
/*
 * make buffer structure
 */
	if(  (BuffNode = m3l_Mklist("Buffer", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
		Perror("m3l_Mklist");

	dim = (size_t *) malloc( 1* sizeof(lmsize_t));
	dim[0] = 1;

	if(  (TmpNode = m3l_Mklist("N_data_sets", "ST", 1, dim, &BuffNode, "/Buffer", "./", (char *)NULL)) == 0)
		Error("m3l_Mklist");
	TmpNode->data.st[0] = n_data_threads;
/*
 * move Data_Set lists to buffer 
 * and add Thread_Status integer
 */
	for(i=0; i < n_data_threads; i++){
		TmpNode = m3l_get_Found_node(SFounds, i);
		m3l_Mv(&TmpNode,  "./Data_Set", "./*", &BuffNode, "/Buffer", "/*", (lmchar_t *)NULL);
		
		dim[0] = 1;
		if(  (TmpNode1 = m3l_Mklist("Thread_Status", "I", 1, dim, &TmpNode, "./Data_Set", "./", (char *)NULL)) == 0)
			Error("m3l_Mklist");
		tmpint = (lmint_t *)m3l_get_data_pointer(TmpNode1);
		tmpint[0] = 0;
		if(  (TmpNode1 = m3l_Mklist("S_Status", "I", 1, dim, &TmpNode, "./Data_Set", "./", (char *)NULL)) == 0)
			Error("m3l_Mklist");
		tmpint = (lmint_t *)m3l_get_data_pointer(TmpNode1);
		tmpint[0] = 0;
		if(  (TmpNode1 = m3l_Mklist("R_Status", "I", 1, dim, &TmpNode, "./Data_Set", "./", (char *)NULL)) == 0)
			Error("m3l_Mklist");
		tmpint = (lmint_t *)m3l_get_data_pointer(TmpNode1);
		tmpint[0] = 0;	}
	
// 	if(m3l_Cat(BuffNode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
// 		Error("CatData");


	free(dim);
	
	m3l_DestroyFound(&SFounds);
	return BuffNode;
}