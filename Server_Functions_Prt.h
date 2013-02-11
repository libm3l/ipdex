#ifndef __SERVER_FUNCTION_PRT_H__
#define __SERVER_FUNCTION_PRT_H__

extern void Pthread_barrier_init(pthread_barrier_t *, lmint_t);
extern void Pthread_barrier_destroy(pthread_barrier_t *);
extern void Pthread_barrier_wait(pthread_barrier_t *);

extern void Pthread_mutex_init(pthread_mutex_t *);
extern void Pthread_mutex_lock(pthread_mutex_t *);
extern void Pthread_mutex_unlock(pthread_mutex_t *);
extern void Pthread_mutex_destroy(pthread_mutex_t *);

extern void Pthread_cond_init(pthread_cond_t *);
extern void Pthread_cond_destroy(pthread_cond_t *);


#endif