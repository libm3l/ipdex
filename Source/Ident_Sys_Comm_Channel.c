

#include "libm3l.h"
#include "lsipdx_header.h"
#include "Ident_Sys_Comm_Channel.h"
// #include "Server_Functions_Prt.h"


lmint_t Ident_Sys_Comm_Channel(node_t *RecNode, node_t **DataBuffer, data_thread_str_t *Data_Threads, lmchar_t *name_of_required_data_set, lmchar_t *SR_mode)
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
	lmint_t retval, *reqtype, *tmpint;
	find_t *SFounds;
	node_t *List, *TmpNode1;
 	lmchar_t *tmpchar;
	lmsize_t len, i, dim[1];
	
	retval = 0;
/*
 * find /Header/Name_of_Channel in RecNode
 * if found, the request is normal.
 */
	if( (SFounds = m3l_Locate(RecNode, "/Header/Name_of_Channel", "./*/*",  (lmchar_t *)NULL)) != NULL){
		if( m3l_get_Found_number(SFounds) != 1)
			Error("Server_Body: Only one Name_of_Channel per Channel allowed");
		if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
			Error("Server_Body: NULL Name_of_Channel");
		tmpchar = m3l_get_data_pointer(List);
		
		if( (len = m3l_get_List_totdim(List)-1) < 1)
			Error("Data_Thread: too short name of data set");
		
		for(i=0; i<len; i++)
			*name_of_required_data_set++ = *tmpchar++;
			       
		*name_of_required_data_set='\0';		
/* 
 * free memory allocated in m3l_Locate
 */
		m3l_DestroyFound(&SFounds);
		
/*
 * find type of process SR_Mode  S-sender, R-receiver
 */
		if( (SFounds = m3l_Locate(RecNode, "/Header/SR_mode", "./*/*",  (lmchar_t *)NULL)) != NULL){
			
			if( m3l_get_Found_number(SFounds) != 1)
				Error("Server_Body: Only one SR_mode per Channel allowed");
/* 
 * pointer to list of found nodes
 */
				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
					Error("NULData_Thread: Missing S_mode");
			
				*SR_mode = *(lmchar_t *)m3l_get_data_pointer(List);
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			printf("Server_Body: Receiving_Processes not found\n");
		}
		
		
		
		
		return 0;
	}
	else
	{
/*
 * did not find the node, the channel is a request of a client to communicate 
 * with server, this request start with _comm_link_ check that it 
 * is really so by looking at name of head node which is _comm_link_
 */
		name_of_required_data_set=m3l_get_List_name(RecNode);
		if(strncmp(name_of_required_data_set, "_sys_comm_", 10) != 0
			&& strlen(name_of_required_data_set) == 10){
/*
 * illegal request, set return value -1 and return
 */
			return -1;
		}	
/*
 * find /_sys_comm/request_type
 * this is an integer and specify what kind of request is it 
 * if found, the request is a communication request of client with server
 * if not found, the request is a normal request of client asking to communicate with another client
 */
		if( (SFounds = m3l_Locate(RecNode, "/_comm_link_/request_type", "./*/*",  (lmchar_t *)NULL)) != NULL){
			if( m3l_get_Found_number(SFounds) != 1)
				Error("Server_Body: Only one /_comm_link_/request_type allowed");
			if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
				Error("Server_Body: NULL /_comm_link_/request_type");
			reqtype = (lmint_t *)m3l_get_data_pointer(List);
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			Error("Server_Body: /_comm_link_/request_type not found\n");
		}
		
		switch(*reqtype){

			case 1:
/*
 * open new connection - find its specification under "/_comm_link_/Channel
 */
				if( (SFounds = m3l_Locate(RecNode, "/_comm_link_/Channel", "./*/*",  (lmchar_t *)NULL)) != NULL){
					if( m3l_get_Found_number(SFounds) != 1)
						Error("Server_Body: Only one /_comm_link_/Channel allowed");
					if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
						Error("Server_Body: NULL /_comm_link_/Channel");
					m3l_DestroyFound(&SFounds);
				}
				else
				{
					Error("Server_Body: /_comm_link_/Channel not found\n");
				}
/*
 * add it to the buffer and add to it other data set
 * similarly to Allocate_DataBuffer.c
 */
				m3l_Mv(&List,  "./Channel", "./*", DataBuffer, "/Buffer", "/*", (lmchar_t *)NULL);
				
				dim[0] = 1;
				if(  (TmpNode1 = m3l_Mklist("Thread_Status", "I", 1, dim, &List, "./Channel", "./", (char *)NULL)) == 0)
					Error("m3l_Mklist");
				tmpint = (lmint_t *)m3l_get_data_pointer(TmpNode1);
				tmpint[0] = 0;
				if(  (TmpNode1 = m3l_Mklist("S_Status", "I", 1, dim, &List, "./Channel", "./", (char *)NULL)) == 0)
					Error("m3l_Mklist");
				tmpint = (lmint_t *)m3l_get_data_pointer(TmpNode1);
				tmpint[0] = 0;
				if(  (TmpNode1 = m3l_Mklist("R_Status", "ST", 1, dim, &List, "./Channel", "./", (char *)NULL)) == 0)
					Error("m3l_Mklist");
				tmpint = (lmint_t *)m3l_get_data_pointer(TmpNode1);
				tmpint[0] = 0;
				
			break;

			case 2:
/*
 * close connection
 */
			
			break;
		}




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

	return retval;
}