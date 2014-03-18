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
#include "lsipdx_header.h"
#include "Check_Request.h"

lmint_t Check_Request(node_t *DataBuffer, lmchar_t *name_of_required_data_set, lmchar_t SR_mode){
/*
 * function compares data in RecNode to what is in DataBuffer
 * looking specifically on name_of_required_data_set and SR_mode
 * return value is 1 (Sender or all Receivers already arrived) 
 * or 0 ie. Sender or at least one Receiver is available
 */
	find_t *DATA_SFounds,*THRStat_SFounds, *THRName_SFounds;
	size_t n_data_threads, i,  len1, len2;
	node_t *TmpNode;
	lmint_t Thread_Status, status;
	lmchar_t *name, S_Status;
	lmsize_t R_Status, Receiving_Processes;
	lmint_t ident;

	status = 0;
	ident = 0;
	
	opts_t *Popts, opts;
	opts.opt_i = '\0'; opts.opt_d = '\0'; opts.opt_f = '\0'; opts.opt_r = 'r'; opts.opt_I = '\0'; opts.opt_L = '\0'; opts.opt_l = '\0';

	Popts = &opts;
	
// 	  if(m3l_Cat(DataBuffer, "--all", "-P", "-L",  "*",   (char *)NULL) != 0)
// 		Error("CatData");
	  
	len1 = strlen(name_of_required_data_set);
/*
 * find if the data set thread is available
 */
// 	if( (DATA_SFounds = m3l_Locate(DataBuffer, "/Buffer/Channel", "/*/*", (lmchar_t *)NULL)) != NULL){
	if( (DATA_SFounds = m3l_locator_caller(DataBuffer,  "/Buffer/Channel", "/*/*", Popts)) != NULL){

		n_data_threads = m3l_get_Found_number(DATA_SFounds);
		
		if(n_data_threads == 0){
			Error("Check_Request: did not find any Data_set");
			m3l_DestroyFound(&DATA_SFounds);
		}
	}
	else
	{
		printf("Check_Request: did not find any Data_set\n");
		exit(0);
	}
/*
 * loop over and find out if the process thread is free to use
 */
	for(i=0; i< n_data_threads; i++){
/*
 * find name of data thread and compare it to tested data thread name
 */
// 		if( (THRName_SFounds = m3l_Locate(m3l_get_Found_node(DATA_SFounds, i), "./Channel/Name_of_Channel", "/*/*", (lmchar_t *)NULL)) != NULL){
		if( (THRName_SFounds = m3l_locator_caller(m3l_get_Found_node(DATA_SFounds, i), "./Channel/Name_of_Channel", "/*/*",  Popts)) != NULL){

			if(n_data_threads == 0){
				Error("Check_Request: did not find any Name_of_Channel");
				m3l_DestroyFound(&THRName_SFounds);
				return -1;
			}
		}
		else
		{
			printf("Check_Request: did not find any Name_of_Channel\n");
			m3l_DestroyFound(&THRName_SFounds);
			return -1;
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
			ident = 1;
/*
 * check status of the thread
 */
// 			if( (THRStat_SFounds = m3l_Locate(m3l_get_Found_node(DATA_SFounds, i), "./Channel/Thread_Status", "/*/*", (lmchar_t *)NULL)) != NULL){
			if( (THRStat_SFounds = m3l_locator_caller(m3l_get_Found_node(DATA_SFounds, i), "./Channel/Thread_Status", "/*/*",  Popts)) != NULL){

				if(n_data_threads == 0){
					Error("Check_Request: did not find any Thread_Status");
					m3l_DestroyFound(&THRStat_SFounds);
				}
			}
			else
			{
				printf("Check_Request: did not find any Thread_Status\n");
				m3l_DestroyFound(&THRStat_SFounds);
				return -1;
			}
		
			TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
			Thread_Status = *(lmint_t *)m3l_get_data_pointer(TmpNode);
			m3l_DestroyFound(&THRStat_SFounds);
/*
 * thread is already occupied, ie. all S and R requests arrived
 * The vales 1 is set in Data_Thread (the thread is blocked there) the value is set to 0 in SR_hub (the thread is unblocked after 
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
 */			else if( SR_mode == 'S'){
// 				if( (THRStat_SFounds = m3l_Locate(m3l_get_Found_node(DATA_SFounds, i), "./Channel/S_Status", "/*/*", (lmchar_t *)NULL)) != NULL){
				if( (THRStat_SFounds = m3l_locator_caller(m3l_get_Found_node(DATA_SFounds, i), "./Channel/S_Status", "/*/*", Popts)) != NULL){

					if(n_data_threads == 0){
						Error("Check_Request: did not find any S_Status");
						m3l_DestroyFound(&THRName_SFounds);
						m3l_DestroyFound(&THRStat_SFounds);
					}
				}
				else
				{
					printf("Check_Request: did not find any S_Status\n");
					status = -1;
				}

				TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
				if( (S_Status = *(lmint_t *)m3l_get_data_pointer(TmpNode))){
					status = 1;}

			}
			else if( SR_mode == 'R'){
/*
 * find actual number of Receiving processes
 */
// 				if( (THRStat_SFounds = m3l_Locate(m3l_get_Found_node(DATA_SFounds, i), "./Channel/R_Status", "/*/*", (lmchar_t *)NULL)) != NULL){
				if( (THRStat_SFounds = m3l_locator_caller(m3l_get_Found_node(DATA_SFounds, i), "./Channel/R_Status", "/*/*", Popts)) != NULL){

					if(n_data_threads == 0){
						Error("Check_Request: did not find any S_Status");
						m3l_DestroyFound(&THRName_SFounds);
						m3l_DestroyFound(&THRStat_SFounds);
					}
				}
				else
				{
					printf("Check_Request: did not find any S_Status\n");
					m3l_DestroyFound(&THRName_SFounds);
					m3l_DestroyFound(&THRStat_SFounds);
					exit(0);
				}
				TmpNode = m3l_get_Found_node(THRStat_SFounds, 0);
				R_Status = *(lmsize_t *)m3l_get_data_pointer(TmpNode);
				m3l_DestroyFound(&THRName_SFounds);
				m3l_DestroyFound(&THRStat_SFounds);
/*
 * find required number of Receiving_Processes
 */
// 				if( (THRStat_SFounds = m3l_Locate(m3l_get_Found_node(DATA_SFounds, i), "./Channel/Receiving_Processes", "/*/*", (lmchar_t *)NULL)) != NULL){
				if( (THRStat_SFounds = m3l_locator_caller(m3l_get_Found_node(DATA_SFounds, i), "./Channel/Receiving_Processes", "/*/*", Popts)) != NULL){

					if(n_data_threads == 0){
						Error("Check_Request: did not find any S_Status");
						m3l_DestroyFound(&THRName_SFounds);
						m3l_DestroyFound(&THRStat_SFounds);
					}
				}
				else
				{
					printf("Check_Request: did not find any S_Status\n");
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
/*
 * if ident == 0, no valid connection name was found
 */
	if(ident == 1)
		return status;
	else
		return -1;  
}
