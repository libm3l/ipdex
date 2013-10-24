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
 *     Function socket_op.c
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
#include "ACK.h"
#include "socket_op.h"

lmint_t open_connection_to_server(const lmchar_t *hostname, lmint_t portno, client_fce_struct_t *ClientInPar, opts_t *Popts){

	lmint_t sockfd, retval;
	node_t *Gnode, *TmpNode;
	struct timespec tim, tim2;

	tim.tv_sec = 0;
	tim.tv_nsec = 100000000L;    /* 0.1 secs */

	if(hostname != NULL){
/*
 * create header which will identify name of data set and Sender (S) or Receiver (R)
 */
		Gnode = Header(ClientInPar->data_name, ClientInPar->SR_MODE);
again:
		if ( (sockfd =  m3l_cli_open_socket(hostname, portno, (lmchar_t *)NULL)) < 0)
			Error("open_connection_to_server: Could not open socket");
/*
 * send header identifying name which connection will be used. Upon receiving this info, 
 * server will send back the answer
 */
		if( (TmpNode = m3l_send_receive_tcpipsocket(Gnode, (lmchar_t *)NULL, sockfd, Popts)) == NULL){
			Perror("open_connection_to_server: m3l_send_receive_tcpipsocket error");

			if(m3l_Umount(&Gnode) != 1)
				Perror("open_connection_to_server: m3l_Umount");
			return -1;
		}
/*
 * get the value of the /RR/val
 * this is an answer from the server which stores the return value in 
 * ret_receipt == 0 all connections to the server specified for 
 * given data set were taken, retry opening it again after certain time
 */
		retval = TmpNode->child->data.i[0];
/*
 * if retval == 1 the data_thread is prepared to transmit the data, 
 * if retval == 0 the data_thread is busy, close socket and try again
 */		
		if(retval == 0){
		
			if(m3l_Umount(&TmpNode) != 1)
				Perror("m3l_Umount");

			if( close(sockfd) == -1)
				Perror("close");
				
			if(nanosleep(&tim , &tim2) < 0 )
				Error("Nano sleep system call failed \n");
			goto again;
		}

		if(m3l_Umount(&Gnode) != 1)
			Perror("m3l_Umount");
		if(m3l_Umount(&TmpNode) != 1)
			Perror("m3l_Umount");
	}
	else
		Error("Hostname not given");
		return -1;
	
	return sockfd;
}