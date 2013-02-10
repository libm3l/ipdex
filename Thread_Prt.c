
#include "libm3l.h"
#include "Server_Header.h"

#include "Thread_Prt.h"

void *Data_Threads(void *arg)
{
	data_thread_args_t *c = (data_thread_args_t *)arg;
	
	pthread_mutex_lock(c->plock);
	printf("In thread \n");	
	
	if(m3l_Cat(c->Node, "--all", "-P", "-L", "*", (lmchar_t *)NULL) != 0)
		Error("CatData");
	
	printf("Out of thread \n");	

// 	sleep(1);
	pthread_mutex_unlock(c->plock);
	
 	return NULL;
}