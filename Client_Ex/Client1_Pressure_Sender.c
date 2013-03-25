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
#include "ACK.h"

// static node_t *client_name(char *);

int main(int argc, char *argv[])
{
	node_t *Gnode=NULL, *RecNode=NULL, *TmpNode = NULL;
	pid_t  childpid;
	size_t *dim, i, j;

	int sockfd, portno, n, status, ch_stat, *tmpint;

        socklen_t clilen;
        struct sockaddr_in cli_addr;
	char *name="Pressure";

	int nmax;
	double *tmpdf;

	nmax = 5;
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
// 	for(i=0; i<nmax; i++){

// 		printf("\n\n--------------------------------    i = %ld\n\n", i);
/*
 * open socket, IP address of server is in argv[1], port number is in portno
 */
		Gnode = Header("Pressure", 'S');
		
		if(m3l_Cat(Gnode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
			Error("CatData");
		
		if ( (sockfd =  m3l_cli_open_socket(argv[1], portno, (char *)NULL)) < 0)
			Error("Could not open socket");

		m3l_Send_receive_tcpipsocket(Gnode,(char *)NULL, sockfd, "--encoding" , "IEEE-754",  "--REOB", (char *)NULL);

		if(m3l_Umount(&Gnode) != 1)
			Perror("m3l_Umount");
		
		
		Gnode = client_name("Text from Client1");
	
		dim = (size_t *) malloc( 1* sizeof(size_t));
		dim[0] = 1;
/*
 * add iteraztion number
 */
		if(  (TmpNode = m3l_Mklist("Iteration_Number", "I", 1, dim, &Gnode, "/Client_Data", "./", (char *)NULL)) == 0)
				Error("m3l_Mklist");
		TmpNode->data.i[0] = i;
/*
 * add pressure array, array has 10 pressure with some values
 */	
		dim[0] = 10;
		if(  (TmpNode = m3l_Mklist("numbers", "D", 1, dim, &Gnode, "/Client_Data", "./", (char *)NULL)) == 0)
				Error("m3l_Mklist");
		tmpdf = (double *)m3l_get_data_pointer(TmpNode);
		for(j=0; j<10; j++)
			tmpdf[j] = i*j*1.1;
		free(dim);
		
		if( close(sockfd) == -1)
			Perror("close");
// 	}


     return 0; 
}

// node_t *client_name(char *name)
// {
// 	node_t *Gnode, *TmpNode;
// 	char *answer="ACKN";
// 	size_t *dim;
// 	
// 	if(  (Gnode = m3l_Mklist("Client_Data", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
// 		Perror("m3l_Mklist");
// 	
// 	dim = (size_t *) malloc( 1* sizeof(size_t));
// 	dim[0] = strlen(name)+1;
// 	
// 	if(  (TmpNode = m3l_Mklist("Name", "C", 1, dim, &Gnode, "/Client_Data", "./", "--no_malloc", (char *)NULL)) == 0)
// 		Error("m3l_Mklist");
// 	TmpNode->data.c = name;
// 	
// 	free(dim);
// 	
//  	return Gnode;
// }
