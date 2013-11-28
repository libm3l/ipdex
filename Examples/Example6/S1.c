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
 *     Function Client1.c
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
#include "lsipdx.h"


int main(int argc, char *argv[])
{
	node_t *Gnode=NULL, *RecNode=NULL, *TmpNode = NULL;
	pid_t  childpid;
	size_t *dim, i, j;

	lmint_t sockfd, portno1, portno2, n, status, ch_stat, *tmpint, *tmpi;

        socklen_t clilen;
        struct sockaddr_in cli_addr;
	lmchar_t *nameIn="DATA_4";
	lmchar_t *nameOut="DATA_1";

	lmint_t nmax, retval;
	lmdouble_t *tmpdf;
	client_fce_struct_t InpPar, *PInpPar;
	opts_t opts, *Popts_1;
	
	struct timespec tim, tim2;
	tim.tv_sec = 5;
// 	tim.tv_sec = 0;
// 	tim.tv_nsec = 300000000L;    /* 0.1 secs */
	tim.tv_nsec = 10000000L;    /* 0.1 secs */

	nmax = 100000;
	PInpPar = &InpPar;
/*
 * get port number
 */
     if (argc < 4) {
       fprintf(stderr,"ERROR, no IPaddress and port numbers provided\n");
       exit(1);
     }
 	portno1 = atoi(argv[2]);
 	portno2 = atoi(argv[3]);
/*
 * open socket - because we use more then just send - receive scenario
 * we need to open socket manualy and used Send_receive function with hostname = NULL, ie. as server
 * portno is then replaced by socket number
 */
	for(i=0; i<nmax; i++){

 		printf("\n\n--------------------------------    i = %ld\n\n", i);
/*
 * open socket, IP address of server is in argv[1], port number is in portno
 */
		Gnode = client_name("Text from Solver_1");
// 	
		dim = (size_t *) malloc( 1* sizeof(size_t));
		dim[0] = 1;
/*
 * add iteraztion number
 */
		if(  (TmpNode = m3l_Mklist("Iteration_Number", "I", 1, dim, &Gnode, "/Client_Data", "./", (char *)NULL)) == 0)
				Error("m3l_Mklist");
		TmpNode->data.i[0] = i;
/*
 * add pressure array, array has 5 pressure with some values
 */	
		dim[0] = 10;
		if(  (TmpNode = m3l_Mklist("S1_numbers", "D", 1, dim, &Gnode, "/Client_Data", "./", (char *)NULL)) == 0)
				Error("m3l_Mklist");
		tmpdf = (double *)m3l_get_data_pointer(TmpNode);
		for(j=0; j<10; j++)
			tmpdf[j] = (i+1)*j*1.1;
		free(dim);
		
// 		if(m3l_Cat(Gnode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
// 			Error("CatData");
/*
 * open socket
 */
		PInpPar->data_name = nameOut;
		PInpPar->SR_MODE = 'S';
		if ( (PInpPar->mode = get_exchange_channel_mode('D', 'N')) == -1)
			Error("wrong client mode");
		Popts_1 = &opts;
		m3l_set_Send_receive_tcpipsocket(&Popts_1);
	
		if( (sockfd = open_connection_to_server(argv[1], portno1, PInpPar, Popts_1)) < 1)
			Error("client_sender: Error when opening socket");
		
		client_sender(Gnode, sockfd,  PInpPar, (opts_t *)NULL, (opts_t *)NULL);
/* 
 * close socket
 */
		if( close(sockfd) == -1)
			Perror("close");
		
		if(m3l_Umount(&Gnode) != 1)
			Perror("m3l_Umount");
		
		if(nanosleep(&tim , &tim2) < 0 )
			Error("Nano sleep system call failed \n");	
/*
 * open socket
 */
		PInpPar->data_name = nameIn;
		PInpPar->SR_MODE = 'R';
		if ( (PInpPar->mode = get_exchange_channel_mode('D', 'N')) == -1)
			Error("wrong client mode");
		Popts_1 = &opts;
		m3l_set_Send_receive_tcpipsocket(&Popts_1);
	
		if( (sockfd = open_connection_to_server(argv[1], portno2, PInpPar, Popts_1)) < 1)
			Error("client_sender: Error when opening socket");
		
		Gnode = client_receiver(sockfd,  PInpPar, (opts_t *)NULL, (opts_t *)NULL);
		
		if(m3l_Cat(Gnode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
			Error("CatData");
/* 
 * close socket
 */
		if( close(sockfd) == -1)
			Perror("close");
		
		if(m3l_Umount(&Gnode) != 1)
			Perror("m3l_Umount");

 	}


     return 0; 
}
