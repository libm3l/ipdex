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
#include "ipdex.h"


int main(int argc, char *argv[])
{
	node_t *Gnode=NULL, *RecNode=NULL, *TmpNode = NULL;
	pid_t  childpid;
	size_t *dim, i, j;

	int sockfd, portno, n, status, ch_stat, *tmpint, *tmpi;

        socklen_t clilen;
        struct sockaddr_in cli_addr;
	char *name="Temperature";

	int nmax, retval;
	double *tmpdf;
	
	client_fce_struct_t InpPar, *PInpPar;
	client_receiver_struct_t *Pretval;

	struct timespec tim, tim2;
	tim.tv_sec = 0;
	tim.tv_nsec = 100000000L;    /* 0.1 secs */
	tim.tv_sec = 0;
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
 * open socket - because we use more then just send - receive scenario
 * we need to open socket manualy and used Send_receive function with hostname = NULL, ie. as server
 * portno is then replaced by socket number
 */
 	for(i=0; i<nmax; i++){

 		printf("\n\n--------------------------------    i = %ld\n\n", i);

		PInpPar->data_name = name;
		PInpPar->SR_MODE = 'R';
		if ( (PInpPar->mode = get_exchange_channel_mode('D', 'N')) == -1)
			Error("wrong client mode");
		
		Pretval = client_receiver(argv[1], portno, PInpPar, (opts_t *)NULL, (opts_t *)NULL);
		if(m3l_Cat(Pretval->data, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
			Error("CatData");
		
		if(m3l_Umount(&Pretval->data) != 1)
			Perror("m3l_Umount");
		free(Pretval);
// 		if(nanosleep(&tim , &tim2) < 0 )
// 			Error("Nano sleep system call failed \n");

 	}


     return 0; 
}
