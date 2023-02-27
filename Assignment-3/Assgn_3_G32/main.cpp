#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <random>
#include <sys/wait.h>
#include <string.h>
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

void djikstra(g_node *gt, int v, int s, int d, FILE *f,int it) // finds the shortest path between 2 given nodes
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
    //fprintf(f, "--------------Iteration %d-------------------\n",it);
    for (int i = 0; i < path.size(); i++)
    {
        // cout << path[i] << " ";
        fprintf(f, "%d ", path[i]);
    }
    // cout << "\n";
    fprintf(f, "\n");
    // fclose(f);
}

void djikstra_optimize(g_node *gt, int v, int s, int d, FILE *f, int* start_dist,int it) // finds the shortest path between 2 given nodes
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
    //fprintf(f, "--------------Iteration %d-------------------\n",it);
    for (int i = 0; i < path.size(); i++)
    {
        // cout << path[i] << " ";
        fprintf(f, "%d ", path[i]);
    }
    // cout << "\n";
    fprintf(f, "\n");
    start_dist[d+s*max_nodes] = distance[d];
    start_dist[s+d*max_nodes] = distance[d];
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
    if (parent[d] == -1){
        return path;
    }
    for (int v = d; v != -1; v = parent[v])
    {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());
    return path;
}




vector<vector<int>> new_dist(int curr,g_node* g,int* start_dist,int nodes)
{
    //vector<int>visited(max_nodes,0);
    vector<int>dist(max_nodes,INT_MAX);
    vector<vector<int>>paths(nodes);
    for(int i=0;i<nodes;i++)
    {
        paths[i].push_back(i);
    }

    dist[curr]=0;
    //dfs(-1,curr,g,0,paths,visited,dist);
    for(int i = 0; i < nodes; i++)
    {
        if(i!=curr)
        {
            auto v=new_djikstra(g,nodes,curr,i);
            paths.push_back(v);
            dist[i] = v.size()-1;
        }
    }

    for(int i=0;i<nodes;i++)
    {
     start_dist[i+max_nodes*curr]=dist[i];
     start_dist[curr+i*max_nodes]=dist[i];
    }

    for(int i=0;i<nodes;i++)
    {
        for(int j=i+1;j<nodes;j++)
        {
            if(dist[i]+dist[j]<start_dist[i+j*max_nodes])
            {
                start_dist[i+j*max_nodes]=dist[i]+dist[j];
                vector<int>v (nodes+1);
                auto p = paths[i];
                reverse(p.begin(),p.end());
                v=p;
                v.pop_back();
                for(auto x:paths[j])
                {
                    v.push_back(x);
                }
                paths.push_back(v);
            }
        }
    }
    return paths;
}


void consumer_optimize(g_node *start_node, int count_nodes, int k, FILE *f, int optimize_flag, int* start_dist, int previous_count,int it)
{
  if(optimize_flag == 1) {
    for (int i = k; i < count_nodes; i += 10)
    {
        for (int j = 0; j < count_nodes; j++)
        {
            djikstra_optimize(start_node, count_nodes, i, j, f, start_dist,it); // calls djikstra for source node num i and destination node num j
        }
    }
   }
   else {
    vector<vector<int>> v;
    if((count_nodes-previous_count) == 0) return;
    for(int i = previous_count; i < count_nodes; i++){
        if(i%10==k)
        {
             v = new_dist(i,start_node,start_dist,count_nodes);//vector<vector<int>>
        }
    }
    fprintf(f, "--------------Iteration %d-------------------\n",it);
    for(auto x:v) {
    for (int i = 0; i < x.size(); i++)
    {
        fprintf(f, "%d ", x[i]);
    }
    if(x.size()!=0) fprintf(f, "\n");
   }
   }
}


void consumer(g_node *start_node, int count_nodes, int k, FILE *f,int it)
{
    fprintf(f, "--------------Iteration %d-------------------\n",it);
    for (int i = k; i < count_nodes; i += 10)
    {
        for (int j = 0; j < count_nodes; j++)
        {
            djikstra(start_node, count_nodes, i, j, f,it); // calls djikstra for source node num i and destination node num j
        }
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

int main(int argc, char* argv[])
{
    int optimize_flag = 0;
    for(int i=0;i<argc;i++)
    if(!strcmp(argv[i],"-optimize")) optimize_flag = 1;
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

    // Shared memory segment to store the already computed distances
    int shmid_dist = shmget(IPC_PRIVATE, max_nodes * max_nodes * sizeof(int), 0666 | IPC_CREAT);
    int *start_dist = (int*)shmat(shmid_dist, (void*)0, 0);
    for (int i = 0; i < max_nodes*max_nodes; ++i)
        start_dist[i] = 0;
    int pid;

    // using key shmid_nodes gives a segment having array of g_nodes
    // used key shmid gives a segment having the graph i.e linked list of nodes

    if((pid=fork())==0)
    {
        //This is the producer

        g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
        node *ptr=(node *)shmat(shmid, (void *)0, 0);
        int num_nodes=0;
        g_node *gt=start_node;
        while(gt[num_nodes].head!=NULL)++num_nodes;
        node *start=ptr;
        while(ptr->vertex!=-1)++ptr;
        // cout<<ptr->vertex<<endl;
        node temp;
        int m;
        while(1)
        {
           m = (rand() %(30 - 10 + 1)) + 10;
           for(int i=0;i<m;++i)
           {
               temp.vertex=num_nodes+i;
               temp.next=NULL;
               *ptr=temp;
               gt[num_nodes+i].head=ptr;
               gt[num_nodes+i].tail=ptr;
               ++ptr;
           }
           num_nodes+=m;
           for(int i=0;i<m;++i)
           {
             ptr=add_edges(num_nodes-m+i,ptr,start_node);

           }
           sleep(50);
        }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
    }

    ////////////////////////////////////////// CONSUMER //////////////////////////////////////////

    if ((pid = fork()) == 0)
    {
        // consumer process
        int previous_count = 0;
        int it=1;
        while (1)
        {
    
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)  // to count the number of nodes
                ++count_nodes;
            FILE *fp = fopen("output0.txt", "a"); // opens up the file in which we have to append the paths
            if(optimize_flag){
                int *start_dist = (int*)shmat(shmid_dist, (void*)0, 0);
                consumer_optimize(start_node, count_nodes, 0, fp, optimize_flag, start_dist, previous_count,it);
                optimize_flag++; 
            }
            else {
              consumer(start_node, count_nodes, 0, fp,it); // calls consumer function
            }
            fclose(fp); 
            sleep(30);
            ++it;
            previous_count = count_nodes;
        }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
    }
    // same process is repeated 9 times for total of 10 consumers
    if ((pid = fork()) == 0)
    {
        // consumer process
        int previous_count = 0;
        int it=1;
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output1.txt", "a");
            if(optimize_flag){
                int *start_dist = (int*)shmat(shmid_dist, (void*)0, 0);
                consumer_optimize(start_node, count_nodes, 1, fp, optimize_flag, start_dist, previous_count,it);
                optimize_flag++; 
            }
            else {
              consumer(start_node, count_nodes, 1, fp,it); // calls consumer function
            }
            fclose(fp);
            sleep(30);
            ++it;
            previous_count = count_nodes;
        }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        int previous_count = 0;
        int it=1;
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output2.txt", "a");
            if(optimize_flag){
                int *start_dist = (int*)shmat(shmid_dist, (void*)0, 0);
                consumer_optimize(start_node, count_nodes, 2, fp, optimize_flag, start_dist, previous_count,it);
                optimize_flag++; 
            }
            else {
              consumer(start_node, count_nodes, 2, fp,it); // calls consumer function
            }
            fclose(fp);
            sleep(30);
            ++it;
            previous_count = count_nodes;
        }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        int previous_count = 0;
        int it=1;
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output3.txt", "a");
            if(optimize_flag){
                int *start_dist = (int*)shmat(shmid_dist, (void*)0, 0);
                consumer_optimize(start_node, count_nodes, 3, fp, optimize_flag, start_dist, previous_count,it);
                optimize_flag++; 
            }
            else {
              consumer(start_node, count_nodes, 3, fp,it); // calls consumer function
            }
            fclose(fp);
            sleep(30);
            ++it;
            previous_count = count_nodes;
        }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        int previous_count = 0;
        int it=1;
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output4.txt", "a");
            if(optimize_flag){
                int *start_dist = (int*)shmat(shmid_dist, (void*)0, 0);
                consumer_optimize(start_node, count_nodes, 4, fp, optimize_flag, start_dist, previous_count,it);
                optimize_flag++; 
            }
            else {
              consumer(start_node, count_nodes, 4, fp,it); // calls consumer function
            }
            fclose(fp);
            sleep(30);
            ++it;
            previous_count = count_nodes;
        }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        int previous_count = 0;
        int it=1;
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output5.txt", "a");
            if(optimize_flag){
                int *start_dist = (int*)shmat(shmid_dist, (void*)0, 0);
                consumer_optimize(start_node, count_nodes, 5, fp, optimize_flag, start_dist, previous_count,it);
                optimize_flag++; 
            }
            else {
              consumer(start_node, count_nodes, 5, fp,it); // calls consumer function
            }
            fclose(fp);
            sleep(30);
            ++it;
            previous_count = count_nodes;
        }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        int previous_count = 0;
        int it=1;
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)++count_nodes;
                ++count_nodes;
            FILE *fp = fopen("output6.txt", "a");
            if(optimize_flag){
                int *start_dist = (int*)shmat(shmid_dist, (void*)0, 0);
                consumer_optimize(start_node, count_nodes, 6, fp, optimize_flag, start_dist, previous_count,it);
                optimize_flag++; 
            }
            else {
              consumer(start_node, count_nodes, 6, fp,it); // calls consumer function
            }
            fclose(fp);
            sleep(30);
            ++it;
            previous_count = count_nodes;
        }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        int previous_count = 0;
        int it=1;
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output7.txt", "a");
            if(optimize_flag){
                int *start_dist = (int*)shmat(shmid_dist, (void*)0, 0);
                consumer_optimize(start_node, count_nodes, 7, fp, optimize_flag, start_dist, previous_count,it);
                optimize_flag++; 
            }
            else {
              consumer(start_node, count_nodes, 7, fp,it); // calls consumer function
            }
            fclose(fp);
            sleep(30);
            ++it;
            previous_count = count_nodes;
        }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        int previous_count = 0;
        int it=1;
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output8.txt", "a");
            if(optimize_flag){
                int *start_dist = (int*)shmat(shmid_dist, (void*)0, 0);
                consumer_optimize(start_node, count_nodes, 8, fp, optimize_flag, start_dist, previous_count,it);
                optimize_flag++; 
            }
            else {
              consumer(start_node, count_nodes, 8, fp,it); // calls consumer function
            }
            fclose(fp);
            sleep(30);
            ++it;
            previous_count = count_nodes;
        }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
    }
    if ((pid = fork()) == 0)
    {
        // consumer process
        int previous_count = 0;
        int it=1;
        while (1)
        {
            g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
            node *ptr = (node *)shmat(shmid, (void *)0, 0);
            int count_nodes = 0;
            g_node *gt = start_node;
            while (gt[count_nodes].head != NULL)
                ++count_nodes;
            FILE *fp = fopen("output9.txt", "a");
            if(optimize_flag){
                int *start_dist = (int*)shmat(shmid_dist, (void*)0, 0);
                consumer_optimize(start_node, count_nodes, 9, fp, optimize_flag, start_dist, previous_count,it);
                optimize_flag++; 
            }
            else {
              consumer(start_node, count_nodes, 9, fp,it); // calls consumer function
            }
            fclose(fp);
            sleep(30);
            ++it;
            previous_count = count_nodes;
        }
        shmdt(ptr);
        shmdt(start_node);
        exit(0);
    }
    ////////////////////////////////////////

    wait(NULL);
    shmdt(ptr);
    shmdt(start_node);
    shmctl(shmid, IPC_RMID, NULL);
    shmctl(shmid_nodes, IPC_RMID, NULL);
    return 0;
}
