#include <iostream>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <cstring>
using namespace std;
int n=37700;

string bruh(ifstream &f)
{
    string s;
    getline(f,s);
    return s;
}

typedef struct action
{
    int user_id;
    int action_id;
    int action_type;
    time_t timestamp;
} action;

typedef struct semaphore
{
    int value;
    queue<action>q;
} semaphore;





//action_type 0->post
//1->comment
//2->like

vector<vector<int>>adj(n);
vector<vector<int>>nextn(n);
vector<vector<pair<int,int>>>cnt(n);
queue<action>monitor;
queue<action>wall;

int main()
{
    string line;

    string file = "musae_git_edges.csv";
    ifstream inFile(file.c_str());
    getline(inFile,line);
    while(getline(inFile,line))
   {
        int pos =(int)line.find(',');
        //cout<<pos<<"\n";
        int n1  = stoi(line.substr(0,pos));
        int n2 = stoi(line.substr(pos+1,line.length()-pos-2));
        adj[n1].push_back(n2);
        adj[n2].push_back(n1);
        //cout<<n1<<" "<<" "<<n2<<"\n";
    }


    for(int i=0;i<n;i++)
    {
        for(auto x:adj[i])
        {
            nextn[i].insert(nextn[i].end(),adj[x].begin(),adj[x].end());
        }
        sort(nextn[i].begin(),nextn[i].end());
        while(!nextn[i].empty())
        {
            int curr = nextn[i][0];
            pair<int,int>temp;
            temp.first = curr;
            temp.second = 0;
            cnt[i].push_back(temp);
            while(!nextn[i].empty()&&nextn[i][0]!=n)
            {
                cnt[i][cnt[i].size()-1].second++;
                nextn[i].erase(nextn[i].begin());
            }
        }
    }

    vector<int>generated;
    set<int>generated_set;
    while(generated_set.size()<100)
    {
        generated_set.insert(1+rand()%n);
    }
    for(auto x:generated_set)
    {
        generated.push_back(x);
    }
    for(auto x:generated)
    {
        int actions = (int)(10.0*(1.0+log2(adj[x-1].size())));
        int id=sizeof(wall)/sizeof(action)+1;
        for(int i=0;i<actions;i++)
        {
            action temp;
            temp.user_id = x;
            temp.action_id = id;
            temp.action_type = rand()%3;
            temp.timestamp = time(NULL);

            monitor.push(temp);
            
            wall.push(temp);
            id++;
        }
    }
    return 0;
}