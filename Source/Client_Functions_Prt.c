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
 *     Function Client_Functions_Prt.c
 *
 *     Date: 2013-10-10
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
#include "Client_Functions_Prt.h"
#include "client_functions_Prt.h"


lmint_t Client_Sender(void *data, const lmchar_t *hostname, lmint_t portnumber, lmchar_t * Options, ...){
	
	lmchar_t *word, **opt;
	opts_t *Popts, opts;
	lmsize_t args_num, len, i, c;
	va_list args;
	lmint_t option_index;
	node_t *List;
	
	List = (node_t *)data;
	
	option_index = 0;

	opts.opt_i = '\0';
/*
 * check if data set exists
 */
	if(List == NULL){
		Warning("Client_Sender: NULL list");
		return -1;
	}
/*
 * get number of options
 */	
	if(Options != NULL){
		va_start(args, Options);
		args_num = 1;
		len = strlen(Options);

		while((word = va_arg(args, lmchar_t *)) != NULL){
			args_num++;
		}
		va_end(args);
		args_num++;
/*
 * get the values of option, for that, allocate opts ** array
 */
		if ( (opt = (lmchar_t**)malloc( (args_num)*sizeof(lmchar_t *) )) == NULL)
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
		opt[0][0]='\0';

 		len = strlen(Options);	
		if ( (opt[1] = (lmchar_t *)malloc( (len+1) * sizeof(lmchar_t ) )) == NULL)
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
				{"send",       	  no_argument,       0, 's'},   /* send */
				{"receive",       no_argument,       0, 'r'},   /* receive */
				{"send-receive",  no_argument,       0, 'a'},   /* send-receive */
				{"libm3l", 	  no_argument,       0, 'l'},   /* data structure type  */
				{"KeepAlive",     required_argument, 0, 'K'},
				{"ATDT",     	  required_argument, 0, 'A'},
				{"encoding",     required_argument,                  0, 'c'}, /* tcpip_socket_IOop.c - libm3l */
				{"buffering",    required_argument,           	     0, 'b'}, /* tcpip_socket_IOop.c - libm3l */
				{"re-open",    required_argument,           	     0, 'R'}, /* tcpip_socket_IOop.c - libm3l */
				{0, 0, 0, 0}
			};
 /*
  * getopt_long stores the option index here. 
  */
			c = getopt_long (args_num, opt, "aA:bclK:srR", long_options, &option_index);
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
				
				case 'b':
/*
 * choose how to copy data to IO buffer
 */
					if( strncmp(optarg, "MEMCPY", 6) == 0){
/*
 * MEMCPY
 */
						opts.opt_MEMCP = 'M';
					}
					else{
/*
 * STRNCPY
 */
						opts.opt_MEMCP = 'S';
					}
				break;
				
				case 'c':
/*
 * choose encoding and serialization
 */
					if( strncmp(optarg, "IEEE-754", 8) == 0){
/*
 * IEEE-754 encoding for numbers
 */
						opts.opt_tcpencoding = 'I';
					}
					else if(strncmp(optarg, "raw", 3) == 0){
/*
 * raw data sending
 */
						opts.opt_tcpencoding = 'r';
					}
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
/*
 * locate nodes using find function
 */
	Popts = &opts;
	
// 	if( client_sender(List, hostname, portnumber, Popts) < 0){
// 		return -1;
// 	}
// 	else{
// 		return 1;
// 	}
	
return 1;	
}





lmint_t Client_Receiver(node_t *List, const lmchar_t *hostname, lmint_t portnumber, lmchar_t * Options, ...){
	
	lmchar_t *word, **opt;
	opts_t *Popts, opts;
	lmsize_t args_num, len, i, c;
	va_list args;
	lmint_t option_index;
	
	option_index = 0;
	
	opts.opt_i = '\0';
/*
 * check if data set exists
 */
	if(List == NULL){
		Warning("Client_Sender: NULL list");
		return -1;
	}
/*
 * get number of options
 */	
	if(Options != NULL){
		va_start(args, Options);
		args_num = 1;
		len = strlen(Options);

		while((word = va_arg(args, lmchar_t *)) != NULL){
			args_num++;
		}
		va_end(args);
		args_num++;
/*
 * get the values of option, for that, allocate opts ** array
 */
		if ( (opt = (lmchar_t**)malloc( (args_num)*sizeof(lmchar_t *) )) == NULL)
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
		opt[0][0]='\0';

 		len = strlen(Options);	
		if ( (opt[1] = (lmchar_t *)malloc( (len+1) * sizeof(lmchar_t ) )) == NULL)
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
				{"send",       	  no_argument,       0, 's'},   /* send */
				{"receive",       no_argument,       0, 'r'},   /* receive */
				{"send-receive",  no_argument,       0, 'a'},   /* send-receive */
				{"libm3l", 	  no_argument,       0, 'l'},   /* data structure type  */
				{"KeepAlive",     required_argument, 0, 'k'},
				{"ATDT",     	  required_argument, 0, 't'},
				{0, 0, 0, 0}
			};
 /*
  * getopt_long stores the option index here. 
  */
			c = getopt_long (args_num, opt, "cdfiIklr", long_options, &option_index);
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

				case 'c':
/*
 * for LINK: if link is being removed, the algorithm searches in the target of the link
 * and set the linked list to NULL, rather then free the memory
 * it is faster but can allocate memory of used way too much
 * to clean it, specify c
 */
					opts.opt_linkscleanemptrefs = 'c';
				break;
				
				case 'i':
/*
 * ignore case
 */
					opts.opt_i = 'i';
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
/*
 * locate nodes using find function
 */
	Popts = &opts;
	
// 	if( client_sender(List, hostname, portnumber, Popts) < 0){
// 		return -1;
// 	}
// 	else{
// 		return 1;
// 	}
	
return 1;	
}