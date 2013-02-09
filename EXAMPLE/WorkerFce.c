
#include "Local_Header.h"
#include "ThreadFce.h"


void counter_init(thread_args_t *c)
{
	c->count = 0;
	pthread_mutex_init(&c->lock, NULL);
	pthread_cond_init(&c->cond, NULL);
}

int counter_get(thread_args_t *c)
{
	pthread_mutex_lock(c->plock);
	int val = *c->pcount;
	pthread_mutex_unlock(c->plock);
	return val;
}

void counter_incr(thread_args_t *c)
{
	pthread_mutex_lock(c->plock);
	int val = *c->pcount;
	val = val + 1;
	*c->pcount = val;
	pthread_cond_broadcast(c->pcond);
	pthread_mutex_unlock(c->plock);
}

void counter_wait_threshold(thread_args_t *c, int threshold)
{
	
	pthread_mutex_lock(c->plock);

	while (*c->pcount < threshold)
		pthread_cond_wait(c->pcond, c->plock);
	

	sleep(1);
	// now c->count >= threshold

	pthread_mutex_unlock(c->plock);

}

