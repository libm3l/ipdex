#include "libm3l.h"
#include "Server_Header.h"
#include "client_functions_Prt.h"
#include "ACK.h"


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
 * if required, open socket 
 */
	if(hostname != NULL){
		/*
 * create header which will identify name of data set and Sender (S) or Receiver (R)
 */
		Gnode = Header(ClientInPar->data_name, ClientInPar->SR_MODE);
again:
		if ( (sockfd =  m3l_cli_open_socket(hostname, portno, (lmchar_t *)NULL)) < 0)
			Error("Could not open socket");

		opts.opt_tcpencoding = 'I';    /*  "--encoding" , "IEEE-754"  */
	
// 		if(  (TmpNode = m3l_Send_receive_tcpipsocket(Gnode,(lmchar_t *)NULL, sockfd, "--encoding" , "IEEE-754", (lmchar_t *)NULL)) == NULL)
// 			Error("Receiving data");
/*
 * send header identifying name which connection will be used. Upon receiving this info, 
 * server will send back the answer
 */
		if( (TmpNode = m3l_send_receive_tcpipsocket(Gnode, (lmchar_t *)NULL, sockfd, Popts_1)) == NULL){
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

			if(hostname != NULL){
				if( close(sockfd) == -1)
					Perror("close");
			}
				
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
		sockfd = portno;
	
	
// 	printf(" CLIENT MODE is %d\n", ClientInPar->mode);
	switch(ClientInPar->mode){
	case 1:
/*
 * send data and receive confirmation that the data were received (--REOB)
 */
// 		m3l_Send_receive_tcpipsocket((node_t *)data,(char *)NULL, sockfd, "--encoding" , "IEEE-754",  "--REOB", (char *)NULL);

		opts.opt_REOBseq = 'G';  /* --REOB */
		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
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
		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
		opts.opt_EOBseq = '\0';  /* --REOB */
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
		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
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
		opts.opt_REOBseq = 'G';  /* --REOB */
		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
		m3l_send_receive_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);
// 		m3l_send_to_tcpipsocket((node_t *)END_OK, (lmchar_t *)NULL, sockfd, Popts_1);
		
	break;

	case 5:
/*
 * send data and receive confirmation that the data were received (--REOB)
 */
// 		m3l_Send_receive_tcpipsocket((node_t *)data,(char *)NULL, sockfd, "--encoding" , "IEEE-754",  "--REOB", (char *)NULL);

		opts.opt_REOBseq = 'G';  /* --REOB */
		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
		m3l_send_receive_tcpipsocket((node_t *)data, (lmchar_t *)NULL, sockfd, Popts_1);
	
	break;

	case 6:
/*
 * send data and receive confirmation that the data were received (--REOB)
 */
// 		m3l_Send_to_tcpipsocket((node_t *)data,(char *)NULL, sockfd, "--encoding" , "IEEE-754",  "--REOB", (char *)NULL);
		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
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
	
// 	printf(" Client receiver \n");
	
	if ( (Pretval = (client_recevier_struct_t *)malloc(sizeof(client_recevier_struct_t))) == NULL)
		Error("client_recevier: allocating Pretval failed ");
/* 
 * if required, open socket 
 */
	if(hostname != NULL){
		/*
 * create header which will identify name of data set and Sender (S) or Receiver (R)
 */
		Gnode = Header(ClientInPar->data_name, ClientInPar->SR_MODE);

again:
		if ( (sockfd =  m3l_cli_open_socket(hostname, portno, (lmchar_t *)NULL)) < 0)
			Error("Could not open socket");

		Pretval->sockfd = sockfd;

// 	printf(" Client receiver after socket\n");
	

// 	if(  (TmpNode = m3l_Send_receive_tcpipsocket(Gnode,(lmchar_t *)NULL, sockfd, "--encoding" , "IEEE-754", (lmchar_t *)NULL)) == NULL)
// 		Error("Receiving data");
/*
 * send header identifying name which connection will be used. Upon receiving this info, 
 * server will send back the answer
 */
		opts.opt_tcpencoding = 'I';  /*  "--encoding" , "IEEE-754"  */
		if( (TmpNode = m3l_send_receive_tcpipsocket(Gnode, (lmchar_t *)NULL, sockfd, Popts_1)) == NULL){
			Perror("m3l_send_receive_tcpipsocket error");
			
			if(m3l_Umount(&Gnode) != 1)
				Perror("m3l_Umount");
			
			Pretval->sockfd = -1;
			return (client_recevier_struct_t *)NULL;
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
			
			if(hostname != NULL){
				if( close(sockfd) == -1)
					Perror("close");
			}
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
		sockfd = portno;
	
// 		printf(" Client receiver sending data \n");

/*
 * confirm the header was received (--SEOB)
 */
	opts.opt_tcpencoding = 'I';  /*  "--encoding" , "IEEE-754"  */
	opts.opt_EOBseq = 'E';       /* --SEOB */
// 	m3l_Send_to_tcpipsocket((node_t *)NULL, (char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB", (char *)NULL);
	m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);
	
	
	switch(ClientInPar->mode){
	case 1:
/*
 * receive payload
 */
// 	if( (Gnode = m3l_Receive_tcpipsocket((char *)NULL, sockfd, "--encoding" , "IEEE-754", (char *)NULL)) == NULL)
// 		Error("Receiving data");
		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("Receiving data");
/*
 * confirm the data was received (--SEOB)
 */
// 	m3l_Send_to_tcpipsocket(NULL,(char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB", (char *)NULL);
		opts.opt_tcpencoding = 'I';  /*  "--encoding" , "IEEE-754"  */
		opts.opt_EOBseq = 'E';   /* --SEOB */
		m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);
		
		if(hostname != NULL){
			if( close(sockfd) == -1)
				Perror("close");
		}
	break;
	
	case 2:
		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
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
		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("Receiving data");
// 		m3l_send_to_tcpipsocket((node_t *)END_OK, (lmchar_t *)NULL, sockfd, Popts_1);
		
// 		if(hostname != NULL && Caller == 'S'){  /* Sender closes socket */
// 			if( close(sockfd) == -1)
// 				Perror("close");
// 		}		
	break;
	

	case 4:
		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("Receiving data");
		
		opts.opt_REOBseq = 'G';  /* --REOB */
// 		m3l_send_receive_tcpipsocket((node_t *)END_OK, (lmchar_t *)NULL, sockfd, Popts_1);
		
	break;

	case 5:
/*
 * receive payload
 */
// 	if( (Gnode = m3l_Receive_tcpipsocket((char *)NULL, sockfd, "--encoding" , "IEEE-754", (char *)NULL)) == NULL)
// 		Error("Receiving data");

		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
		if( (Gnode = m3l_receive_tcpipsocket((const lmchar_t *)NULL, sockfd, Popts_1)) == NULL)
			Error("Receiving data");
/*
 * confirm the data was received (--SEOB)
 */
// 	m3l_Send_to_tcpipsocket(NULL,(char *)NULL, sockfd, "--encoding" , "IEEE-754", "--SEOB", (char *)NULL);

		opts.opt_tcpencoding = 'I';  /*  "--encoding" , "IEEE-754"  */
		opts.opt_EOBseq = 'E';   /* --SEOB */
		m3l_send_to_tcpipsocket((node_t *)NULL, (lmchar_t *)NULL, sockfd, Popts_1);

	break;
	
	case 6:
		opts.opt_tcpencoding = 'I';   /*  "--encoding" , "IEEE-754"  */
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