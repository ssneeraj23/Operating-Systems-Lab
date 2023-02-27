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

void djikstra(g_node *gt, int v, int s, FILE *f) // finds the shortest path between 2 given nodes
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
    for (int d = 0; d < v; ++d)
    {

        vector<int> path;
        if(d==s){
            continue;
        }
        if (parent[d] == -1)
            break;
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
    }
    // fclose(f);
}
void consumer(g_node *start_node, int count_nodes, int k, FILE *f)
{

    for (int i = k; i < count_nodes; i += 10)
    {
        /*for (int j = 0; j < count_nodes; j++)
        {
            djikstra(start_node, count_nodes, i, j, f); // calls djikstra for source node num i and destination node num j
        }*/
        djikstra(start_node, count_nodes, i, f); // calls djikstra for source node num i and all possible destinations
    }

    // fprintf(f, "\n");
}
node *add_edges(int v, node *ptr, g_node *gt)
{
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
        // cout<<"k is "<<k<<endl;
        int con = (rand() % (deg_sum - 1 + 1)) + 1;
        for (int j = 0; j < v; ++j)
        {
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

int main()
{
    int shmid = shmget(IPC_PRIVATE, max_edges * 2 * sizeof(node), 0666 | IPC_CREAT);
    // shmat to attach to shared memory
    node *ptr = (node *)shmat(shmid, (void *)0, 0);
    node *start = ptr;
    node temp;
    g_node all_nodes[max_nodes];
    int n1;
    int n2;
    FILE *f = fopen("facebook_combined.txt", "r");
    int k = 0;
    while (fscanf(f, "%d %d", &n1, &n2) > 0)
    {
        ++k;
        if (all_nodes[n1].head == NULL)
        {
            temp.vertex = n1;
            temp.next = NULL;
            *ptr = temp;
            all_nodes[n1].head = ptr;
            all_nodes[n1].tail = ptr;
            ++ptr;
        }
        if (all_nodes[n2].head == NULL)
        {
            temp.vertex = n2;
            temp.next = NULL;
            *ptr = temp;
            all_nodes[n2].head = ptr;
            all_nodes[n2].tail = ptr;
            ++ptr;
        }
        temp.vertex = n2;
        temp.next = NULL;
        *ptr = temp;
        all_nodes[n1].tail->next = ptr;
        all_nodes[n1].tail = ptr;
        ++all_nodes[n1].degree;
        ++ptr;
        temp.vertex = n1;
        temp.next = NULL;
        *ptr = temp;
        all_nodes[n2].tail->next = ptr;
        all_nodes[n2].tail = ptr;
        ++all_nodes[n2].degree;
        ++ptr;
    }
    fclose(f);
    temp.vertex = -1;
    temp.next = NULL;
    *ptr = temp;
    ++ptr;
    int shmid_nodes = shmget(IPC_PRIVATE, max_nodes * sizeof(g_node), 0666 | IPC_CREAT);
    g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
    for (int i = 0; i < max_nodes; ++i)
        start_node[i] = all_nodes[i];
    int pid;

    // using key shmid_nodes gives a segment having array of g_nodes
    // used key shmid gives a segment having the graph i.e linked list of nodes

    if ((pid = fork()) == 0)
    {

        cout << "I am the producer\n";
        g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
        node *ptr = (node *)shmat(shmid, (void *)0, 0);
        int num_nodes = 0;
        g_node *gt = start_node;
        while (gt[num_nodes].head != NULL)
            ++num_nodes;
        node *start = ptr;
        while (ptr->vertex != -1)
            ++ptr;
        cout << ptr->vertex << endl;
        cout << "Num nodes are " << num_nodes << endl;
        node temp;
        int m;
        while (1)
        {
            m = (rand() % (30 - 10 + 1)) + 10;
            for (int i = 0; i < m; ++i)
            {
                temp.vertex = num_nodes + i;
                temp.next = NULL;
                *ptr = temp;
                gt[num_nodes + i].head = ptr;
                gt[num_nodes + i].tail = ptr;
                ++ptr;
            }
            num_nodes += m;
            cout << "m is " << m << endl;
            for (int i = 0; i < m; ++i)
            {
                cout << "In for loop for adding edges\n";
                ptr = add_edges(num_nodes - m + i, ptr, start_node);
            }

            node *tt = gt[num_nodes - 1].head;
            cout << "the nodes connected to " << num_nodes - 1 << " are\n";
            while (tt != NULL)
            {
                cout << tt->vertex << endl;
                tt = tt->next;
            }
            sleep(50);
        }
    }
    ////////////////////////////////////////// CONSUMER //////////////////////////////////////////
    if ((pid = fork()) == 0)
    {
        // consumer process
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL) // to count the number of nodes
                ++count_nodes;
            FILE *fp = fopen("output0.txt", "w");     // opens up the file in which we have to append the paths
            consumer(start_node, count_nodes, 0, fp); // calls consumer function
            fclose(fp);
            sleep(30);
        }
    }
    // same process is repeated 9 times for total of 10 consumers
    if ((pid = fork()) == 0)
    {
        // consumer process
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output1.txt", "w");
            consumer(start_node, count_nodes, 1, fp);
            fclose(fp);
            sleep(30);
        }
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output2.txt", "w");
            consumer(start_node, count_nodes, 2, fp);
            fclose(fp);
            sleep(30);
        }
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output3.txt", "w");
            consumer(start_node, count_nodes, 3, fp);
            fclose(fp);
            sleep(30);
        }
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output4.txt", "w");
            consumer(start_node, count_nodes, 4, fp);
            fclose(fp);
            sleep(30);
        }
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output5.txt", "w");
            consumer(start_node, count_nodes, 5, fp);
            fclose(fp);
            sleep(30);
        }
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output6.txt", "w");
            consumer(start_node, count_nodes, 6, fp);
            fclose(fp);
            sleep(30);
        }
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output7.txt", "w");
            consumer(start_node, count_nodes, 7, fp);
            fclose(fp);
            sleep(30);
        }
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output8.txt", "w");
            consumer(start_node, count_nodes, 8, fp);
            fclose(fp);
            sleep(30);
        }
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output9.txt", "w");
            consumer(start_node, count_nodes, 9, fp);
            fclose(fp);
            sleep(30);
        }
    }
    ////////////////////////////////////////

    wait(NULL);
    shmdt(ptr);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
