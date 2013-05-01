

#include "libm3l.h"
#include "Server_Header.h"
#include "Allocate_DataBuffer.h"

node_t *Allocate_DataBuffer(node_t *Gnode){
/*
 * this routine allocates a structure containign 
 * information about transfered data sets and number of threads 
 * each data set is going to be accessed by
 */
	lmsize_t i, n_data_threads;
	find_t *SFounds;
	node_t *RetNode;
	
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

// 	size_t *dim;
// 	
// 	if(  (Gnode = m3l_Mklist("Answer", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
// 		Perror("m3l_Mklist");
// 	
// 	dim = (size_t *) malloc( 1* sizeof(size_t));
// 	dim[0] = strlen(answer)+1;
// 	
// 	if(  (TmpNode = m3l_Mklist("ANSWER", "C", 1, dim, &Gnode, "/Answer", "./", "--no_malloc", (char *)NULL)) == 0)
// 		Error("m3l_Mklist");
// 	TmpNode->data.c = answer;
// 	
// 	free(dim);

	for(i=0; i < n_data_threads; i++){

	}

	return RetNode;
}