

#include "Header.h"
#include "libm3l.h"
#include "Data_Fork.h"

lmint_t Data_Fork(node_t *Gnode){

	lmsize_t i, n_forks;
	find_t *SFounds;
	node_t  *Tmp = NULL, *LocNode;
	pid_t childpid;
	opts_t *Popts, opts;
	
	opts.opt_i = '\0'; opts.opt_d = '\0'; opts.opt_f = '\0'; opts.opt_r = 'r'; opts.opt_I = '\0'; opts.opt_k = '\0'; opts.opt_b = '\0';opts.opt_l = '\0';
	opts.opt_add = '\0';

	if( (SFounds = m3l_Locate(Gnode, "/COMM_DEF/Data_Sets/Data_Set", "/*/*/*", (lmchar_t *)NULL)) != NULL){
//  	if( (SFounds = m3l_Detach_List(&Gnode, "/COMM_DEF/Data_Sets/Data_Set", "/*/*/*", (lmchar_t *)NULL)) != NULL){
		
		n_forks = m3l_get_Found_number(SFounds);
		
		if(n_forks == 0){
			Error("Server: did not find any Data_set");
			m3l_DestroyFound(&SFounds);
		}
		
// 			for(i=0; i < n_forks; i++){
// 				Tmp = m3l_get_Found_node(SFounds, i);
// 				printf(" \n\n\nFound name is %s  %p   %s\n", Tmp->name, Tmp, Tmp->type);
// 				
// 				 if(m3l_Cat(SFounds->Found_Nodes[i]->List, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
// 					Error("CatData");
// 			}
// 		m3l_DestroyFound(&SFounds);
	}
	else
	{
		printf("Server: did not find any Data_set\n");
		exit(0);
	}
	
	
	printf(" \n\n GNODE - after Detach \n");
	
	if(m3l_Cat(Gnode, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
		Error("CatData");
		
	for(i=0; i < n_forks; i++){
		
		printf(" Cycle is is %d\n", i);
/*
 * create a node
 */
		if(  (LocNode = m3l_Mklist("Local_Fork_Data", "DIR", 0, 0, (node_t **)NULL, (const char *)NULL, (const char *)NULL, (char *)NULL)) == 0)
			Perror("m3l_Mklist");
		Tmp = m3l_get_Found_node(SFounds, i);
				
		m3l_Mv(&Tmp, "./Data_Set", "./*", &LocNode, "/Local_Fork_Data", "/*", (lmchar_t *)NULL);   /* move node */
		
// 		if(m3l_Cat(LocNode, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
// 					Error("CatData");

// 		LocNode = m3l_get_Found_node(SFounds, i);
		printf("\n\nFork: \n");
		if(m3l_Cat(LocNode, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
			Error("CatData");
/*
 * spawn a child by forking
 */	
		childpid = Fork();
		if (childpid == 0) {
			
			printf(" child process %d - pid is %d \n", i, getpid());

/*
 * child, free unused data
 */	
			printf(" chld: umounting Gnode");
			if(m3l_Umount(&Gnode) != 1)
				Error("m3l_Umount");
			printf(" chld: umounting Gnode");

			m3l_DestroyFound(&SFounds);
/* 
 * cat LocNode
 */
// 		if(m3l_Cat(LocNode, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
// 			Error("CatData");
		
			if(m3l_Umount(&LocNode) != 1)
				Error("m3l_Umount");
			
			exit(0);
		}
		else
		{
			
			printf(" parent process - pid is %d \n",  getpid());

/*
 * parent
 */	
			if(m3l_Umount(&LocNode) != 1)
				Error("m3l_Umount");
		}
	}
		
	m3l_DestroyFound(&SFounds);
	
	printf(" \n\n GNODE \n");
	
	if(m3l_Cat(Gnode, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
		Error("CatData");
	
	sleep(2);
}