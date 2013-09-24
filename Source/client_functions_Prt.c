#include "libm3l.h"
#include "client_functions_Prt.h"
#include "ACK.h"


lmint_t client_sender(void *data, const lmchar_t *hostname, lmint_t portno, lmchar_t *data_name, lmchar_t SR_MODE, opts_t *Popts, opts_t *Popst_lm3l){

	node_t *Gnode, *TmpNode;
	lmint_t sockfd, retval;
	
	opts_t *Popts_1, opts;
	
	struct timespec tim, tim2;
	tim.tv_sec = 0;
	tim.tv_nsec = 100000000L;    /* 0.1 secs */

	opts.opt_linkscleanemptlinks = '\0';  // clean empty links
	opts.opt_nomalloc = '\0'; // if 'm', do not malloc (used in Mklist --no_malloc
	opts.opt_linkscleanemptrefs = '\0'; // clean empty link references
	opts.opt_tcpencoding = 't'; // serialization and encoding when sending over TCP/IP
	opts.opt_shutdown = '\0'; // shutdown when finished with sending
	opts.opt_MEMCP = 'S';  // type of buffering
	opts.opt_EOBseq = '\0'; // send EOFbuff sequence only
	opts.opt_REOBseq = '\0'; // read EOFbuff sequence only
	
	
	Popts_1 = &opts;
/*
 * create header which will identify name of data set and Sender (S) or Receiver (R)
 */
// 		Gnode = Header("Pressure", 'S');
		Gnode = Header(data_name, SR_MODE);
/* 
 * if required, open socket 
 */
again:
		if ( (sockfd =  m3l_cli_open_socket(hostname, portno, (lmchar_t *)NULL)) < 0)
			Error("Could not open socket");
		
// 		if(  (TmpNode = m3l_Send_receive_tcpipsocket(Gnode,(lmchar_t *)NULL, sockfd, "--encoding" , "IEEE-754", (lmchar_t *)NULL)) == NULL)
// 			Error("Receiving data");
		
		
		if( (TmpNode = m3l_send_receive_tcpipsocket(Gnode, (lmchar_t *)NULL, sockfd, Popts_1)) == NULL){
			Perror("m3l_send_receive_tcpipsocket error");
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
/*
 * send data and receive confirmation that the data were received (--REOB)
 */
// 		m3l_Send_receive_tcpipsocket((node_t *)data,(char *)NULL, sockfd, "--encoding" , "IEEE-754",  "--REOB", (char *)NULL);

		opts.opt_REOBseq = 'G';
		m3l_send_receive_tcpipsocket(Gnode, (lmchar_t *)NULL, sockfd, Popts_1);
		
		if(Gnode == NULL)
			Error("transfer data error");
		
		if(m3l_Umount(&Gnode) != 1)
			Perror("m3l_Umount");
		
		if( close(sockfd) == -1)
			Perror("close");
}






lmint_t client_recevier(void *data, const lmchar_t *hostname, lmint_t portno, lmchar_t *data_name, lmchar_t SR_MODE, opts_t *Popts, opts_t *Popst_lm3l){

	node_t *Gnode, *TmpNode;
	lmint_t sockfd, retval;
	
	opts_t *Popts_1, opts;
	
	struct timespec tim, tim2;
	tim.tv_sec = 0;
	tim.tv_nsec = 100000000L;    /* 0.1 secs */

	opts.opt_linkscleanemptlinks = '\0';  // clean empty links
	opts.opt_nomalloc = '\0'; // if 'm', do not malloc (used in Mklist --no_malloc
	opts.opt_linkscleanemptrefs = '\0'; // clean empty link references
	opts.opt_tcpencoding = 't'; // serialization and encoding when sending over TCP/IP
	opts.opt_shutdown = '\0'; // shutdown when finished with sending
	opts.opt_MEMCP = 'S';  // type of buffering
	opts.opt_EOBseq = '\0'; // send EOFbuff sequence only
	opts.opt_REOBseq = '\0'; // read EOFbuff sequence only
	
	
	Popts_1 = &opts;
/*
 * create header which will identify name of data set and Sender (S) or Receiver (R)
 */
// 		Gnode = Header("Pressure", 'S');
		Gnode = Header(data_name, SR_MODE);
/* 
 * if required, open socket 
 */
again:
		if ( (sockfd =  m3l_cli_open_socket(hostname, portno, (lmchar_t *)NULL)) < 0)
			Error("Could not open socket");
		
// 		if(  (TmpNode = m3l_Send_receive_tcpipsocket(Gnode,(lmchar_t *)NULL, sockfd, "--encoding" , "IEEE-754", (lmchar_t *)NULL)) == NULL)
// 			Error("Receiving data");
		
		
		if( (TmpNode = m3l_send_receive_tcpipsocket(Gnode, (lmchar_t *)NULL, sockfd, Popts_1)) == NULL){
			Perror("m3l_send_receive_tcpipsocket error");
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
/*
 * confirm the header was received (--SEOB)
 */
		opts.opt_EOBseq = 'E';
// 		m3l_Send_to_tcpipsocket((node_t *)NULL, (char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB", (char *)NULL);
		m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);
/*
 * receive payload
 */
// 		if( (Gnode = m3l_Receive_tcpipsocket((char *)NULL, sockfd, "--encoding" , "IEEE-754", (char *)NULL)) == NULL)
// 			Error("Receiving data");
		
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("Receiving data");

/*
 * confirm the data was received (--SEOB)
 */
// 		m3l_Send_to_tcpipsocket(NULL,(char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB", (char *)NULL);
		m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);
		
		if( close(sockfd) == -1)
			Perror("close");
}