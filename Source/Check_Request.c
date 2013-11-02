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
 *     Function Check_Request.c
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
#include "ipdex_header.h"
#include "Check_Request.h"

lmint_t Check_Request(node_t *DataBuffer, lmchar_t *name_of_required_data_set, lmchar_t *SR_mode, lmchar_t *Name_Of_SRProc){
/*
 * function compares data in RecNode to what is in DataBuffer
 * looking specifically on name_of_required_data_set and SR_mode
 */
	find_t *DATA_SFounds,*THRStat_SFounds, *THRName_SFounds;
	size_t n_data_threads, i, n_queue_reqst, len1, len2;
	node_t *TmpNode, *Queued_Reqst;
	lmint_t Thread_Status, ThrStat, status;
	lmchar_t *name, S_Status;
	lmsize_t R_Status, Receiving_Processes;

	status = 0;


	  if(m3l_Cat(DataBuffer, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
		Error("CatData");

	 exit(0);
	
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
 * loop over and find out if the process thread is free to use
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
		
		len2 = strlen(name);
/*
 * if this is the required data thread, 
 * find it it is free (Thread_Status == 0 or occupied Thread_Status ==1)
 * and return 
 */
		if(len1 == len2 && strncmp(name, name_of_required_data_set, len1) == 0){
/*
 * check status of the thread
 */
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
 * thread is already occupied, ie. all S and R requests arrived
 * The vales 1 is set in Thread_Prt (the thread is blocked there) the value is set to 0 in SR_hub (the thread is unblocked after 
 * transfer of the data set is finished
 */
			if(Thread_Status == 1){
				status = 1;
			}
/*
 * check R and S status
 * if S == 1 Sender already arrived
 * if R == Receiving_Processes all receivers already arrived
 * 
 */			else if( *SR_mode == 'S'){
				if( (THRStat_SFounds = m3l_Locate(m3l_get_Found_node(DATA_SFounds, i), "./Data_Set/S_Status", "/*/*", (lmchar_t *)NULL)) != NULL){
				
					if(n_data_threads == 0){
						Error("Server: did not find any S_Status");
						m3l_DestroyFound(&THRName_SFounds);
						m3l_DestroyFound(&THRStat_SFounds);
					}
				}
				else
				{
					printf("Server: did not find any S_Status\n");
					exit(0);
				}
				
				TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
				if( (S_Status = *(lmint_t *)m3l_get_data_pointer(TmpNode))){
					status = 1;}
				
			}
			else if( *SR_mode == 'R'){
/*
 * find actual number of Receiving processes
 */
				if( (THRStat_SFounds = m3l_Locate(m3l_get_Found_node(DATA_SFounds, i), "./Data_Set/R_Status", "/*/*", (lmchar_t *)NULL)) != NULL){
				
					if(n_data_threads == 0){
						Error("Server: did not find any S_Status");
						m3l_DestroyFound(&THRName_SFounds);
						m3l_DestroyFound(&THRStat_SFounds);
					}
				}
				else
				{
					printf("Server: did not find any S_Status\n");
					exit(0);
				}
				TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
				R_Status = *(lmsize_t *)m3l_get_data_pointer(TmpNode);
				m3l_DestroyFound(&THRName_SFounds);
/*
 * find required number of Receiving_Processes
 */
				if( (THRStat_SFounds = m3l_Locate(m3l_get_Found_node(DATA_SFounds, i), "./Data_Set/Receiving_Processes", "/*/*", (lmchar_t *)NULL)) != NULL){
				
					if(n_data_threads == 0){
						Error("Server: did not find any S_Status");
						m3l_DestroyFound(&THRName_SFounds);
						m3l_DestroyFound(&THRStat_SFounds);
					}
				}
				else
				{
					printf("Server: did not find any S_Status\n");
					exit(0);
				}
				TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
				Receiving_Processes = *(lmsize_t *)m3l_get_data_pointer(TmpNode);
				
				if( Receiving_Processes == R_Status) status=1;
			}
			
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