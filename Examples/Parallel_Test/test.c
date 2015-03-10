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
 *     Function Start_Data_Thread.c
 *
 *     Date: 2013-07-16
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
#include "test.h"

pthread_mutex_t   	lock;

void *ThreadTest(void *);

int main(){

	
	test_struct_t **DataArgs;
	int i, pth_err, nthreads;
	
	pthread_t *PID;
	
	Pthread_mutex_init(&lock);
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if( (DataArgs = (test_struct_t **)malloc(sizeof(test_struct_t *) * 3)) == NULL)
		perror("malloc");

	for(i=0; i<3 ; i++){
	     if( (DataArgs[i] = (test_struct_t *)malloc(sizeof(test_struct_t))) == NULL)
		   perror("malloc");
	     if( (DataArgs[i]->name = (lmchar_t *)malloc(8*sizeof(lmchar_t))) == NULL)
		   perror("malloc");	     
	     if( (DataArgs[i]->name1 = (lmchar_t *)malloc(8*sizeof(lmchar_t))) == NULL)
		   perror("malloc");	     
	}
	
	if( (PID = (pthread_t *)malloc(sizeof(pthread_t)*3)) == NULL)
		perror("malloc");
/*
 * allocate Data_Str
 */
	DataArgs[0]->portno = 31000;
	snprintf(DataArgs[0]->name, 8, "%s", "CFD2CSM");
	DataArgs[0]->name[7] = '\0';
	snprintf(DataArgs[0]->name1, 8, "%s", "CSM2CFD");
	DataArgs[0]->name1[7] = '\0';

	DataArgs[1]->portno = 31000;
	snprintf(DataArgs[1]->name, 8, "%s", "CFD2SIM");
	DataArgs[1]->name[7] = '\0';
	snprintf(DataArgs[1]->name1, 8, "%s", "SIM2CFD");
	DataArgs[1]->name1[7] = '\0';
	
	DataArgs[2]->portno = 32000;
	snprintf(DataArgs[2]->name, 8, "%s", "CFD2CSM");
	DataArgs[2]->name[7] = '\0';
	snprintf(DataArgs[2]->name1, 8, "%s", "CSM2CFD");
	DataArgs[2]->name1[7] = '\0';
	
	nthreads = 3;
	
	for(i=0; i < nthreads; i++){

	printf(" Snding port %d channels '%s' and '%s'\n", DataArgs[i]->portno, DataArgs[i]->name, DataArgs[i]->name1);

	while ( ( pth_err = pthread_create(&PID[i], &attr, &ThreadTest,  DataArgs[i])) != 0 && errno == EAGAIN);
		if(pth_err != 0)
			Perror("pthread_create()");

	}
	
	printf(" Joining threads\n");
	
	for(i=0; i < nthreads; i++){	
		if( pthread_join(PID[i], NULL) != 0)
		Error(" Joining thread failed");
	}
	
	printf(" Threads joined\n");
	
	
	free(PID);
	
	for(i=0; i<nthreads ; i++){
		free(DataArgs[i]->name);
		free(DataArgs[i]->name1);
		free(DataArgs[i]);
	}	
	free(DataArgs);
}





void *ThreadTest(void *arg)
{
/* 
 * this is a thread which is associated to a specific data set name
 * the thread identifies incomming requests by comparing data set name with the 
 * arriving data set name and if possitive, decrement number of available threads 
 * for the particular data set (the numbet of available threads at the beginning = S + number_o_R threads
 * Once the number of available threads == 0, (ie. all S + R threads arrived) the 
 * thread notifies SR_Hub which synchronizes the S_R transfer (SR_Threads)
 *
 * Number of all active Data_Thread == number of transferred data sets + 1. The values is set in 
 * Data_Thread (*Data_Thread->sync->nthreads = *Data_Thread->n_data_threads + 1;
 */
	test_struct_t *c = (test_struct_t *)arg;

	client_fce_struct_t InpPar, *PInpPar;
	opts_t *Popts_1, opts, opts_1, *Popts;

	node_t *Gnode=NULL, *TmpNode = NULL, *FoundNode = NULL;
	size_t dim[1], i, tot_dim;

	lmint_t sockfd, portno;
	
	lmdouble_t *tmpfloat;
	find_t *SFounds;
	
	portno = c->portno;
	
	
	printf(" port %d channels '%s' and '%s'\n", portno, c->name, c->name1);
	
// Pthread_mutex_lock(&lock);


	PInpPar = &InpPar;
	PInpPar->channel_name = c->name;
	PInpPar->SR_MODE = 'S';
	if ( (PInpPar->mode = get_exchange_channel_mode('D', 'N')) == -1)
		Error("socket_issanastr2edge_disp: wrong client mode");
	
	Popts   = &opts;
	Popts_1 = &opts_1;
	m3l_set_Send_receive_tcpipsocket(&Popts_1);
	m3l_set_Find(&Popts);
	
	if(  (Gnode = m3l_Mklist("CFD_2_CSM", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
		Perror("socket_issanastr2edge_disp: m3l_Mklist");

	dim[0] = 1;
	if(  (TmpNode = m3l_Mklist("Port", "I", 1, dim, &Gnode, "/CFD_2_CSM", "./", (char *)NULL)) == 0)
		Error("socket_issanastr2edge_disp: m3l_Mklist");
	TmpNode->data.i[0] = portno;
	
	if(  (TmpNode = m3l_Mklist("Time", "D", 1, dim, &Gnode, "/CFD_2_CSM", "./", (char *)NULL)) == 0)
		Error("socket_issanastr2edge_disp: m3l_Mklist");
	TmpNode->data.df[0] = 1;
	
	dim[0] = 10;
	if(  (TmpNode = m3l_Mklist("P", "D", 1, dim, &Gnode, "/CFD_2_CSM", "./", (char *)NULL)) == 0)
		Error("socket_issanastr2edge_disp: m3l_Mklist");
	for(i=0; i<dim[0]; i++)
		TmpNode->data.df[i] = i;
	
	Pthread_mutex_lock(&lock);
		printf(" Port number %d\n\n", portno);
		
// 	if(m3l_Cat(Gnode, "--detailed", "-P", "-L",  "*",   (char *)NULL) != 0)
// 		Error("CatData");
	Pthread_mutex_unlock(&lock);
	
	printf(" OPENING port %d channels '%s' and '%s'\n", portno, c->name, c->name1);
	
// Pthread_mutex_lock(&lock);	
/*
 * open socket
 */
	if( (sockfd = open_connection_to_server("localhost", c->portno, PInpPar, Popts_1)) < 1)
		Error("client_sender: Error when opening socket");
	
	printf(" OPENED port %d channels '%s' and '%s'\n", portno, c->name, c->name1);

// Pthread_mutex_unlock(&lock);

/*
 * send data 
 */
	if(m3l_Cat(Gnode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
		Error("CatData");
	
	
	if ( client_sender(Gnode, sockfd, PInpPar, (opts_t *)NULL, (opts_t *)NULL) !=1 )
		Error("socket_issanastr2edge_disp: client_sender()");
	
	if( close(sockfd) == -1)
		Perror("socket_edge2simulink: close");
/*
 * free borrowed memory
 */
	if(m3l_Umount(&Gnode) != 1)
		Perror("socket_issanastr2edge_disp: m3l_Umount");

// Pthread_mutex_unlock(&lock);
	
	printf(" SENT port %d channels '%s' and '%s'\n", portno, c->name, c->name1);
/*
 * receive data 
 */
	PInpPar = &InpPar;
	PInpPar->channel_name = c->name1;
	PInpPar->SR_MODE = 'R';
	if ( (PInpPar->mode = get_exchange_channel_mode('D', 'N')) == -1)
		Error("socket_edge2stripe: wrong client mode");

	Popts   = &opts;
	Popts_1 = &opts_1;
	m3l_set_Send_receive_tcpipsocket(&Popts_1);
	m3l_set_Find(&Popts);
// 	printf(" receiving data from connection \n");
	if( (sockfd = open_connection_to_server("localhost", portno, PInpPar, Popts_1)) < 1)
		Error("socket_edge2stripe: Error when opening socket");
	
	printf(" OPENED port %d channels '%s''\n", portno, c->name1);

	if ( (Gnode = client_receiver(sockfd, PInpPar, (opts_t *)NULL, (opts_t *)NULL)) == NULL)
		Error("socket_edge2stripe: client_receiver()"); 
/*
 * close socket 
 */
// 	printf(" data received from connection \n");

	if( close(sockfd) == -1)
		Perror("socket_edge2stripe: close");
/*
 * find displacements DX, DY, DZ and copy the values to Edge allocated memory
 */
	if( (SFounds = m3l_Locate(Gnode, "/CSM_2_CFD/DX", "/*/*",  (lmchar_t *)NULL)) != NULL){

		if( m3l_get_Found_number(SFounds) != 1)
			Error("socket_edge2stripe: More then one DX data set found");
/* 
 * pointer to list of found nodes
 */
		if( (FoundNode = m3l_get_Found_node(SFounds, 0)) == NULL)
			Error("socket_edge2stripe: Did not find 1st data pointer");
		tot_dim = m3l_get_List_totdim(FoundNode);
		if( (tmpfloat = (lmdouble_t *)m3l_get_data_pointer(FoundNode)) == NULL)
			Error("socket_edge2stripe: Did not find DX data pointer");
/* 
 * free memory allocated in m3l_Locate
 */
		m3l_DestroyFound(&SFounds);
	}
	else
	{
		Error("socket_edge2stripe: DX not found\n");
	}
		
	if( (SFounds = m3l_Locate(Gnode, "/CSM_2_CFD/DY", "/*/*",  (lmchar_t *)NULL)) != NULL){
		
		if( m3l_get_Found_number(SFounds) != 1)
			Error("socket_edge2stripe: More then one DX data set found");
/* 
 * pointer to list of found nodes
 */
		if( (FoundNode = m3l_get_Found_node(SFounds, 0)) == NULL)
			Error("socket_edge2stripe: Did not find 1st data pointer");
		tot_dim = m3l_get_List_totdim(FoundNode);
		if( (tmpfloat = (lmdouble_t *)m3l_get_data_pointer(FoundNode)) == NULL)
			Error("socket_edge2stripe: Did not find DY data pointer");
/* 
 * free memory allocated in m3l_Locate
 */
		m3l_DestroyFound(&SFounds);
	}
	else
	{
		Error("socket_edge2stripe: DY not found\n");
	}

	if( (SFounds = m3l_Locate(Gnode, "/CSM_2_CFD/DZ", "/*/*",  (lmchar_t *)NULL)) != NULL){

		if( m3l_get_Found_number(SFounds) != 1)
			Error("socket_edge2stripe: More then one DZ data set found");
/* 
 * pointer to list of found nodes
 */
		if( (FoundNode = m3l_get_Found_node(SFounds, 0)) == NULL)
			Error("socket_edge2stripe: Did not find 1st data pointer");
		tot_dim = m3l_get_List_totdim(FoundNode);
		if( (tmpfloat = (lmdouble_t *)m3l_get_data_pointer(FoundNode)) == NULL)
			Error("socket_edge2stripe: Did not find DZ data pointer");
/* 
 * free memory allocated in m3l_Locate
 */
		m3l_DestroyFound(&SFounds);
	}
	else
	{
		Error("socket_edge2stripe: DZ not found\n");
	}
/*
 * free borrowed memory
 */
	if(m3l_Umount(&Gnode) != 1)
		Perror("socket_edge2stripe: m3l_Umount");
	
	printf(" RECEIVED port %d channels '%s' and '%s'\n", portno, c->name, c->name1);
	
// Pthread_mutex_unlock(&lock);


	return NULL;
}
