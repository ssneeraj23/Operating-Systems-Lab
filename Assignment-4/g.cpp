#include <iostream>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>

using namespace std;

#define node_cnt 500

pthread_mutex_t mutex_log = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_monitor = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // monitor queue pushing

typedef struct action
{
    int user_id;
    int action_id;
    int action_type;
    time_t timestamp;
    int p = -1;

} action;

class cmp
{
public:
    bool operator()(action &a1, action &a2)
    {
        if (a1.p == -1)
        {
            if (a1.timestamp < a2.timestamp)
                return true;
            else
                false;
        }
        if (a1.p > a2.p)
            return true;
        return false;
    }
};

typedef struct node
{
    queue<action> wall;
    priority_queue<action, vector<action>, cmp> feed;
    int priority; // 0 indicates chrono
    int like_cnt = 1;
    int post_cnt = 1;
    int comment_cnt = 1;

} node;

// Define all the global variables here

vector<vector<int>> adj(node_cnt);
vector<node> all_nodes(node_cnt);
vector<vector<int>> common_neigh(node_cnt);
queue<action> monitor;
FILE *log_file = fopen("sns.log", "w");

// Define funcitons here

int find_comm_neigh(int a, int b, vector<vector<int>> &g)
{
    if (a == b)
        return 0;
    unordered_set<int> s1, s2;
    for (auto x : g[a])
        s1.insert(x);
    for (auto x : g[b])
        s2.insert(x);
    int c = 0;
    for (auto x : s1)
    {
        if (s2.find(x) != s2.end())
            ++c;
    }
    return c;
}

int get_num_comm_neigh(int a, int b, vector<vector<int>> &g)
{
    if (a > b)
        swap(a, b);
    return g[a][b - a];
}
// pushUpdate threads
void* pushUpdate(void *arg)
{
    cout<<"inside pushupdate\n";
    while (true)
    {
        action i;
        int flag = 0; // to check if somehting has been popped out of monitor queue
        // push action of a node into all its neighbours
        pthread_mutex_lock(&mutex2);
        pthread_cond_wait(&cond, &mutex2); // to signal that monitor queue has been updated

        pthread_mutex_lock(&mutex_monitor); // to make the pop operation atomic
        if (!monitor.empty())
        {
            flag=1; // indicates that something has been popped out of monitor queue
            i = monitor.front();
            monitor.pop();
        }
        pthread_mutex_unlock(&mutex_monitor);

        pthread_mutex_lock(&mutex1); // to make adding actions into the feed_queue of neighbours atomic
        if (flag == 1)
        {
            action temp = i;
            for (auto j : adj[i.user_id])
            {
                // if node of id i.user_id has priority bit 1, then chrono_compare else priority_compare
                all_nodes[j].feed.push(temp);
                
                printf("Pushed action %d of user %d into feed queue of user %d\n", temp.action_id, temp.user_id, j);
                fprintf(log_file, "Pushed action %d of user %d into feed queue of user %d\n", temp.action_id, temp.user_id, j);
            }
        }
        // if time priority we can immediately signal else wait for all actions of a particular node to be pushed into feed queues

        pthread_mutex_unlock(&mutex1);

        pthread_mutex_unlock(&mutex2);
    }
    return NULL;
}
// User simulator threads

void *user_sim(void *arg)
{
    vector<int> generated;
    set<int> generated_set;
    srand(time(NULL));
    while (1)
    {
        generated.clear();
        generated_set.clear();
        while (generated_set.size() < 100)
        {
            cout << rand() % node_cnt << endl;
            generated_set.insert(rand() % node_cnt);
        }
        for (auto x : generated_set)
        {
            generated.push_back(x);
        }
        fprintf(log_file, "The random nodes that are selected are\n");
        printf("The random nodes that are selected are\n");
        for (auto x : generated_set)
        {
            fprintf(log_file, "%d\n", x);
            printf("%d\n", x);
        }
        for (auto x : generated)
        {
            int actions;
            if (adj[x].size() == 0)
                actions = 1;
            else
                actions = (int)(10.0 * (1.0 + log2(adj[x].size())));
            fprintf(log_file, "NodeID : %d   No of Actions generated : %d  Degree : %d\n", x, actions, (int)adj[x].size());
            fprintf(log_file, "The actions generated are \n");
            printf("NodeID : %d   No of Actions generated : %d  Degree : %d\n", x, actions, (int)adj[x].size());
            // printf("The actions generated are \n");
            int id1, id2, id3;
            id1 = all_nodes[x].like_cnt;
            id2 = all_nodes[x].post_cnt;
            id3 = all_nodes[x].comment_cnt;
            string atype;
            for (int i = 0; i < actions; i++)
            {
                action temp;
                temp.user_id = x;
                temp.action_type = rand() % 3;
                temp.timestamp = time(NULL);
                if (temp.action_type == 0)
                {
                    temp.action_id = id1;
                    ++id1;
                    atype = "like";
                }
                else
                {
                    if (temp.action_type == 1)
                    {
                        temp.action_id = id2;
                        ++id2;
                        atype = "post";
                    }
                    else
                    {
                        temp.action_id = id3;
                        ++id3;
                        atype = "comment";
                    }
                }
                // lock the file
                pthread_mutex_lock(&mutex_log);
                fprintf(log_file, "UserID: %d action_id: %d action_type: %s timestamp: %s", x, temp.action_id, atype.c_str(), ctime(&temp.timestamp));
                // printf("UserID: %d action_id: %d action_type: %s timestamp: ",x,temp.action_id,atype,ctime(&temp.timestamp));
                pthread_mutex_unlock(&mutex_log);

                // lock
                pthread_mutex_lock(&mutex_monitor);
                monitor.push(temp);
                pthread_cond_broadcast(&cond);
                pthread_mutex_unlock(&mutex_monitor);

                all_nodes[x].wall.push(temp);
            }
            all_nodes[x].like_cnt = id1;
            all_nodes[x].post_cnt = id2;
            all_nodes[x].comment_cnt = id3;
        }
        sleep(120);
    }
    return NULL;
}
int main()
{
    int pos, n1, n2;
    string line;
    ifstream inFile("temp.txt");
    getline(inFile, line);

    // Loading the graph
    while (getline(inFile, line))
    {
        pos = (int)line.find(',');
        n1 = stoi(line.substr(0, pos));
        n2 = stoi(line.substr(pos + 1, line.length() - pos - 1));
        adj[n1].push_back(n2);
        adj[n2].push_back(n1);
    }
    inFile.close();

    // cout<<"File read\n";
    //  set random priority values to nodes

    srand(time(NULL));
    for (auto &x : all_nodes)
    {
        x.priority = rand() % 2;
    }

    // cout<<"Priorities set\n";
    //  finding number of common neighbours

    for (int i = 0; i < node_cnt; ++i)
    {
        for (int j = i; j < node_cnt; ++j)
        {
            common_neigh[i].push_back(find_comm_neigh(i, j, adj));
        }
    }

    // cout<<"Comm neighb found\n";

    pthread_t us_sim_thrd;

    // Create thread 1
    pthread_create(&us_sim_thrd, NULL, user_sim, NULL);
    // creating 25 threads of pushUpdate
    pthread_t push_update[25];
    for (int i = 0; i < 25; i++)
    {
        pthread_create(push_update, NULL, pushUpdate, NULL);
    }




    //To be at the end
    for (int i = 0; i < 25; i++)
    {
        pthread_join(push_update[i],NULL);
    }
     pthread_join(us_sim_thrd, NULL);
    return 0;
}