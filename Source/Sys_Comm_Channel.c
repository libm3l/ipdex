
#include "libm3l.h"
#include "Sys_Comm_Channel.h"


lmint_t Sys_Comm_Channel()
{
/*
 * function handles communication between client and server via SYSTEM_COMM channel
 * the primary task is to increase/dicrease a number of communication channels,  number of 
 * S_R threads and character (KA mode, ATDT mode) for a specified channel
 */
// 	if( (SFounds = m3l_Locate(Gnode, "/Buffer/Channel", "/*/*", (lmchar_t *)NULL)) != NULL){
// 		
// 		Data_Thread->n_data_threads = m3l_get_Found_number(SFounds);
// 		
// 		if(Data_Thread->n_data_threads == 0){
// 			Error("Server: did not find any Data_set");
// 			m3l_DestroyFound(&SFounds);
// 		}
// 	}
// 	else
// 	{
// 		printf("Server: did not find any Data_set\n");
// 		exit(0);
// 	}
// 	
// 	
// 	
// 	
// 	
// 	
// 			if( (SFounds = m3l_Locate(c->Node, "/Buffer/Channel/Name_of_Channel", "./*/*",  (lmchar_t *)NULL)) != NULL){
// 			
// 			if( m3l_get_Found_number(SFounds) != 1)
// 				Error("Data_Thread: Only one Name_of_Channel per Channel allowed");
// /* 
//  * pointer to list of found nodes
//  */
// 				if( (List = m3l_get_Found_node(SFounds, 0)) == NULL)
// 					Error("Data_Thread: NULL Name_of_Channel");
// 			
// 				data_set_name = m3l_get_data_pointer(List);
// 				if( (len = m3l_get_List_totdim(List)-1) < 1)
// 					Error("Data_Thread: too short name of data set");
// 				if( snprintf(local_set_name, MAX_NAME_LENGTH,"%s",data_set_name) < 0)
// 					Perror("snprintf");
// 				local_set_name[len] ='\0';
// /* 
//  * free memory allocated in m3l_Locate
//  */
// 			m3l_DestroyFound(&SFounds);
// 		}
// 		else
// 		{
// 			Error("Data_Thread: Name_of_Channel not found\n");
// 		}
}