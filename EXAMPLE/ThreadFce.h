
#ifndef __THREADFCE_H_
#define __THREADFCE_H_

void counter_init(thread_args_t *c);
int  counter_get(thread_args_t *c);
void counter_incr(thread_args_t *c);
void counter_wait_threshold(thread_args_t *c, int threshold);

void *worker(void *arg);
void *worker2(void *arg);

#endif
