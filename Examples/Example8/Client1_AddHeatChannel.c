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
// 	lmchar_t *name="systemreq";
	lmchar_t *name="pressure";

	lmint_t nmax, retval;
	lmdouble_t *tmpdf;
	client_fce_struct_t InpPar, *PInpPar;
	opts_t opts, *Popts_1;
	
	struct timespec tim, tim2;
// 	tim.tv_sec = 1;
	tim.tv_sec = 0;
// 	tim.tv_nsec = 300000000L;    /* 0.1 secs */
	tim.tv_nsec = 10000000L;    /* 0.1 secs */

	nmax = 100000;
	PInpPar = &InpPar;
/*
 * get port number
 */
     if (argc < 4) {
       fprintf(stderr,"ERROR, no IPaddress, port number or name of new channel provided\n");
       exit(1);
     }
 	portno = atoi(argv[2]);
/*
 * open socket - because we use more then just send - receive scenario
 * we need to open socket manualy and used Send_receive function with hostname = NULL, ie. as server
 * portno is then replaced by socket number
 */
/*
 * open socket, IP address of server is in argv[1], port number is in portno
 */
		Gnode = ChannelList("HEAT", 2,  'D', 'N');
/*
 * open socket
 */
		PInpPar->data_name = name;
		PInpPar->SR_MODE = 'X';
		if ( (PInpPar->mode = get_exchange_channel_mode('D', 'N')) == -1)
			Error("wrong client mode");
		Popts_1 = &opts;
		m3l_set_Send_receive_tcpipsocket(&Popts_1);
	
// 		if( (sockfd = open_connection_to_server(argv[1], portno, PInpPar, Popts_1)) < 1)
		sockfd = add_connection(argv[1], portno, argv[3], Popts_1);
		if( sockfd  == -1)
			Error("client_sender: Error when opening socket");
		else if(sockfd == 101)
			Warning("Required new connection aleady exists");
		else if(sockfd == -2)
			Warning("max attempt exceeded");		
/* 
 * close socket
 */
		else
			if( close(sockfd) == -1)
				Perror("close");
		
		if(m3l_Umount(&Gnode) != 1)
			Perror("m3l_Umount");

     return 0; 
}
