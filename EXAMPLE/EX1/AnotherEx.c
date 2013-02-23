// manager thread thread

pthread_cond_t pseudo_barrier;
pthread_cond_t pseudo_barrier_complete_cond;
pthread_mutex_t pseudo_barrier_mux;
int pseudo_barrier_counter = NUM_THREADS;
int pseudo_barrier_complete_flag = 0;

void thread_manager(void) 
{
    pthread_cond_init( &pseudo_barrier, NULL);
    pthread_cond_init( &pseudo_barrier_complete_cond, NULL);
    pthread_mutex_init( &pseudo_barrier_mux, NULL);


    for (int i = 0 ; i < NUM_THREADS; ++i) {
        pthread_create( /*... */);
    }

    // wait for threads to 'stage'
    pthread_mutex_lock( &pseudo_barrier_mux);
    while (pseudo_barrier_counter != 0) {
        pthread_cond_wait( &pseudo_barrier, &pseudo_barrier_mux);
    }
    pthread_mutex_unlock( &pseudo_barrier_mux);


    // at this point, all threads have either bailed out or are waiting to go
    // let 'em rip

    pthread_mutex_lock( &pseudo_barrier_mux);
    pseudo_barrier_complete_flag = 1;
    pthread_mutex_unlock( &pseudo_barrier_mux);
    pthread_cond_broadcast( &pseudo_barrier_complete_cond);

    // do whatever else the manager thread needs to do...
}


// worker threads
void* worker_thread(void* context)
{
    int error_result = 0;

    // whatever initialization...
    //  if this thread is going to bail out due to an error, it needs to 
    //  set the `error_result` value appropriately and still drop into the 
    //  following code

    // let the manager know that this thread is waiting (or isn't going to participate)
    pthread_mutex_lock( &pseudo_barrier_mux);
    --pseudo_barrier_counter;

    if (pseudo_barrier_counter == 0) {
        // all other threads are accounted for, let the manager know we're ready
        pthread_cond_signal( &pseudo_barrier);
    }

    // if this thread isn't going to contine because of some error, it's already 
    //  accounted for that fact in the `my_barrier_count`, so we can return here
    //  without preventing the pseudo-barrier from being met.
    if (some_error_occurred) {
        pthread_mutex_lock( &pseudo_barrier_mux);
        return NULL;
    }

    // NOTE: we're still holding pseudo_barrier_mux, so the master thread is still 
    //  blocked, even if we've signaled it - it'll jhave to wait until this 
    //  thread is blocking on `pseudo_barrier_complete_cond`

    while (!pseudo_barrier_complete_flag) {
        pthread_cond_wait( &pseudo_barrier_complete_cond, &pseudo_barrier_mux);
    }
    pthread_mutex_unlock( &pseudo_barrier_mux);


    // do the work...
}