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
#include "lsipdx_header.h"
#include "client_functions_Prt.h"
#include "ACK.h"
#include "socket_op.h"

//      mode 1: ATDTMode == 'D' && KeepAlive_Mode == 'N'  /* Direct transfer, close socket */
//      mode 2: ATDTMode == 'A' && KeepAlive_Mode == 'N'  /* Alternate transfer, close socket */
//      mode 3: ATDTMode == 'D' && KeepAlive_Mode == 'C'  /* Direct transfer, close socket after client request it*/
//      mode 4: ATDTMode == 'A' && KeepAlive_Mode == 'C'  /* Alternate transfer, close socket after client request it*/
//      mode 5: ATDTMode == 'D' && KeepAlive_Mode == 'Y'  /* Direct transfer, do not close socket*/
//      mode 6: ATDTMode == 'A' && KeepAlive_Mode == 'Y'  /* Alternate transfer, do not close socket*/
/*
 * opt_EOBseq and opt_REOBseq are used to specify that 
 * the only thing which is sent/receive through channel 
 * is EOB sequence
 * It is used if multiple libm3l list are to be sequentially sent by one process and 
 * received by another process. To notify the processes that one libm3l list was sent, the 
 * receiving process, after receving sequence sends EOB EOB and sending process will receive it
 * The only porpose is to interrupt the flow of bytes from one libm3l node
 * before the next byte libm3l sequence is to be sent
 */

lmint_t client_sender(void *data, lmint_t sockfd, client_fce_struct_t *ClientInPar, opts_t *Popts, opts_t *Popst_lm3l){
	
	opts_t *Popts_1, opts;

	Popts_1 = &opts;
	m3l_set_Send_receive_tcpipsocket(&Popts_1);

	switch(ClientInPar->mode){
	case 1:
/*
 * send data and receive confirmation that the data were received (--REOB)
 */
// 		m3l_Send_receive_tcpipsocket((node_t *)data,(char *)NULL, sockfd, "--encoding" , "IEEE-754",  "--REOB", (char *)NULL);

		opts.opt_REOBseq 	= 'G';  /* --REOB */
		m3l_send_receive_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);
	
	break;
		
	case 2:
/*
 * send data and receive confirmation that the data were received (--REOB)
 */
// 		m3l_Send_to_tcpipsocket((node_t *)data,(char *)NULL, sockfd, "--encoding" , "IEEE-754",  "--REOB", (char *)NULL);
		opts.opt_EOBseq 	= '\0';  /* --REOB */
		m3l_send_to_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);
/* 
 * if sender behaves a sender (ATDT = A mode), Receiver wait for client confirming all data were sent
 * opt_REOBseq serves as a termination request when process is just sending data, when specifying this
 * the sender will send EOB sequence and receiver will receive EOB sequency only
 */
		if(ClientInPar->SR_MODE == 'R'){
			opts.opt_REOBseq = 'G';  /* --REOB */
			m3l_receive_tcpipsocket((lmchar_t *)NULL, sockfd, Popts_1);
			opts.opt_REOBseq = '\0';  /* --REOB */			
		}
	break;

	case 5:
/*
 * send data and receive confirmation that the data were received (--REOB)
 */
		opts.opt_REOBseq 	= 'G';  /* --REOB */
		m3l_send_receive_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);
	
	break;

	case 6:
/*
 * send data and do not wait for confirmation that the data were received (--REOB)
 */
		opts.opt_EOBseq 	= '\0';  /* --REOB */
		m3l_send_to_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);

	break;
	
	default:
		Error("client_sender: Wrong mode - check KeepAlive and ATDT specifications");
	break;
	}

	return 1;
}


node_t *client_receiver(lmint_t sockfd, client_fce_struct_t *ClientInPar, opts_t *Popts, opts_t *Popst_lm3l){

	node_t *Gnode;
	
	opts_t *Popts_1, opts;
	
	Popts_1 = &opts;
	m3l_set_Send_receive_tcpipsocket(&Popts_1);
	
	Gnode = NULL;
	
	switch(ClientInPar->mode){
	case 1:
/*
 * receive payload
 */
	if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
		Error("client_receiver: Receiving data");
/*
 * confirm the data was received (--SEOB)
 */
// 	m3l_Send_to_tcpipsocket(NULL,(char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB", (char *)NULL);
		opts.opt_EOBseq 	= 'E';   /* --SEOB */
		m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);
	break;
	
	case 2:
		opts.opt_REOBseq     = '\0';  /* --REOB */
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("client_receiver: Receiving data");
/*
 * if receiver behaves a sender (ATDT = A mode), Sender confirms all data were transferred
 */
		if(ClientInPar->SR_MODE == 'S'){  
			opts.opt_EOBseq  = 'E';       /* --SEOB */
			m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);
			opts.opt_EOBseq = '\0';       /* --SEOB */
		}
	break;

	case 5:
/*
 * receive payload
 */

		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("client_receiver: Receiving data");
/*
 * confirm the data was received (--SEOB)
 */
		opts.opt_EOBseq = 'E';   /* --SEOB */
		m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);

	break;
	
	case 6:
/*
 * do not wait for confirmation that data were received
 */
		opts.opt_REOBseq     = '\0';  /* --REOB */
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("client_receiver: Receiving data");

	break;
	
	default:
		Error("client_receiver: Wrong mode - check KeepAlive and ATDT specifications");
	break;
	}

	return Gnode;
}
