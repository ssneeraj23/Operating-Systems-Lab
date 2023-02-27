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
    cout<<"v is "<<v<<endl;
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
        cout<<"k is "<<k<<endl;
        int con = (rand() % (deg_sum - 1 + 1)) + 1;
        for (int j = 0; j < v; ++j)
        {
            //cout<<"here\n";
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

int main(int argc, char* argv[])
{

    int shmid=atoi(argv[1]);
    int shmid_nodes=atoi(argv[2]);
    // using key shmid_nodes gives a segment having array of g_nodes
    // used key shmid gives a segment having the graph i.e linked list of nodes
    //cout << "I am the producer\n";
    g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
    node *ptr = (node *)shmat(shmid, (void *)0, 0);
    int num_nodes = 0;
    g_node *gt = start_node;
    while (gt[num_nodes].head != NULL)
        ++num_nodes;
    node *start = ptr;
    while (ptr->vertex != -1)
        ++ptr;
    // cout << ptr->vertex << endl;
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
        for (int i = 0; i < m; ++i)
        {
            ptr = add_edges(num_nodes - m + i, ptr, start_node);
        }
        sleep(50);
    }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
}

