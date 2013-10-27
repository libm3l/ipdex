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
 *     Function client_functions_Prt.c
 *
 *     Date: 2013-10-20
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
#include "Server_Header.h"
#include "client_functions_Prt.h"
#include "ACK.h"
#include "socket_op.h"

//      mode 1: ATDTMode == 'D' && KeepAllive_Mode == 'N'  /* Direct transfer, close socket */
//      mode 2: ATDTMode == 'A' && KeepAllive_Mode == 'N'  /* Alternate transfer, close socket */
//      mode 3: ATDTMode == 'D' && KeepAllive_Mode == 'C'  /* Direct transfer, close socket after client request it*/
//      mode 4: ATDTMode == 'A' && KeepAllive_Mode == 'C'  /* Alternate transfer, close socket after client request it*/
//      mode 5: ATDTMode == 'D' && KeepAllive_Mode == 'Y'  /* Direct transfer, do not close socket*/
//      mode 6: ATDTMode == 'A' && KeepAllive_Mode == 'Y'  /* Alternate transfer, do not close socket*/

lmint_t client_sender(void *data, const lmchar_t *hostname, lmint_t portno, client_fce_struct_t *ClientInPar, opts_t *Popts, opts_t *Popst_lm3l){

	node_t *Gnode, *TmpNode;
	lmint_t sockfd, retval;
	
	opts_t *Popts_1, opts;

// 	opts.opt_linkscleanemptlinks = '\0';  // clean empty links
// 	opts.opt_nomalloc = '\0'; // if 'm', do not malloc (used in Mklist --no_malloc
// 	opts.opt_linkscleanemptrefs = '\0'; // clean empty link references
// 	opts.opt_tcpencoding = 'I'; // serialization and encoding when sending over TCP/IP
// 	opts.opt_shutdown = '\0'; // shutdown when finished with sending
// 	opts.opt_MEMCP = 'S';  // type of buffering
// 	opts.opt_EOBseq = '\0'; // send EOFbuff sequence only
// 	opts.opt_REOBseq = '\0'; // read EOFbuff sequence only

	Popts_1 = &opts;
	m3l_set_Send_receive_tcpipsocket(&Popts_1);
/* 
 * if required, open socket 
 */
	if(hostname != NULL){
		if( (sockfd = open_connection_to_server(hostname, portno, ClientInPar, Popts_1)) < 1)
			Error("client_sender: Error when opening socket");
	}
	else
		sockfd = portno;
	
	switch(ClientInPar->mode){
	case 1:
/*
 * send data and receive confirmation that the data were received (--REOB)
 */
// 		m3l_Send_receive_tcpipsocket((node_t *)data,(char *)NULL, sockfd, "--encoding" , "IEEE-754",  "--REOB", (char *)NULL);

		opts.opt_REOBseq 	= 'G';  /* --REOB */
		m3l_send_receive_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);
		
		if(hostname != NULL){
			if( close(sockfd) == -1)
				Perror("close");
		}
	
	break;
		
	case 2:
/*
 * send data and receive confirmation that the data were received (--REOB)
 */
// 		m3l_Send_to_tcpipsocket((node_t *)data,(char *)NULL, sockfd, "--encoding" , "IEEE-754",  "--REOB", (char *)NULL);
		opts.opt_EOBseq 	= '\0';  /* --REOB */
		m3l_send_to_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);
/* 
 *  Only Receiver closes socket, before that wait for client confirming all data were sent
 */
		if(ClientInPar->SR_MODE == 'R'){
			
			opts.opt_REOBseq = 'G';  /* --REOB */
			m3l_receive_tcpipsocket((lmchar_t *)NULL, sockfd, Popts_1);
			opts.opt_REOBseq = '\0';  /* --REOB */			
			
			if( close(sockfd) == -1)
				Perror("close");
		}

	break;

	case 3:
		
		opts.opt_REOBseq     = 'G';  /* --REOB */
		m3l_send_receive_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);

// 		m3l_send_receive_tcpipsocket((node_t *)END_OK, (lmchar_t *)NULL, sockfd, Popts_1);
		
// 		if(hostname != NULL && END_OK == OK){
// 			if( close(sockfd) == -1)
// 				Perror("close");
// 		}
		
	break;

	case 4:
/*
 * send data and receive REOB
 */	
		opts.opt_REOBseq 	= 'G';  /* --REOB */
		m3l_send_receive_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);
// 		m3l_send_to_tcpipsocket((node_t *)END_OK, (lmchar_t *)NULL, sockfd, Popts_1);
		
	break;

	case 5:
/*
 * send data and receive confirmation that the data were received (--REOB)
 */
// 		m3l_Send_receive_tcpipsocket((node_t *)data,(char *)NULL, sockfd, "--encoding" , "IEEE-754",  "--REOB", (char *)NULL);

		opts.opt_REOBseq 	= 'G';  /* --REOB */
		m3l_send_receive_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);
	
	break;

	case 6:
/*
 * send data and receive confirmation that the data were received (--REOB)
 */
// 		m3l_Send_to_tcpipsocket((node_t *)data,(char *)NULL, sockfd, "--encoding" , "IEEE-754",  "--REOB", (char *)NULL);
		m3l_send_to_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);
		
		/* maybe adding REOB */
		
	break;
	
	default:
		Error("client_sender: Wrong mode - check KeepAlive and ATDT specifications");
	break;
	}

	return sockfd;
}


client_recevier_struct_t *client_recevier(const lmchar_t *hostname, lmint_t portno, client_fce_struct_t *ClientInPar, opts_t *Popts, opts_t *Popst_lm3l){

	node_t *Gnode, *TmpNode;
	lmint_t sockfd, retval;
	
	opts_t *Popts_1, opts;
	
	client_recevier_struct_t *Pretval;

// 	opts.opt_linkscleanemptlinks = '\0';  // clean empty links
// 	opts.opt_nomalloc = '\0'; // if 'm', do not malloc (used in Mklist --no_malloc
// 	opts.opt_linkscleanemptrefs = '\0'; // clean empty link references
// 	opts.opt_tcpencoding = 'I'; // serialization and encoding when sending over TCP/IP
// 	opts.opt_shutdown = '\0'; // shutdown when finished with sending
// 	opts.opt_MEMCP = 'S';  // type of buffering
// 	opts.opt_EOBseq = '\0'; // send EOFbuff sequence only
// 	opts.opt_REOBseq = '\0'; // read EOFbuff sequence only
	
	Popts_1 = &opts;
	m3l_set_Send_receive_tcpipsocket(&Popts_1);
	
	if ( (Pretval = (client_recevier_struct_t *)malloc(sizeof(client_recevier_struct_t))) == NULL)
		Error("client_recevier: allocating Pretval failed ");
/* 
 * if required, open socket 
 */
	if(hostname != NULL){
		if( (sockfd = open_connection_to_server(hostname, portno, ClientInPar, Popts_1)) < 1 )
			Error("client_recevier: Error when opening socket");
	}
	else
		sockfd = portno;
/*
 * confirm the header was received (--SEOB)
 */
	opts.opt_EOBseq 	= 'E';       /* --SEOB */
// 	m3l_Send_to_tcpipsocket((node_t *)NULL, (char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB", (char *)NULL);
	m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);
	
	
	switch(ClientInPar->mode){
	case 1:
/*
 * receive payload
 */
// 	if( (Gnode = m3l_Receive_tcpipsocket((char *)NULL, sockfd, "--encoding" , "IEEE-754", (char *)NULL)) == NULL)
// 		Error("Receiving data");
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("Receiving data");
/*
 * confirm the data was received (--SEOB)
 */
// 	m3l_Send_to_tcpipsocket(NULL,(char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB", (char *)NULL);
		opts.opt_EOBseq 	= 'E';   /* --SEOB */
		m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);
		
		if(hostname != NULL){
			if( close(sockfd) == -1)
				Perror("close");
		}
	break;
	
	case 2:
		opts.opt_REOBseq     = '\0';  /* --REOB */
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("Receiving data");
/*
 * Sender closes socket, before that it conforms all data were transferred
 */
		if(ClientInPar->SR_MODE == 'S'){  /* Sender closes socket */
			
			opts.opt_EOBseq  = 'E';       /* --SEOB */
			m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);
			opts.opt_EOBseq = '\0';       /* --SEOB */
			
			if( close(sockfd) == -1)
				Perror("close");
		}
	break;

	case 3:
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("Receiving data");
// 		m3l_send_to_tcpipsocket((node_t *)END_OK, (lmchar_t *)NULL, sockfd, Popts_1);
		
// 		if(hostname != NULL && Caller == 'S'){  /* Sender closes socket */
// 			if( close(sockfd) == -1)
// 				Perror("close");
// 		}		
	break;
	

	case 4:
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("Receiving data");
		opts.opt_REOBseq 	= 'G';  /* --REOB */
// 		m3l_send_receive_tcpipsocket((node_t *)END_OK, (lmchar_t *)NULL, sockfd, Popts_1);
		
	break;

	case 5:
/*
 * receive payload
 */
// 	if( (Gnode = m3l_Receive_tcpipsocket((char *)NULL, sockfd, "--encoding" , "IEEE-754", (char *)NULL)) == NULL)
// 		Error("Receiving data");

		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("Receiving data");
/*
 * confirm the data was received (--SEOB)
 */
// 	m3l_Send_to_tcpipsocket(NULL,(char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB", (char *)NULL);

		opts.opt_EOBseq = 'E';   /* --SEOB */
		m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);

	break;
	
	case 6:
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL){
			if(m3l_Umount(&Gnode) != 1)
				Perror("m3l_Umount");
			Error("Receiving data");
		}
		/* maybe adding SEOB */

	break;
	
	default:
		Error("client_sender: Wrong mode - check KeepAlive and ATDT specifications");
	break;
	}

// 	return (void *)Gnode;
	Pretval->data = Gnode;
	return Pretval;
}

