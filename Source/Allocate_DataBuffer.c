

#include "libm3l.h"
#include "Server_Header.h"
#include "Allocate_DataBuffer.h"

node_t *Allocate_DataBuffer(node_t *Gnode){
/*
 * this routine allocates a structure containign 
 * information about transfered data sets and number of threads 
 * each data set is going to be accessed by
 */
	lmsize_t i, n_data_threads, *dim;
	find_t *SFounds=NULL;
	node_t *BuffNode=NULL, *TmpNode=NULL;
	
	if(Gnode == NULL){
		Warning("Data_Thread: NULL Gnode");
		return NULL;
	}
/*
 * find how many data sets - defines how many data_threads to will be required
 */
	if( (SFounds = m3l_Locate(Gnode, "/COMM_DEF/Data_Sets/Data_Set", "/*/*/*", (lmchar_t *)NULL)) != NULL){
		
		n_data_threads = m3l_get_Found_number(SFounds);
		
		if(n_data_threads == 0){
			Error("Server: did not find any Data_set");
			m3l_DestroyFound(&SFounds);
		}
	}
	else
	{
		printf("Server: did not find any Data_set\n");
		exit(0);
	}
/*
 * make buffer structure
 */
	if(  (BuffNode = m3l_Mklist("Buffer", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
		Perror("m3l_Mklist");

	dim = (size_t *) malloc( 1* sizeof(lmsize_t));
	dim[0] = 1;

	if(  (TmpNode = m3l_Mklist("N_data_sets", "ST", 1, dim, &BuffNode, "/Buffer", "./", (char *)NULL)) == 0)
		Error("m3l_Mklist");
	TmpNode->data.st[0] = n_data_threads;
	
	if(  (TmpNode = m3l_Mklist("Buff_Status", "I", 1, dim, &BuffNode, "/Buffer", "./", (char *)NULL))  == 0)
		Error("m3l_Mklist");
	TmpNode->data.i[0] = 0;
	
	free(dim);


	for(i=0; i < n_data_threads; i++){
		
		TmpNode = m3l_get_Found_node(SFounds, i);
		
// 		if(m3l_Cat(TmpNode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
// 			Error("CatData");
		
		m3l_Mv(&TmpNode,  "./Data_Set", "./*", &BuffNode, "/Buffer", "/*", (lmchar_t *)NULL); 		
	}
	
	
			if(m3l_Cat(BuffNode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
			Error("CatData");
		/*	if(m3l_Cat(Gnode, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
			Error("CatData");*/	

	m3l_DestroyFound(&SFounds);
// 	exit(0);
	
	return BuffNode;
}