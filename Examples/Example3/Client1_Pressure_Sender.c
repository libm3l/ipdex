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

	lmint_t sockfd, portno, n, status, ch_stat, *tmpint, *tmpi;

        socklen_t clilen;
        struct sockaddr_in cli_addr;
	lmchar_t *name="Pressure";

	lmint_t nmax, retval;
	lmdouble_t *tmpdf;
	client_fce_struct_t InpPar, *PInpPar;
	opts_t *Popts_1, opts;
	
	struct timespec tim, tim2;
	tim.tv_sec = 0;
	tim.tv_nsec = 1000000L;    /* 0.1 secs */

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
	PInpPar->SR_MODE = 'S';
	if ( (PInpPar->mode = get_exchange_channel_mode('D', 'Y')) == -1)
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

		Gnode = client_name("Text from Client1");
// 	
		dim = (size_t *) malloc( 1* sizeof(size_t));

		dim[0] = 10;
		if(  (TmpNode = m3l_Mklist("numbers", "D", 1, dim, &Gnode, "/Client_Data", "./", (char *)NULL)) == 0)
				Error("m3l_Mklist");
		tmpdf = (double *)m3l_get_data_pointer(TmpNode);
		for(j=0; j<10; j++)
			tmpdf[j] = (i+1)*j*1.1;
		free(dim);

		if(m3l_Cat(Gnode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
			Error("CatData");


	for(i=0; i<nmax; i++){

// 		if( i%1000 == 0)  printf("\n\n--------------------------------    i = %ld\n\n", i);
 		printf("\n\n--------------------------------    i = %ld\n\n", i);
/*
 * open socket, IP address of server is in argv[1], port number is in portno
 */
// 		dim[0] = 1;
// /*
//  * add iteraztion number
//  */
// 		if(  (TmpNode = m3l_Mklist("Iteration_Number", "I", 1, dim, &Gnode, "/Client_Data", "./", (char *)NULL)) == 0)
// 				Error("m3l_Mklist");
// 		TmpNode->data.i[0] = i;
/*
 * add pressure array, array has 10 pressure with some values
 */			
		if(m3l_Cat(Gnode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
 			Error("CatData");
		
		client_sender(Gnode, sockfd, PInpPar, (opts_t *)NULL, (opts_t *)NULL);

 	}

		if(m3l_Umount(&Gnode) != 1)
			Perror("m3l_Umount");

/*
 * close socket
 */
	if( close(sockfd) == -1)
		Perror("close");


     return 0; 
}
