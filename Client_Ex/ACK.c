/*
 *     Copyright (C) 2012  Adam Jirasek
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *     
 *     contact: libm3l@gmail.com
 * 
 */



/*
 *     Function ACK.c
 *
 *     Date: 2013-02-23
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
#include "ACK.h"


node_t *ackn(void)
{
	node_t *Gnode, *TmpNode;
	char *answer="ACKN";
	size_t *dim;
	
	if(  (Gnode = m3l_Mklist("Answer", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
		Perror("m3l_Mklist");
	
	dim = (size_t *) malloc( 1* sizeof(size_t));
	dim[0] = strlen(answer)+1;
	
	if(  (TmpNode = m3l_Mklist("ANSWER", "C", 1, dim, &Gnode, "/Answer", "./", "--no_malloc", (char *)NULL)) == 0)
		Error("m3l_Mklist");
	TmpNode->data.c = answer;
	
	free(dim);
	
 	return Gnode;
}

node_t *client_name(char *name)
{
	node_t *Gnode, *TmpNode;
	char *answer="ACKN";
	size_t *dim;
	
	if(  (Gnode = m3l_Mklist("Client_Data", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
		Perror("m3l_Mklist");
	
	dim = (size_t *) malloc( 1* sizeof(size_t));
	dim[0] = strlen(name)+1;
	
	if(  (TmpNode = m3l_Mklist("Name", "C", 1, dim, &Gnode, "/Client_Data", "./", "--no_malloc", (char *)NULL)) == 0)
		Error("m3l_Mklist");
	TmpNode->data.c = name;
	
	free(dim);
	
 	return Gnode;
}


node_t *Header(char *name, char RWmode){

	node_t *Gnode, *TmpNode;
	lmsize_t *dim;

	if(  (Gnode = m3l_Mklist("Header", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
		Perror("m3l_Mklist");
	
	dim = (size_t *) malloc( 1* sizeof(size_t));
	dim[0] = strlen(name)+1;
	
	if(  (TmpNode = m3l_Mklist("Name_of_Data_Set", "C", 1, dim, &Gnode, "/Header", "./", "--no_malloc", (char *)NULL)) == 0)
		Error("m3l_Mklist");
	TmpNode->data.c = name;
	
	
	dim[0] = 2;
	
	if(  (TmpNode = m3l_Mklist("SR_mode", "C", 1, dim, &Gnode, "/Header", "./", (char *)NULL)) == 0)
		Error("m3l_Mklist");
	TmpNode->data.c[0] = RWmode;
	TmpNode->data.c[1] = '\0';
	free(dim);
	
	return Gnode;
}