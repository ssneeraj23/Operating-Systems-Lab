#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include<unistd.h>
#include<bits/stdc++.h>
#include <random>
#include <sys/wait.h>
using namespace std;
#define max_nodes 10000
#define max_edges 1000000

typedef struct node 
{
    int vertex;
    struct node * next=NULL;
} node;
typedef struct g_node
{
    struct node * head=NULL;
    struct node *tail=NULL;
    int degree=0;
} g_node;


node* add_edges(int v,node *ptr,g_node *gt)
{
     int  k = (rand() %(20 - 1 + 1)) + 1;
     vector<int> ve;
     int deg_sum=0;
     for(int i=0;i<v;++i)
     {
        deg_sum+=gt[i].degree;
        ve.push_back(deg_sum);
     }
     cout<<"k is "<<k<<endl;
     node temp;
     for(int i=0;i<k;++i)
     {
        //cout<<"k is "<<k<<endl;
        int con=(rand() %(deg_sum - 1 + 1)) + 1;
        for(int j=0;j<v;++j)
        {
            if(ve[j]>=con)
            {
                con=j;
                break;
            }
        }
        temp.vertex=con;
        temp.next=NULL;
        *ptr=temp;
        gt[v].tail->next=ptr;
        gt[v].tail=ptr;
        ++gt[v].degree;
        ++ptr;
        temp.vertex=v;
        temp.next=NULL;
        *ptr=temp;
        gt[con].tail->next=ptr;
        gt[con].tail=ptr;
        ++gt[con].degree;
        ++ptr;
     }
     return ptr;
}

int main()
{
	int shmid = shmget(IPC_PRIVATE,max_edges*2*sizeof(node) , 0666 | IPC_CREAT);
	//shmat to attach to shared memory
	node *ptr = (node *)shmat(shmid, (void *)0, 0);
    node *start=ptr;
    node temp;
	g_node all_nodes[max_nodes];
    int n1;
	int n2;
	FILE *f = fopen("facebook_combined.txt", "r");
    int k=0;
    while (fscanf(f, "%d %d", &n1, &n2) > 0)
	{
        ++k;
        // if(k<100000)cout<<k<<endl;
        // else break;
		//printf("%d %d\n", n1, n2);
		if(all_nodes[n1].head==NULL)
        {
            temp.vertex=n1;
            temp.next=NULL;
            *ptr=temp;
            all_nodes[n1].head=ptr;
            all_nodes[n1].tail=ptr;
            ++ptr;
        }
        if(all_nodes[n2].head==NULL)
        {
            temp.vertex=n2;
            temp.next=NULL;
            *ptr=temp;
            all_nodes[n2].head=ptr;
            all_nodes[n2].tail=ptr;
            ++ptr;
        }
        temp.vertex=n2;
        temp.next=NULL;
        *ptr=temp;
        all_nodes[n1].tail->next=ptr;
        all_nodes[n1].tail=ptr;
        ++all_nodes[n1].degree;
        ++ptr;
        temp.vertex=n1;
        temp.next=NULL;
        *ptr=temp;
        all_nodes[n2].tail->next=ptr;
        all_nodes[n2].tail=ptr;
        ++all_nodes[n2].degree;
        ++ptr;
	}
    fclose(f);
    temp.vertex=-1;
    temp.next=NULL;
    *ptr=temp;
    ++ptr;
    // node *s=all_nodes[4031].head;
    // while(s!=NULL)
    // {                                           //This is doing its job as expected :)
    //     cout<<s->vertex<<endl;
    //     s=s->next;
    // }
    int shmid_nodes = shmget(IPC_PRIVATE,max_nodes*sizeof(g_node) , 0666 | IPC_CREAT);
    g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
    for(int i=0;i<max_nodes;++i)start_node[i]=all_nodes[i];
    int pid;
    if((pid=fork())==0)
    {
        cout<<"I am the producer\n";
        g_node *start_node = (g_node *)shmat(shmid_nodes, (void *)0, 0);
        node *ptr=(node *)shmat(shmid, (void *)0, 0);
        int num_nodes=0;
        g_node *gt=start_node;
        while(gt[num_nodes].head!=NULL)++num_nodes;
        node *start=ptr;
        while(ptr->vertex!=-1)++ptr;
        cout<<ptr->vertex<<endl;
        cout<<"Num nodes are "<<num_nodes<<endl;
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
           cout<<"m is "<<m<<endl;
           for(int i=0;i<m;++i)
           {
            cout<<"In for loop\n";
             ptr=add_edges(num_nodes-m+i,ptr,start_node);
             
           }
           
           node *tt=gt[num_nodes-1].head;
           while(tt!=NULL)
           {
            cout<<tt->vertex<<endl;
            tt=tt->next;
           }
           sleep(50);
        }
        
    }
    wait(NULL);
	shmdt(ptr);
	shmctl(shmid,IPC_RMID,NULL);
	return 0;
}
