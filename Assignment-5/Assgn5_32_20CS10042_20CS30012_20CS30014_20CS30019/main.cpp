#include "main.hpp"

int N, Y, X;
int *guests_priority;
room *all_rooms;
multiset<pair<int, int>, cmp> rooms_prio; // pair.first-> roomId, Pair.second-> priority
sem_t total_occupancy;
int uncleaned;
pthread_mutex_t unclnlock = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t roomlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clean_start=PTHREAD_COND_INITIALIZER;

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