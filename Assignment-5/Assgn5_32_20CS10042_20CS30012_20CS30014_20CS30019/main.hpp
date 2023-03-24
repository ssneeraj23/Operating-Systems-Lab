#ifndef _MAIN_header
#define _MAIN_header
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

extern int N, Y, X;
extern int *guests_priority;
extern room *all_rooms;
extern multiset<pair<int, int>, cmp> rooms_prio; // pair.first-> roomId, Pair.second-> priority
extern sem_t total_occupancy;
extern int uncleaned;
extern pthread_mutex_t unclnlock;
extern pthread_mutex_t roomlock;
extern pthread_cond_t clean_start;

extern void *guest(void *arg);
extern void *cleaner(void *arg);

#endif