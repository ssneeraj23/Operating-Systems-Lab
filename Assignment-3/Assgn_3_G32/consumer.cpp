#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <random>
#include <sys/wait.h>
using namespace std;

#define max_nodes 10000
#define max_edges 1000000

typedef struct node
{
    int vertex;
    struct node *next = NULL;
} node;
typedef struct g_node
{
    struct node *head = NULL;
    struct node *tail = NULL;
    int degree = 0;
} g_node;

node *add_edges(int v, node *ptr, g_node *gt)
{
    cout << "v is " << v << endl;
    int k = (rand() % (20 - 1 + 1)) + 1;
    vector<int> ve;
    int deg_sum = 0;
    for (int i = 0; i < v; ++i)
    {
        deg_sum += gt[i].degree;
        ve.push_back(deg_sum);
    }
    cout << "k is " << k << endl;
    node temp;
    for (int i = 0; i < k; ++i)
    {
        cout << "k is " << k << endl;
        int con = (rand() % (deg_sum - 1 + 1)) + 1;
        for (int j = 0; j < v; ++j)
        {
            // cout<<"here\n";
            if (ve[j] >= con)
            {
                con = j;
                break;
            }
        }
        temp.vertex = con;
        temp.next = NULL;
        *ptr = temp;
        gt[v].tail->next = ptr;
        gt[v].tail = ptr;
        ++gt[v].degree;
        ++ptr;
        temp.vertex = v;
        temp.next = NULL;
        *ptr = temp;
        gt[con].tail->next = ptr;
        gt[con].tail = ptr;
        ++gt[con].degree;
        ++ptr;
    }
    return ptr;
}

void djikstra(g_node *gt, int v, int s, int d, FILE *f) // finds the shortest path between 2 given nodes
{
    int distance[v];
    int seen[v];
    vector<int> parent(v + 1, -1);
    for (int i = 0; i < v; ++i)
    {
        distance[i] = INT_MAX;
        seen[i] = 0;
    }
    distance[s] = 0;
    int min = INT_MAX;
    int min_index;
    for (int i = 0; i < v; ++i)
    {
        min = INT_MAX;
        for (int j = 0; j < v; ++j)
        {
            if (seen[j] == 0 && distance[j] <= min)
            {
                min = distance[j];
                min_index = j;
                // parent[min_index]=j;
            }
        }
        seen[min_index] = 1;
        node *ptr = gt[min_index].head;
        while (ptr != NULL)
        {
            if (seen[ptr->vertex] == 0 && distance[ptr->vertex] > distance[min_index] + 1)
            {
                distance[ptr->vertex] = distance[min_index] + 1;
                parent[ptr->vertex] = min_index;
            }
            ptr = ptr->next;
        }
    }
    vector<int> path;
    if (parent[d] == -1)
        return;
    for (int v = d; v != -1; v = parent[v])
    {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());
    // string filename = "output" + to_string(s % 10) + ".txt";
    // FILE * f = fopen(filename, "a");
    for (int i = 0; i < path.size(); i++)
    {
        // cout << path[i] << " ";
        fprintf(f, "%d ", path[i]);
    }
    // cout << "\n";
    fprintf(f, "\n");
    // fclose(f);
}

void djikstra_optimize(g_node *gt, int v, int s, int d, FILE *f, int *start_dist) // finds the shortest path between 2 given nodes
{
    int distance[v];
    int seen[v];
    vector<int> parent(v + 1, -1);
    for (int i = 0; i < v; ++i)
    {
        distance[i] = INT_MAX;
        seen[i] = 0;
    }
    distance[s] = 0;
    int min = INT_MAX;
    int min_index;
    for (int i = 0; i < v; ++i)
    {
        min = INT_MAX;
        for (int j = 0; j < v; ++j)
        {
            if (seen[j] == 0 && distance[j] <= min)
            {
                min = distance[j];
                min_index = j;
                // parent[min_index]=j;
            }
        }
        seen[min_index] = 1;
        node *ptr = gt[min_index].head;
        while (ptr != NULL)
        {
            if (seen[ptr->vertex] == 0 && distance[ptr->vertex] > distance[min_index] + 1)
            {
                distance[ptr->vertex] = distance[min_index] + 1;
                parent[ptr->vertex] = min_index;
            }
            ptr = ptr->next;
        }
    }
    vector<int> path;
    if (parent[d] == -1)
        return;
    for (int v = d; v != -1; v = parent[v])
    {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());
    // string filename = "output" + to_string(s % 10) + ".txt";
    // FILE * f = fopen(filename, "a");
    for (int i = 0; i < path.size(); i++)
    {
        // cout << path[i] << " ";
        fprintf(f, "%d ", path[i]);
    }
    // cout << "\n";
    fprintf(f, "\n");
    start_dist[d + s * max_nodes] = distance[d];
    start_dist[s + d * max_nodes] = distance[d];
    // fclose(f);
}

vector<int> new_djikstra(g_node *gt, int v, int s, int d) // finds the shortest path between 2 given nodes
{
    int distance[v];
    int seen[v];
    vector<int> parent(v + 1, -1);
    for (int i = 0; i < v; ++i)
    {
        distance[i] = INT_MAX;
        seen[i] = 0;
    }
    distance[s] = 0;
    int min = INT_MAX;
    int min_index;
    for (int i = 0; i < v; ++i)
    {
        min = INT_MAX;
        for (int j = 0; j < v; ++j)
        {
            if (seen[j] == 0 && distance[j] <= min)
            {
                min = distance[j];
                min_index = j;
                // parent[min_index]=j;
            }
        }
        seen[min_index] = 1;
        node *ptr = gt[min_index].head;
        while (ptr != NULL)
        {
            if (seen[ptr->vertex] == 0 && distance[ptr->vertex] > distance[min_index] + 1)
            {
                distance[ptr->vertex] = distance[min_index] + 1;
                parent[ptr->vertex] = min_index;
            }
            ptr = ptr->next;
        }
    }
    vector<int> path;
    if (parent[d] == -1)
    {
        return path;
    }
    for (int v = d; v != -1; v = parent[v])
    {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<vector<int>> new_dist(int curr, g_node *g, int *start_dist, int nodes)
{
    // vector<int>visited(max_nodes,0);
    vector<int> dist(max_nodes, INT_MAX);
    vector<vector<int>> paths(nodes);
    for (int i = 0; i < nodes; i++)
    {
        paths[i].push_back(i);
    }

    dist[curr] = 0;
    // dfs(-1,curr,g,0,paths,visited,dist);
    for (int i = 0; i < nodes; i++)
    {
        if (i != curr)
        {
            auto v = new_djikstra(g, nodes, curr, i);
            paths.push_back(v);
            dist[i] = v.size() - 1;
        }
    }

    for (int i = 0; i < nodes; i++)
    {
        start_dist[i + max_nodes * curr] = dist[i];
        start_dist[curr + i * max_nodes] = dist[i];
    }

    for (int i = 0; i < nodes; i++)
    {
        for (int j = i + 1; j < nodes; j++)
        {
            if (dist[i] + dist[j] < start_dist[i + j * max_nodes])
            {
                start_dist[i + j * max_nodes] = dist[i] + dist[j];
                vector<int> v(nodes + 1);
                auto p = paths[i];
                reverse(p.begin(), p.end());
                v = p;
                v.pop_back();
                for (auto x : paths[j])
                {
                    v.push_back(x);
                }
                paths.push_back(v);
            }
        }
    }
    return paths;
}

void consumer_optimize(g_node *start_node, int count_nodes, int k, FILE *f, int optimize_flag, int *start_dist, int previous_count)
{
    if (optimize_flag == 1)
    {
        for (int i = k; i < count_nodes; i += 10)
        {
            for (int j = 0; j < count_nodes; j++)
            {
                djikstra_optimize(start_node, count_nodes, i, j, f, start_dist); // calls djikstra for source node num i and destination node num j
            }
        }
    }
    else
    {
        vector<vector<int>> v;
        if ((count_nodes - previous_count) == 0)
            return;
        for (int i = previous_count; i < count_nodes; i++)
        {
            if (i % 10 == k)
            {
                v = new_dist(i, start_node, start_dist, count_nodes); // vector<vector<int>>
            }
        }
        for (auto x : v)
        {
            for (int i = 0; i < x.size(); i++)
            {
                fprintf(f, "%d ", x[i]);
            }
            if (x.size() != 0)
                fprintf(f, "\n");
        }
    }
}

void consumer(g_node *start_node, int count_nodes, int k, FILE *f)
{

    for (int i = k; i < count_nodes; i += 10)
    {
        for (int j = 0; j < count_nodes; j++)
        {
            djikstra(start_node, count_nodes, i, j, f); // calls djikstra for source node num i and destination node num j
        }
    }

    // fprintf(f, "\n");
}
int main(int argc, char *argv[])
{
    int optimize_flag = 0;
    for(int i=0;i<argc;i++)
    if(!strcmp(argv[i],"-optimize")) optimize_flag = 1;
    int shmid = atoi(argv[1]);
    int shmid_nodes = atoi(argv[2]);
    int shmid_dist=atoi(argv[3]);
    // using key shmid_nodes gives a segment having array of g_nodes
    // used key shmid gives a segment having the graph i.e linked list of nodes

    int previous_count = 0;
    g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
    node *ptr = (node *)shmat(shmid, (void *)0, 0);
    while (1)
    { 
       int count_nodes = 0;
        g_node *gt = start_node;
        while (gt[count_nodes].head != NULL)
            ++count_nodes;
        FILE *fp = fopen("output2.txt", "a");
        if (optimize_flag)
        {
            int *start_dist = (int *)shmat(shmid_dist, (void *)0, 0);
            consumer_optimize(start_node, count_nodes, 2, fp, optimize_flag, start_dist, previous_count);
            optimize_flag++;
        }
        else
        {
            consumer(start_node, count_nodes, 2, fp); // calls consumer function
        }
        fclose(fp);
        sleep(30);
        previous_count = count_nodes;
    }
    shmdt(ptr);
    shmdt(start_node);
    exit(0);
}
