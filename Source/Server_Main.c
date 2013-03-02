#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"



#include "SignalC.h"
#include "Data_Fork.h"
// #include "Data_Thread.h"
#include "Server_Body.h"


lmint_t main (int argc, char **argv){
/*
 * function is a main server function
 * it reads data in libm3l format defining
 * inter-process communication
 * spawns a child for each data set.
 */

	lmint_t c, portnum, status, j;
	lmint_t digit_optind = 0;
	lmchar_t *Filename=NULL;
	
	node_t *Gnode = NULL;	
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


	for(j=0; j<10; j++){

printf("   \n\n  CYCLE       %d\n\n ", j);

	if( (Gnode = m3l_Fread(Filename, (lmchar_t *)NULL))  == NULL){
		free(Filename);
		Error("Server: m3l_Fread");
	}
// 	free(Filename);
/*
 * if specified, write the file on screen
 */	
	if(opt_s == 'y'){
 		if(m3l_Cat(Gnode, "--all", "-L", "-P", "*",   (lmchar_t *)NULL) != 0)
 			Warning("CatData");
	}
/*
 * Ctrl C signal handler
 */
// 	signal(SIGINT, catch_int);
/*
 * SIGCHLD signal handler
 */    
// 	signal(SIGCHLD,sig_chld); 
// 	Data_Fork(Gnode);
	
	
	Server_Body(Gnode);
/*
 * free borrowed memory
 */
	if( (c = m3l_Umount(&Gnode)) != 1)
		Perror("m3l_Umount");


	}

	
// 	printf(" ------------------------------   Waiting for children \n");
// 	wait(&status);
// 	printf(" ------------------------------   DONE \n");

	printf(" ------------------------------   Exiting main function () \n");

	exit(EXIT_SUCCESS);
	
}