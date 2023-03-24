#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

struct cmp
{
    bool operator()(const pair<int, int> &a, const pair<int, int> &b)const
    {
        return a.second < b.second;
    }
};

typedef struct
{
    int g_count = 0;
    int timestayed[2];
    int current_guest_priority = -1;
} room;

int N, Y, X;
int *guests_priority;
room *all_rooms;
multiset<pair<int, int>, cmp> rooms_prio; // pair.first-> roomId, Pair.second-> priority
sem_t total_occupancy;
int uncleaned;
pthread_mutex_t unclnlock = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t roomlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clean_start=PTHREAD_COND_INITIALIZER;

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

int main()
{
    uncleaned = 0;
    cout << "Enter Y:";
    cin >> Y;
    cout << "Enter N:";
    cin >> N;
    cout << "Enter X:";
    cin >> X;

    all_rooms = new room[N];
    for (int i = 0; i < N; ++i)
    {
        rooms_prio.insert({i, -1});
    }
    guests_priority = new int[Y];
    srand(time(NULL));
    int temp;
    set<int> s;
    sem_init(&total_occupancy, 0, 2 * N);
    for (int i = 0; i < Y; ++i)
    {

        while (1)
        {
            temp = rand() % Y + 1;
            if (s.find(temp) == s.end())
            {
                s.insert(temp);
                guests_priority[i] = temp;
                break;
            }
        }
    }
    // Creating guest threads
    vector<pthread_t> guest_thr(Y); // send args
    for (int i = 0; i < Y; i++)
        pthread_create(&guest_thr[i], NULL, guest, guests_priority + i);

    int* cleaner_ids;
    cleaner_ids = (int*)malloc(X*sizeof(int));
    for(int i = 0; i < X; i++)
     cleaner_ids[i] = i+1;

    // Creating cleaner threads
    vector<pthread_t> cleaner_thr(X);
    for (int i = 0; i < X; i++)
        pthread_create(&cleaner_thr[i], NULL, cleaner, cleaner_ids+i);

    for (int i = 0; i < Y; i++)
        pthread_join(guest_thr[i], NULL);
    for (int i = 0; i < X; i++)
        pthread_join(cleaner_thr[i], NULL);

    // free the malloced variables

    return 0;
}
