
#include "libm3l.h"
#include "Server_Header.h"
#include "Server_Functions_Prt.h"
#include "IO_Functions.h"

extern lmint_t optind;
static lmint_t verbose_flag;

lmint_t Send_Data(node_t *Gnode, const lmchar_t *hostname, lmint_t portnumber, lmchar_t * Options, ...)
{
	opts_t *Popts, opts;
	va_list args;
	lmint_t option_index, c, portnr;
	size_t args_num, len, i;
	lmchar_t *word, **opt, *search_term, *search_term1;
	
	option_index = 0;
/*
 * get number of options
 */	
	if(Options == NULL)
		return -1;

	va_start(args, Options);
	args_num = 1;
	len = strlen(Options);

	while((word = va_arg(args, lmchar_t *)) != NULL){
		args_num ++;
	}
	va_end(args);
/*
 * get the values of option, for that, allocate opts ** array
 */
	if(args_num > 1){

		if ( (opt = (lmchar_t**)malloc( (args_num+1)*sizeof(lmchar_t *) )) == NULL)
			Perror("malloc");
/*
 * get the value of the first argument
 */
		va_start(args, Options);
/*
 * array member [0] will be empty
 */
		if ( (opt[0] = (lmchar_t *)malloc( sizeof(lmchar_t) )) == NULL)
				Perror("malloc");
	
 		len = strlen(Options);	
		if ( (opt[1] = (lmchar_t *)malloc( (len+1)*sizeof(lmchar_t) )) == NULL)
				Perror("malloc");
		strncpy(opt[1], Options, len);
		opt[1][len] = '\0';
/*
 * get the value of other arguments
 */	
		for(i=2; i<args_num; i++){
			word = va_arg(args, lmchar_t *);
			len = strlen(word);
			if ( (opt[i] = (lmchar_t *)malloc( (len+1)*sizeof(lmchar_t) )) == NULL)
				Perror("malloc");
			strncpy(opt[i], word, len);
			opt[i][len] = '\0';
		}
/*
 * get the name to look for
 */
 		search_term1 = va_arg(args, lmchar_t *);
		if ( (search_term = strdup(search_term1)) == NULL)
			Perror("strdup");
/*
 * end args
 */
		va_end(args);
/*
 * get meaning of options
 * first - reset opting = 0 to reinitialize getopt_long
 */
		optind = 0;
		while (1)
		{
			static struct option long_options[] =
			{
/*
 * options for cat_list
 */
				{"server",        	no_argument,       0, 's'}, /* server name */
				{"port",   		no_argument,       0, 'p'}, /* port number */
				{0, 0, 0, 0}
			};
 /*
  * getopt_long stores the option index here. 
  */
			c = getopt_long (args_num, opt, "sp", long_options, &option_index);
/*
 * Detect the end of the options 
 */
			if (c == -1)
				break;

			switch (c)
			{
				case 0:
 
					if (long_options[option_index].flag != 0)
						break;
			
					printf ("option %s", long_options[option_index].name);
					if (optarg)
						printf (" with arg %s", optarg);
					printf ("\n");
					break;

				case 'p':
/*
 * all
 */
					portnr = atoi(optarg);
				break;

				case '?':
/* 
 * Error, getopt_long already printed an error message
 */
				break;

				default:
				abort ();
			}
		}		
/*
 * free array opt **
 */
		for(i=0; i<args_num; i++)
			free(opt[i]);
		free(opt);
	}
	else
	{
/*
 * no additional options provided
 * get the value of the first argument, as not options are specified the argument is the name to look for
 */
// 		va_start(args, Options);
// 		if ( (search_term = strdup(Options)) == NULL)
// 			Perror("strdup");
// 		va_end(args);


		printf(" No options specified \n");
	}
/*
 * call find function with specified options
 */	
	Popts = &opts;
	
	ACKN = m3l_send_receive_tcpipsocket(HEADER .... )
	ACKN = m3l_send_to_tcpipsocket(Gnode .... )
	
	free(search_term);
	
}