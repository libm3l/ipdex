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
 *     Function Ident_Sys_Comm_Channel.c
 *
 *     Date: 2014-03-07
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
#include "Ident_Sys_Comm_Channel.h"
#include "Server_Functions_Prt.h"
#include "Check_Request.h"

lmint_t Ident_Sys_Comm_Channel(node_t *RecNode, node_t **DataBuffer, data_thread_str_t *Data_Threads, 
	     lmchar_t *name_of_required_data_set, lmchar_t *SR_mode, lsipdx_answer_t *Answers, lmint_t sockfd)
{
/*
 * function handles communication between client and server via _sys_comm_ request.
 * This channel is used to process requests sent by clients to the server.
 * Some of the stask managen through this channel are
 * 	-	add/remove data channel
 * 	-	leave Data_Thread (SR_Hub) for KA = Y
 * 	-	increase/decrease number of S_R threads for a particular Data_Thread
 * 	-	change KA and/or ATDRT mode
 * 
 *	return value
 * 		-1	Error
 * 		 0	Usual request for connection between clients
 * 		 100	Add new connection (channel)
 * 		 200	Delete existing connection
 */
	lmint_t retval, *reqtype;
	find_t *SFounds;
	node_t *List;
	lmchar_t *tmpchar, *req_name, *ATDT_Mode, *KA_mode, *tmpname;
	lmsize_t len, i, nRcli;
	opts_t *Popts, opts, *Popts_tcp, opts_tcp;

	Popts = &opts;	
	m3l_set_Find(&Popts);
	
	Popts_tcp = &opts_tcp;	
	m3l_set_Send_receive_tcpipsocket(&Popts_tcp);

	retval = 0;
	
	tmpname = name_of_required_data_set;
/*
 * find /Header/Name_of_Channel in RecNode
 * if found, the request is normal.
 */
	req_name = m3l_get_List_name(RecNode);
	if( (len = strlen(req_name)) < 1)
		Error("Ident_Sys_Comm_Channel: too short name of data set");
	
	if( len == 6 && strncmp(req_name, "Header", 6) == 0){
/*
 * usual data request
 */
		if( (SFounds = m3l_locator_caller(RecNode, "/Header/Name_of_Channel", "./*/*",  Popts)) != NULL){
			if( m3l_get_Found_number(SFounds) != 1)
				Error("Ident_Sys_Comm_Channel: Only one Name_of_Channel per Channel allowed");
			if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
				Error("Ident_Sys_Comm_Channel: NULL Name_of_Channel");
			tmpchar = m3l_get_data_pointer(List);
			
			if( (len = m3l_get_List_totdim(List)-1) < 1)
				Error("Ident_Sys_Comm_Channel: too short name of data set");
			
			for(i=0; i<len; i++)
				*tmpname++ = *tmpchar++;
			*tmpname='\0';
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
/*
 * find type of process SR_Mode  S-sender, R-receiver
 */
			if( (SFounds = m3l_locator_caller(RecNode, "/Header/SR_mode", "./*/*",  Popts)) != NULL){
				
				if( m3l_get_Found_number(SFounds) != 1)
					Error("Ident_Sys_Comm_Channel: Only one SR_mode per Channel allowed");
/* 
 * pointer to list of found nodes
 */
				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
					Error("Ident_Sys_Comm_Channel: Missing S_mode");
			
				*SR_mode = *(lmchar_t *)m3l_get_data_pointer(List);
/* 
 * free memory allocated in m3l_Locate
 */
				m3l_DestroyFound(&SFounds);
			}
			else
			{
				printf("Ident_Sys_Comm_Channel: Receiving_Processes not found\n");
				m3l_DestroyFound(&SFounds);
			}
			
			retval = 0;
// 			
			retval = Check_Request( (*DataBuffer), name_of_required_data_set, *SR_mode);
// 			printf(" RETVAL is %d \n", retval);
			*Data_Threads->checkdata = 0;
			return retval;
		}
		else{
/*
 * 	did not find /Header/Name_of_Channel
 */
			printf("Ident_Sys_Comm_Channel: /Header/Name_of_Channel not found\n");
			m3l_DestroyFound(&SFounds);
		}
	}
	else{
/*
 * did not find the node, the channel is a request of a client to communicate 
 * with server, this request start with _comm_link_ check that it 
 * is really so by looking at name of head node which is _comm_link_
 */
		*Data_Threads->checkdata = 1;

		name_of_required_data_set=m3l_get_List_name(RecNode);
		if(strncmp(name_of_required_data_set, "_sys_comm_", 10) != 0 ||  strlen(name_of_required_data_set) != 10){
			opts_tcp.opt_EOBseq = '\0'; // send EOFbuff sequence only
			if( m3l_send_to_tcpipsocket(Answers->RR_WRREQ, (const char *)NULL, sockfd, Popts_tcp) < 1)
				Error("Ident_Sys_Comm_Channel: Error during sending RR_WRCONREQ to sockfd");
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
// 		if( (SFounds = m3l_Locate(RecNode, "/_comm_link_/request_type", "./*/*",  (lmchar_t *)NULL)) != NULL){
		if( (SFounds = m3l_locator_caller(RecNode, "/_comm_link_/request_type", "./*/*",  Popts)) != NULL){
			if( m3l_get_Found_number(SFounds) != 1)
				Error("Ident_Sys_Comm_Channel: Only one /_comm_link_/request_type allowed");
			if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
				Error("Ident_Sys_Comm_Channel: NULL /_comm_link_/request_type");
			reqtype = (lmint_t *)m3l_get_data_pointer(List);
/* 
 * free memory allocated in m3l_Locate
 */
			m3l_DestroyFound(&SFounds);
		}
		else
		{
			Error("Ident_Sys_Comm_Channel: /_comm_link_/request_type not found\n");
		}
		
		switch(*reqtype){

			case 100:
/*
 * add new connection
 */
/*
 * find /_sys_link_/Name_of_Channel in RecNode
 */
				if( (SFounds = m3l_locator_caller(RecNode, "/_sys_link_/Name_of_Channel", "./*/*",  Popts)) != NULL){
					if( m3l_get_Found_number(SFounds) != 1)
						Error("Ident_Sys_Comm_Channel: Only one Name_of_Channel per Channel allowed");
					if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
						Error("Ident_Sys_Comm_Channel: NULL Name_of_Channel");
					name_of_required_data_set = m3l_get_data_pointer(List);
					
					if( (len = m3l_get_List_totdim(List)-1) < 1)
						Error("Ident_Sys_Comm_Channel: too short name of data set");
/* 
 * free memory allocated in m3l_Locate
 */
					m3l_DestroyFound(&SFounds);
				}
/*
 * check if connection is already available
 */
// 				for(i=0, i< Queue->n_conn; i++){
// 					len1 = strlen(Queue->name_of_connection);
// 					if( len == len1 && strncmp(Queue->name_of_connection, name_of_required_data_set,len) == 0){
// /*
//  * connection alraedy exists, send WRCONREQ
//  */
// 						opts_tcp.opt_EOBseq = '\0'; // send EOFbuff sequence only
// 						if( m3l_send_to_tcpipsocket(Answers->RR_WRREQ, (const char *)NULL, sockfd, Popts_Send) < 1)
// 							Error("Ident_Sys_Comm_Channel: Error during sending RR_WRCONREQ to sockfd");
// 						return -1;
// 					}
// 				}
/*
 * find SR mode, KA_mode, Receiving clients number
 */
				if( (SFounds = m3l_locator_caller(RecNode, "/_sys_link_/CONNECTION/ATDT_Mode", "./*/*/*",  Popts)) != NULL){

					if( m3l_get_Found_number(SFounds) != 1)
						Error("Ident_Sys_Comm_Channel: Only one CONNECTION/ATDT_Mode per Channel allowed");
/* 
 * pointer to list of found nodes
 */
						if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
							Error("Ident_Sys_Comm_Channel: NULL ATDT_Mode");
					
						ATDT_Mode = (lmchar_t *)m3l_get_data_pointer(List);
/* 
 * free memory allocated in m3l_Locate
 */
					m3l_DestroyFound(&SFounds);
				}
				else
				{
					Error("SR_hub: CONNECTION/ATDT_Mode not found\n");
				}

				if( (SFounds = m3l_locator_caller(RecNode, "/_sys_link_/CONNECTION/KEEP_CONN_ALIVE_Mode", "./*/*/*",  Popts)) != NULL){

					if( m3l_get_Found_number(SFounds) != 1){
						Warning("Ident_Sys_Comm_Channel: Only one CONNECTION/KEEP_CONN_ALIVE_Mode per Channel allowed");
						opts_tcp.opt_EOBseq = '\0'; // send EOFbuff sequence only
						if( m3l_send_to_tcpipsocket(Answers->RR_POS, (const char *)NULL, sockfd, Popts_tcp) < 1)
							Error("Error during sending data to sockfd");
						if( close(sockfd) == -1)
							Perror("close");
						m3l_DestroyFound(&SFounds);
					}
/* 
 * pointer to list of found nodes
 */
					if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
						Error("Ident_Sys_Comm_Channel: NULL KEEP_CONN_ALIVE_Mode");
					
					KA_mode = (lmchar_t *)m3l_get_data_pointer(List);
/* 
 * free memory allocated in m3l_Locate
 */
					m3l_DestroyFound(&SFounds);
				}
				else
				{
					Warning("Ident_Sys_Comm_Channel: CONNECTION/KEEP_CONN_ALIVE_Mode not found\n");
					opts_tcp.opt_EOBseq = '\0'; // send EOFbuff sequence only
					if( m3l_send_to_tcpipsocket(Answers->RR_POS, (const char *)NULL, sockfd, Popts_tcp) < 1)
						Error("Error during sending data to sockfd");
					if( close(sockfd) == -1)
						Perror("close");
					m3l_DestroyFound(&SFounds);
				}


				if( (SFounds = m3l_locator_caller(RecNode, "/_sys_link_/Receiving_Processes", "./*/*",  Popts)) != NULL){

					if( m3l_get_Found_number(SFounds) != 1){
						Warning("Ident_Sys_Comm_Channel: Only one Receiving_Processes per Channel allowed");
						opts_tcp.opt_EOBseq = '\0'; // send EOFbuff sequence only
						if( m3l_send_to_tcpipsocket(Answers->RR_POS, (const char *)NULL, sockfd, Popts_tcp) < 1)
							Error("Error during sending data to sockfd");
						if( close(sockfd) == -1)
							Perror("close");
						m3l_DestroyFound(&SFounds);
					}
/* 
 * pointer to list of found nodes
 */
						if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
							Error("Ident_Sys_Comm_Channel: NULL Receiving_Processes");
					
						nRcli = *(lmsize_t *)m3l_get_data_pointer(List);
/* 
 * free memory allocated in m3l_Locate
 */
					m3l_DestroyFound(&SFounds);
				}
				else
				{
					Warning("Ident_Sys_Comm_Channel: Receiving_Processes not found\n");
					opts_tcp.opt_EOBseq = '\0'; // send EOFbuff sequence only
					if( m3l_send_to_tcpipsocket(Answers->RR_POS, (const char *)NULL, sockfd, Popts_tcp) < 1)
						Error("Error during sending data to sockfd");
					if( close(sockfd) == -1)
						Perror("close");
					m3l_DestroyFound(&SFounds);
					return -1;
				}
				
// 		!!!!!		NOTE: Start SR_hub and SR_Data_Threads   !!!!!!
					
				retval = 100;
			break;

			case 200:
/*
 * close connection, find the name of closed connection
 */
// 				if( (SFounds = m3l_Locate(RecNode, "/_comm_link_/Name_of_Channel", "./*/*",  (lmchar_t *)NULL)) != NULL){
				if( (SFounds = m3l_locator_caller(RecNode, "/_comm_link_/Name_of_Channel", "./*/*",  Popts)) != NULL){
					if( m3l_get_Found_number(SFounds) != 1)
						Error("Ident_Sys_Comm_Channel: Only one Name_of_Channel per Channel allowed");
					if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
						Error("Ident_Sys_Comm_Channel: NULL Name_of_Channel");
					tmpchar = m3l_get_data_pointer(List);
					
					if( (len = m3l_get_List_totdim(List)-1) < 1)
						Error("Ident_Sys_Comm_Channel: too short name of data set");
					
					for(i=0; i<len; i++)
						*name_of_required_data_set++ = *tmpchar++;
				
					*name_of_required_data_set='\0';
					
					m3l_DestroyFound(&SFounds);

					retval = 101;
				}
				else
					m3l_DestroyFound(&SFounds);
					retval = -1;
			break;
		}
	}
	return retval;
}