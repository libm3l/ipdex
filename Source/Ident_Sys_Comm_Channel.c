

#include "libm3l.h"
#include "lsipdx_header.h"
#include "Ident_Sys_Comm_Channel.h"
// #include "Server_Functions_Prt.h"


lmint_t Ident_Sys_Comm_Channel(node_t *RecNode, data_thread_str_t *Data_Threads)
{
/*
 * function handles communication between client and server via SERVER_SYS_LINK channel.
 * This channel is used to process requests sent by clients to the server.
 * Some of the stask managen through this channel are
 * 	-	add/remove data channel
 * 	-	leave Data_Thread (SR_Hub) for KA = Y
 * 	-	increase/decrease number of S_R threads for a particular Data_Thread
 * 	-	change KA and/or ATDRT mode
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