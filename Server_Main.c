#include "libm3l.h"
#include "Server_Header.h"



#include "SignalC.h"
#include "Data_Fork.h"
#include "Data_Thread.h"


// extern lmint_t optind;
// static lmint_t verbose_flag;

lmint_t main (int argc, char **argv){
/*
 * function is a main server function
 * it reads data in libm3l format defining
 * inter-process communication
 * spawns a child for each data set.
 */

	lmint_t c, portnum, status;
	lmint_t digit_optind = 0;
	lmchar_t *Filename=NULL;
	
	node_t *Gnode = NULL;
	data_thread_str_t *Data_Threads;
	
	lmsize_t i;
	find_t *SFounds;
	node_t *LocNode;
	
	char opt_s='\0';
	
	portnum = -1;
/*
 * get options 
 */
	while (1) {
		lmint_t this_option_optind = optind ? optind : 1;
		lmint_t option_index = 0;
		static struct option long_options[] = {
			{"port",    		required_argument, 	0, 'p' },
			{"help",     		no_argument,       	0, 'h' },
			{"input_file",     	required_argument,	0, 'i' },
			{"show_file",     	no_argument,       	0, 's' },
			{0,         0,                 0,  0 }
		};

		c = getopt_long(argc, argv, "hi:p:s:012?",long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
			case 0:
				printf("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
				printf("\n");
			break;

			case '0':
			case '1':
			case '2':
				if (digit_optind != 0 && digit_optind != this_option_optind)
					printf("digits occur in two different argv-elements.\n");
				digit_optind = this_option_optind;
				printf("option %c\n", c);
			break;
			
			case 'i':
				printf("option i with value '%s'\n", optarg);
				if ( (Filename = strdup(optarg)) == NULL)
					Perror("strdup");
			break;
			
			case 'p':
				printf("option p with value '%s'\n", optarg);
				portnum = atoi(optarg);
			break;
			case 's':
				opt_s = 'y';
			break;
			
			case '?':
			case 'h':
				printf("Help : \n");
			break;

			default:
				printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	if (optind < argc) {
		printf("non-option ARGV-elements: ");
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}
/*
 * check that file name is given
 */
	if(Filename == NULL){
		free(Filename);
		Error("Server: no definition file specified\n");
	}
/*
 * check that port number is given
 */	
	if(portnum < 1){
		free(Filename);
		Error("Server: no port specified\n");
	}	
/*
 * open definition file
 */
	if( (Gnode = m3l_Fread(Filename, (lmchar_t *)NULL))  == NULL){
		free(Filename);
		Error("Server: m3l_Fread");
	}
	free(Filename);
	
	
	
// //===========================================================
// 	
// 	if( (SFounds = m3l_Detach_List(&Gnode, "/COMM_DEF/Data_Sets/Data_Set", "/*/*/*", (lmchar_t *)NULL)) != NULL){
// 		for(i=0; i < m3l_get_Found_number(SFounds); i++){
// 			
// 			LocNode = m3l_get_Found_node(SFounds, i);
// 			
// 			if(m3l_Cat(LocNode, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
// 				Error("m3l_Cat");
// 			
// 			if(m3l_Umount(&LocNode) != 1)
// 				Error("m3l_Umount");	
// 		}
// 		m3l_DestroyFound(&SFounds);
// 	}
// 	
// 	if(m3l_Cat(Gnode, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
// 				Error("m3l_Cat");
// 	if(m3l_Umount(&Gnode) != 1)
// 		Error("m3l_Umount");	
// 	exit(0);
// //===========================================================


/*
 * if specified, write the file on screen
 */	
	if(opt_s == 'y'){
	
		if(m3l_Cat(Gnode, "--all", "-L", "-P", "*",   (lmchar_t *)NULL) != 0)
 	                   Error("CatData");
		
	}
/*
 * Ctrl C signal handler
 */
	signal(SIGINT, catch_int);
	
/*
 * SIGCHLD signal handler
 */    
	signal(SIGCHLD,sig_chld); 
// 	Data_Fork(Gnode);
	
	Data_Threads = Data_Thread(Gnode);
/* 
 * bind, listen socket
 */


	for(i=0; i< Data_Threads->n_data_threads; i++){
		pthread_join(Data_Threads->data_threads[i], NULL);
 		printf("thread %ld is finished\n", i);
	}
	
	free(Data_Threads->data_threads);
	free(Data_Threads);
	
	
/*
 * free borrowed memory
 */
	if( (c = m3l_Umount(&Gnode)) != 1)
		Perror("m3l_Umount");

	
// 	printf(" ------------------------------   Waiting for children \n");
// 	wait(&status);
// 	printf(" ------------------------------   DONE \n");

	printf(" ------------------------------   Exiting main function () \n");

	exit(EXIT_SUCCESS);
	
}