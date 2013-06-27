
#include "libm3l.h"
#include "Server_Header.h"
#include "Check_Request.h"

lmint_t Check_Request(node_t *DataBuffer, lmchar_t *name_of_required_data_set, lmchar_t *SR_mode, lmchar_t *Name_Of_SRProc){
/*
 * function compares data in RecNode to what is in DataBuffer
 * looking specifically on name_of_required_data_set and SR_mode
 */
// 	find_t *DATA_SFounds, *SRproc_Founds, *THRStat_SFounds, *Tqst_SFounds, *QueuedNDS_SFounds, *QueuedSRm_SFounds;

	find_t *DATA_SFounds,*THRStat_SFounds, *THRName_SFounds;
	size_t n_data_threads, i, n_queue_reqst, len1, len2;
	node_t *TmpNode, *Queued_Reqst;
	lmint_t Thread_Status, ThrStat, status;
	lmchar_t *name, *srmode;
	
	status = 0;
	
	len1 = strlen(name_of_required_data_set);
/*
 * find if the data set thread is available
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
/*
 * find name of data thread and compare it to tested data thread name
 */
		if( (THRName_SFounds = m3l_Locate(m3l_get_Found_node(DATA_SFounds, i), "./Data_Set/Name_of_Data_Set", "/*/*", (lmchar_t *)NULL)) != NULL){
			
			if(n_data_threads == 0){
				Error("Server: did not find any Name_of_Data_Set");
				m3l_DestroyFound(&THRName_SFounds);
			}
		}
		else
		{
			printf("Server: did not find any Name_of_Data_Set\n");
			exit(0);
		}
		
		TmpNode = m3l_get_Found_node(THRName_SFounds, 0);
		name = (lmchar_t *)m3l_get_data_pointer(TmpNode);
// 		TmpNode = m3l_get_Found_node(QueuedSRm_SFounds, i);
// 		srmode = (lmchar_t *)m3l_get_data_pointer(TmpNode);
		
		len2 = strlen(name);
/*
 * if this is the required data thread, 
 * find it it is free (Thread_Status == 0 or occupied Thread_Status ==1)
 * and return 
 */
		if(len1 == len2 && strncmp(name, name_of_required_data_set, len1) == 0){

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
			
			if(Thread_Status == 1)status = 1;
			
			m3l_DestroyFound(&THRStat_SFounds);
			m3l_DestroyFound(&THRName_SFounds);
			break;
		}
/*
 * Thread_Status == 0 data thread is free
 * Thread_Status == 1 data thread is busy
 */
		m3l_DestroyFound(&THRName_SFounds);
	}
	
	m3l_DestroyFound(&DATA_SFounds);
	
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