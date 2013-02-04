#include "Header.h"
#include "libm3l.h"
#include <pthread.h>

extern lmint_t optind;
static lmint_t verbose_flag;

lmint_t main (int argc, char ** argv){
/*
 * function is a main server function
 * it reads data in libm3l format defining
 * inter-process communication
 * spawns a child for each data set
 */
lmint_t count;
  
      for (count = 1; count < argc; count++)
	{
	  printf("argv[%d] = %s\n", count, argv[count]);
	}
    

// 	optind = 0;
// 		while (1)
// 		{
// 			static struct option long_options[] =
// 			{
// 				{"port",     	no_argument,    	0, 'p'},  /* port number */
// 				{0, 0, 0, 0}
// 			}; 
//  /*
//   * getopt_long stores the option index here. 
//   */
//  			c = getopt_long (args_num, opt, "abc", long_options, &option_index);
// /*
//  * Detect the end of the options 
//  */
// 			if (c == -1)
// 				break;
// 
// 			switch (c)
// 			{
// 				case 0:
//  
// 					if (long_options[option_index].flag != 0)
// 						break;
// 			
// 					printf ("option %s", long_options[option_index].name);
// 					if (optarg)
// 						printf (" with arg %s", optarg);
// 					printf ("\n");
// 					break;
// 
// 				case 'i':
// /*
//  * ignore case
//  */
// 					opts.opt_i = 'i';
// 			}
// 		}
	
}