#include "main.hpp"

void *guest(void *arg)
{
    int p = *((int *)arg); 
    pair<int, int> temp;
    srand(time(NULL));
    int smval;
    while (1)
    {
        cout << "\nEntered guest thread with priority " << p;
        int random_time = random() % 9 + 11;
        cout << ". This Guest sleeps for " << random_time << " seconds before requesting the room" << endl;
        sleep(random_time);
        printf("\nGuest with priority %d wakes up and requests for the room \n",p);
        // get lock for uncleaned
        pthread_mutex_lock(&unclnlock);
        if (uncleaned == 0)
        {
            sem_wait(&total_occupancy);
            sem_getvalue(&total_occupancy, &smval);
            if (smval != 0)
            {
                // release lock
                pthread_mutex_unlock(&unclnlock);
            }
            else
            {
                pthread_mutex_lock(&roomlock);
                auto it = rooms_prio.begin();
                temp = *it;
                rooms_prio.erase(it);
                if (temp.second == -1)
                {
                    temp.second = p + (all_rooms[temp.first].g_count) * Y;
                    rooms_prio.insert(temp);
                    ++all_rooms[temp.first].g_count;
                    // all_rooms guest priority under consideration
                    int sleep_time = rand() % 19 + 11;
                    all_rooms[temp.first].timestayed[all_rooms[temp.first].g_count - 1] = sleep_time;
                    printf("Guest with priority %d occupied the room %d. ", p, temp.first); 
                    printf("This guest sleeps here for %d seconds \n",sleep_time);

                    pthread_mutex_unlock(&roomlock);
                    uncleaned = N;
                    pthread_cond_broadcast(&clean_start);
                    pthread_mutex_unlock(&unclnlock);
                    sleep(sleep_time);
                    // wake frm sleep

                }
                else if (temp.second <= Y && p > temp.second)
                {
                    temp.second = p + (all_rooms[temp.first].g_count) * Y;
                    rooms_prio.insert(temp);
                    ++all_rooms[temp.first].g_count;
                    // all_rooms guest priority under consideration
                    int sleep_time = rand() % 19 + 11;
                    all_rooms[temp.first].timestayed[all_rooms[temp.first].g_count - 1] = sleep_time;

                    printf("Guest with priority %d occupied the room %d. ", p, temp.first);
                    printf("This guest sleeps here for %d seconds \n",sleep_time);

                    uncleaned = N;
                    pthread_cond_broadcast(&clean_start);
                    pthread_mutex_unlock(&unclnlock);
                    // release lock
                    // wakeup thread with priority temp.second from sleep
                    pthread_mutex_unlock(&roomlock);
                    sleep(sleep_time);
                }
                else
                {
                    pthread_mutex_unlock(&unclnlock);
                    sem_post(&total_occupancy);
                    pthread_mutex_unlock(&roomlock);
                }
                continue;
            }
            pthread_mutex_lock(&roomlock);
            auto it = rooms_prio.begin();
            temp = *it;
            rooms_prio.erase(it);
            if (temp.second == -1)
            {
                temp.second = p + (all_rooms[temp.first].g_count) * Y;
                rooms_prio.insert(temp);
                ++all_rooms[temp.first].g_count;
                // all_rooms guest priority under consideration
                int sleep_time = rand() % 19 + 11;
                all_rooms[temp.first].timestayed[all_rooms[temp.first].g_count - 1] = sleep_time;
                
                printf("Guest with priority %d occupied the room %d. ", p, temp.first);
                printf("This guest sleeps here for %d seconds \n",sleep_time);

                pthread_mutex_unlock(&roomlock);
                sleep(sleep_time);
                // wake frm sleep
                rooms_prio.erase(temp);
                temp.second = -1;
                rooms_prio.insert(temp);
            }
            else if (temp.second <= Y && p > temp.second)
            {
                temp.second = p + (all_rooms[temp.first].g_count) * Y;
                rooms_prio.insert(temp);
                ++all_rooms[temp.first].g_count;
                // all_rooms guest priority under consideration
                int sleep_time = rand() % 19 + 11;
                all_rooms[temp.first].timestayed[all_rooms[temp.first].g_count - 1] = sleep_time;
                
                printf("Guest with priority %d occupied the room %d. ", p, temp.first);
                printf("This guest sleeps here for %d seconds \n",sleep_time);

                // wakeup thread with priority temp.second from sleep
                pthread_mutex_unlock(&roomlock);
                sleep(sleep_time);
            }
            else
            {
                sem_post(&total_occupancy);
                pthread_mutex_unlock(&roomlock);
            }
        }
        else {
            cout << "Caution: Cleaning in progress. Room cannot be assigned." << endl;
            pthread_mutex_unlock(&unclnlock);
        }
    }
    return NULL;
}
