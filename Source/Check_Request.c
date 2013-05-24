
#include "libm3l.h"
#include "Server_Header.h"
#include "Check_Request.h"

lmint_t Check_Request(node_t *DataBuffer, node_t *RecNode, lmchar_t *name_of_required_data_set, lmchar_t *SR_mode, lmchar_t *Name_Of_SRProc, lmint_t initreq){
/*
 * function compares data in RecNode to what is in DataBuffer
 * looking specifically on name_of_required_data_set and SR_mode
 * 
 * if initreq == 0, RecNode was received from the socket. In this case, if the Data thread is occupied, save it in the buffer for later use
 * if initreq == 1 RecNode is one of the nodes stored in the buffer and is just rechecked
 */
	find_t *DATA_SFounds, *SRproc_Founds, *THRStat_SFounds, *Tqst_SFounds, *QueuedNDS_SFounds, *QueuedSRm_SFounds;
	size_t n_data_threads, i, n_queue_reqst, len1, len2;
	node_t *TmpNode, *Queued_Reqst;
	lmint_t Thread_Status, ThrStat, status;
	lmchar_t *name, *srmode;
	
	if(initreq == 0){
/*
 * find Queued_Reqst 
 */
		if( (Tqst_SFounds = m3l_Locate(DataBuffer, "/Buffer/Queued_Reqst", "/*/*", (lmchar_t *)NULL)) == NULL){
			printf("Server: did not find any Data_set\n");
			exit(0);
		}
/*
 * only one Queued_Reqst exists
 */
		Queued_Reqst = m3l_get_Found_node(Tqst_SFounds, 0);
		n_queue_reqst = m3l_get_List_ndim(Queued_Reqst);
/*
 * check if RecNode is identical to one of the nodes in Queued_Reqst
 * two characteristics are used - Name_of_Data_Set and SR_mode
 * if it is so, return -1
 * Check only if initreq == 0
 */
		if(n_queue_reqst > 0){
			
		len1 = strlen(name_of_required_data_set);
			
			if( (QueuedNDS_SFounds = m3l_Locate(Queued_Reqst, "./Queued_Reqst/Header/Name_of_Data_Set", "./*/*/*", (lmchar_t *)NULL)) != NULL){
				printf("Server: did not find any ./Queued_Reqst/Header/Name_of_Data_Set\n");
				exit(0);
			}	
			if( (QueuedSRm_SFounds = m3l_Locate(Queued_Reqst, "./Queued_Reqst/Header/SR_mode", "./*/*/*", (lmchar_t *)NULL)) != NULL){
				printf("Server: did not find any ./Queued_Reqst/Header/SR_mode\n");
				exit(0);
			}
			
			
			for(i=0; i<n_queue_reqst; i++){
				
				TmpNode = m3l_get_Found_node(QueuedNDS_SFounds, i);
				name = (lmchar_t *)m3l_get_data_pointer(TmpNode);
				TmpNode = m3l_get_Found_node(QueuedSRm_SFounds, i);
				srmode = (lmchar_t *)m3l_get_data_pointer(TmpNode);
				
				len2 = strlen(name);
				
				if(len1 == len2 && strncmp(name, name_of_required_data_set, len1) == 0){
/*
 * if Sender (S) is already saved, disregard this one.
 * only one S per session is allowed
 */
					if(SR_mode == srmode && SR_mode == 'S'){
						m3l_DestroyFound(&QueuedSRm_SFounds);
						m3l_DestroyFound(&QueuedNDS_SFounds);
						return -1;
					}
					
					m3l_DestroyFound(&QueuedSRm_SFounds);
					m3l_DestroyFound(&QueuedNDS_SFounds);
					return -1;
				}
			}
			m3l_DestroyFound(&QueuedSRm_SFounds);
			m3l_DestroyFound(&QueuedNDS_SFounds);
		}
		
		m3l_DestroyFound(&Tqst_SFounds);

	}
	status = 0;
/*
 * if RecNode is not already in Queued_Reqst queue 
 * find if the data set thread is available
 * if not available, save RecNode in Queued_Reqst
 */
	if( (DATA_SFounds = m3l_Locate(DataBuffer, "/Buffer/Data_Set", "/*/*", (lmchar_t *)NULL)) != NULL){
		
		n_data_threads = m3l_get_Found_number(DATA_SFounds);
		
		if(n_data_threads == 0){
			Error("Server: did not find any Data_set");
			m3l_DestroyFound(&DATA_SFounds);
		}
	}
	else
	{
		printf("Server: did not find any Data_set\n");
		exit(0);
	}
/*
 * loop over and find out it the process thread is free to use
 */
	for(i=0; i< n_data_threads; i++){
		
		if( (THRStat_SFounds = m3l_Locate(m3l_get_Found_node(DATA_SFounds, i), "./Data_Set/Thread_Status", "/*/*", (lmchar_t *)NULL)) != NULL){
			
			if(n_data_threads == 0){
				Error("Server: did not find any Thread_Status");
				m3l_DestroyFound(&THRStat_SFounds);
			}
		}
		else
		{
			printf("Server: did not find any Thread_Status\n");
			exit(0);
		}
	
		TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
		Thread_Status = *(lmint_t *)m3l_get_data_pointer(TmpNode);
/*
 * Thread_Status == 0 data thread is free
 * Thread_Status == 1 data thread is busy
 */
		m3l_DestroyFound(&THRStat_SFounds);
	}
/*
 * if Thread_Status == 0, thread is free, if Thread_Status == 1, thread is not free, 
 * 
 * if initreq == 0 and Thread_Status == 1 save RecNode in the buffer under /Buffer/Queued_Reqst
 * - check if the Similar node is not already there
 * 
 *  * if initreq == 1 and Thread_Status == 0 detach RecNode from the buffer under /Buffer/Queued_Reqst
 */	
	if(initreq == 0 && Thread_Status == 1){
// 		m3l_Mv(&RecNode, "/Header", "/*", &DataBuffer, "/Buffer/Queued_Reqst", "/*/*", (lmchar_t *)NULL);
		m3l_Mv(&RecNode, "/Header", "/*", &Queued_Reqst, "./*", "./*", (lmchar_t *)NULL);
		
			if(m3l_Cat(DataBuffer, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
				Error("CatData");
		
		status = 1;
	}
	else if(initreq == 1 && Thread_Status == 0){
/*
 * checking data sets saved in buffer. If the data_thead is available, detach the request from buffer and return
 * status 0
 */
		if( m3l_detach_list(1, &RecNode, NULL) != 1) 	/*only one list is detached */
			Error("Detaching list");
		
		status = 0;
		
	}
	else if(initreq == 1 && Thread_Status == 1){
/*
 * request is from buffer, the data thread is still occupied
 */
		status = 1;
	}
	
	m3l_DestroyFound(&DATA_SFounds);	
	
// 	exit(0);
	
	return status;
}


find_t *find_Queued_Reqst(node_t *DataBuffer){
	find_t *Tqst_SFounds;
/*
 * find Queued_Reqst 
 */
	if( (Tqst_SFounds = m3l_Locate(DataBuffer, "/Buffer/Queued_Reqst/Header", "/*/*/*", (lmchar_t *)NULL)) == NULL)
		return (find_t *)NULL;
	
	return Tqst_SFounds;
}