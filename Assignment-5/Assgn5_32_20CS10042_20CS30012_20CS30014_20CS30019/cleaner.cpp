#include "main.hpp"

void *cleaner(void *arg)
{
    int r;
    int st;
    int cleaner_id = *((int *)arg);
    pthread_mutex_t dmut = PTHREAD_MUTEX_INITIALIZER;
    while (1)
    {
        pthread_mutex_lock(&dmut);
        pthread_cond_wait(&clean_start, &dmut);
        pthread_mutex_unlock(&dmut);
        while (1)
        {
            r = rand() % N;
            // get lock for uncleaned
            pthread_mutex_lock(&unclnlock);
            if (uncleaned != 0)
            {
                // release uncleaned lock
                pthread_mutex_unlock(&unclnlock);
                pthread_mutex_lock(&roomlock);
                if (all_rooms[r].g_count != 0)
                {
                    if (all_rooms[r].current_guest_priority == -1)
                    {
                        // room is empty
                        st = all_rooms[r].timestayed[1] + all_rooms[r].timestayed[0];
                        all_rooms[r].g_count = 0;

                        printf("\nCleaner %d cleans room %d for the time %d seconds\n", cleaner_id, r, st);
                        // release lock
                        pthread_mutex_unlock(&roomlock);
                        sleep(st);
                    }
                    else
                    {
                        // wake the guest thread
                        st = all_rooms[r].timestayed[1] + all_rooms[r].timestayed[0];
                        all_rooms[r].g_count = 0;

                        printf("\nCleaner %d cleans room %d for the time %d seconds\n", cleaner_id, r, st);
                        pthread_mutex_unlock(&roomlock);
                        sleep(st);
                    }
                    // get lock for uncleaned
                    pthread_mutex_lock(&unclnlock);
                    if (uncleaned == 0)
                    {
                        cout<<"uncleaned is "<<uncleaned<<endl;
                        pthread_mutex_unlock(&unclnlock);
                        // release lock
                        break;
                    }
                    else
                    {
                        if (uncleaned == 1)
                        {
                            --uncleaned;
                            cout<<"uncleaned is "<<uncleaned<<endl;
                            // release lock
                            for(int i=0;i<2*N;++i)sem_post(&total_occupancy);
                            pthread_mutex_unlock(&unclnlock);
                            break;
                        }
                        --uncleaned;
                        cout<<"uncleaned is "<<uncleaned<<endl;
                        pthread_mutex_unlock(&unclnlock);
                    }
    
                }
                else {
                    pthread_mutex_unlock(&roomlock);
                }
            }
            else
            {
                pthread_mutex_unlock(&unclnlock);
                // release lock
                break;
            }
        }
    }

    return NULL;
}
