#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include<unistd.h>
#include<bits/stdc++.h>
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
} g_node;

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
        ++ptr;
        temp.vertex=n1;
        temp.next=NULL;
        *ptr=temp;
        all_nodes[n2].tail->next=ptr;
        all_nodes[n2].tail=ptr;
        ++ptr;
	}

    // node *s=all_nodes[4031].head;
    // while(s!=NULL)
    // {                                           //This is doing its job as expected :)
    //     cout<<s->vertex<<endl;
    //     s=s->next;
    // }
    if(fork()==0)
    {
        cout<<"I am the child\n";
        node *ptr=(node *)shmat(shmid, (void *)0, 0);
        while(ptr!=NULL)
        {
            cout<<ptr->vertex<<endl;
            ptr=ptr->next;
        }
    }
	fclose(f);
	shmdt(ptr);
	shmctl(shmid,IPC_RMID,NULL);
	return 0;
}
