#include "libm3l.h"
#include "Server_Header.h"
#include "ACK.h"
#include "socket_op.h"

lmint_t open_connection_to_server(lmchar_t *hostname, lmint_t portno, client_fce_struct_t *ClientInPar, opts_t *Popts){

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
			Error("Could not open socket");

// 		opts.opt_tcpencoding = 'I';    /*  "--encoding" , "IEEE-754"  */
	
// 		if(  (TmpNode = m3l_Send_receive_tcpipsocket(Gnode,(lmchar_t *)NULL, sockfd, "--encoding" , "IEEE-754", (lmchar_t *)NULL)) == NULL)
// 			Error("Receiving data");
/*
 * send header identifying name which connection will be used. Upon receiving this info, 
 * server will send back the answer
 */
		if( (TmpNode = m3l_send_receive_tcpipsocket(Gnode, (lmchar_t *)NULL, sockfd, Popts)) == NULL){
			Perror("m3l_send_receive_tcpipsocket error");

			if(m3l_Umount(&Gnode) != 1)
				Perror("m3l_Umount");
			return -1;
		}
/*
 * get the value of the /RR/val
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
	
	return sockfd;
}