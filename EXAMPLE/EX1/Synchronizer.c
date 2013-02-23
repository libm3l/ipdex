#define SYNC_MAX_COUNT  10

void SynchronizationPoint()
{
        /* use static variables to ensure initialization */
        static mutex_t sync_lock = PTHREAD_MUTEX_INITIALIZER;
        static cond_t  sync_cond = PTHREAD_COND_INITIALIZER;
        static int sync_count = 0;

        /* lock the access to the count */
        pthread_mutex_lock(&sync_lock);

        /* increment the counter */
        sync_count++;

        /* check if we should wait or not */
        if (sync_count < SYNC_MAX_COUNT)

             /* wait for the others */
             pthread_cond_wait(&sync_cond, &sync_lock);

        else

            /* broadcast that everybody reached the point */
            pthread_cond_broadcast(&sync_cond);

        /* unlocks the mutex - otherwise only one thread
                will be able to return from the routine! */
        pthread_mutex_unlock(&sync_lock);
}