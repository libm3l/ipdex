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
	size_t i, j, *dim;

	int sockfd, portno, n, status, ch_stat, *tmpint, *tmpi;

        socklen_t clilen;
        struct sockaddr_in cli_addr;
	char *name="Pressure";

	int nmax, retval;
	double *tmpdf;
	opts_t opts, *Popts_1;
	
	client_fce_struct_t InpPar, *PInpPar;

	struct timespec tim, tim2;
	tim.tv_sec  = 0;
	tim.tv_nsec = 100000000L;    /* 0.1 secs */
	tim.tv_sec  = 0;
	tim.tv_nsec = 10000000L;    /* 0.01 secs */

	nmax = 100000;
	
	PInpPar = &InpPar;
/*
 * get port number
 */
     if (argc < 3) {
       fprintf(stderr,"ERROR, no IPaddress and port number provided\n");
       exit(1);
     }
 	portno = atoi(argv[2]);
/*
 * open socket
 */
	PInpPar->data_name = name;
	PInpPar->SR_MODE = 'R';
	if ( (PInpPar->mode = get_exchange_channel_mode('A', 'Y')) == -1)
		Error("wrong client mode");
	Popts_1 = &opts;
	m3l_set_Send_receive_tcpipsocket(&Popts_1);
	
	if( (sockfd = open_connection_to_server(argv[1], portno, PInpPar, Popts_1)) < 1)
		Error("client_sender: Error when opening socket");
/*
 * open socket - because we use more then just send - receive scenario
 * we need to open socket manualy and used Send_receive function with hostname = NULL, ie. as server
 * portno is then replaced by socket number
 */
	dim = (size_t *) malloc( 1* sizeof(size_t));
	Gnode = client_name("T2222");
		
	dim[0] = 1;
/*
 * add iteration number
 */
	if(  (TmpNode = m3l_Mklist("Iteration_Number", "I", 1, dim, &Gnode, "/Client_Data", "./", (char *)NULL)) == 0)
			Error("m3l_Mklist");
	TmpNode->data.i[0] = i;


	for(i=0; i<nmax; i++){

 		printf("\n\n--------------------------------    i = %ld\n\n", i);
		
		RecNode = client_receiver(sockfd, PInpPar, (opts_t *)NULL, (opts_t *)NULL);
		
 		if(m3l_Cat(RecNode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
 			Error("CatData");
		
		if(m3l_Umount(&RecNode) != 1)
			Perror("m3l_Umount");
		TmpNode->data.i[0] = i;		
		client_sender(Gnode, sockfd, PInpPar, (opts_t *)NULL, (opts_t *)NULL);


// 		if(nanosleep(&tim , &tim2) < 0 )
// 			Error("Nano sleep system call failed \n");

 	}
	free(dim);
	if(m3l_Umount(&Gnode) != 1)
		Perror("m3l_Umount");/* 

 * close socket
 */
	if( close(sockfd) == -1)
		Perror("close");
		

     return 0; 
}
