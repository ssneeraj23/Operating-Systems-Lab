#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <term.h>
#include <fstream>
#include <curses.h>
#include "/usr/include/readline/readline.h"
#include <sys/wait.h>
#include <bits/stdc++.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h> 
using namespace std;

// Parsing Input Command
 
struct command
{
    char *args[128];
    char *outredirect, *inredirect;
};

vector<command> get_Input(const char user_input[])
{
    char *user_input_copy = strdup(user_input);
    vector<command> vec;
    const char pipedelim[] = "|";
    const char spacedelim[] = " ";
    char *token1, *token2;
    while ((token1 = strsep(&user_input_copy, pipedelim)) != NULL)
    {
        command comm;
        comm.outredirect = comm.inredirect = NULL;
        int cnt = 0, outredflag = 0, inredflag = 0;
        while ((token2 = strsep(&token1, spacedelim)) != NULL)
        {
            if (!strcmp(token2, ""))
                continue;
            if (outredflag)
            {
                comm.outredirect = strdup(token2);
                outredflag = 0;
                continue;
            }
            if (inredflag)
            {
                comm.inredirect = strdup(token2);
                inredflag = 0;
                continue;
            }
            if (!strcmp(token2, "<"))
            {
                inredflag = 1;
                continue;
            }
            if (!strcmp(token2, ">"))
            {
                outredflag = 1;
                continue;
            }
            comm.args[cnt++] = strdup(token2);
        }
        comm.args[cnt] = NULL;
        if (cnt)
            vec.push_back(comm);
    }
    return vec;
}


void execprocess(const vector<command> &procs, int background)
{
    // current_process_group = 0;
    // foregroundprocs.clear();
    // while (!stoppedprocs.empty())
    //     stoppedprocs.pop();
    int pipefd[2];
    for (int i = 0; i < procs.size(); i++)
    {
        int infd = STDIN_FILENO, outfd = STDOUT_FILENO;
        if (procs[i].inredirect != NULL)
        {
            infd = open(procs[i].inredirect, O_RDONLY);
        }
        if (procs[i].outredirect != NULL)
        {
            outfd = open(procs[i].outredirect, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0666);
        }
        if (i > 0)
        {
            infd = pipefd[0];
        }
        if (i + 1 < procs.size())
        {
            if (pipe(pipefd) == -1)
            {
                perror("Pipe error: ");
            }
            outfd = pipefd[1];
        }
        // sigchldBlocker(SIG_BLOCK);
        pid_t child_pid = fork();
        int stat_loc;
        if (child_pid == 0)
        {
            // sigchldBlocker(SIG_UNBLOCK);
            if (infd != STDIN_FILENO)
            {
                dup2(infd, STDIN_FILENO);
                close(infd);
            }
            if (outfd != STDOUT_FILENO)
            {
                dup2(outfd, STDOUT_FILENO);
                close(outfd);
            }
            // setpgid(0, current_process_group);
            execvp(procs[i].args[0], procs[i].args);
            perror("Exec error: ");
            exit(EXIT_FAILURE);
        }
        if(!background)waitpid(child_pid, &stat_loc, WUNTRACED);
        // if (current_process_group == 0)
        // {
        //     current_process_group = child_pid;
        //     if (!background)
        //     {
        //         tcsetpgrp(STDIN_FILENO, current_process_group);
        //         tcsetattr(STDIN_FILENO, TCSANOW, &oldtio);
        //     }
        // }
        // if (!background)
        //     foregroundprocs.insert(child_pid);
        // else
        //     backgroundprocs.insert(child_pid);
        // sigchldBlocker(SIG_UNBLOCK);
        if (i + 1 < procs.size())
            close(outfd);
    }
    // if (!background)
    // {
    //     while (!foregroundprocs.empty())
    //         ;
    //     while (!stoppedprocs.empty())
    //     {
    //         backgroundprocs.insert(stoppedprocs.top());
    //         kill(stoppedprocs.top(), SIGCONT);
    //         stoppedprocs.pop();
    //     }
    //     tcsetpgrp(STDIN_FILENO, getpgrp());
    //     tcsetattr(STDIN_FILENO, TCSANOW, &tio);
    // }
    // current_process_group = 0;
    // foregroundprocs.clear();
    // while (!stoppedprocs.empty())
    //     stoppedprocs.pop();
}



//This is main

int main()
    {
        vector <command> v;
        char *input;
        pid_t child_pid;
        char direc[1024];
        int background;
        int last;
        while (1)
        {
            background=0;
            getcwd(direc, 1024);
            printf("new_root@GROUP_32:%s", direc);
            input = readline("# ");
            char temp[1024];
            strcpy(temp, input);
            v = get_Input(temp);
            if (v.size()==0)
            { /* Handle empty commands */
                free(input);
                continue;
            }
            if(input[strlen(input)-1]=='&')
            {
                background=1;
                char **p=v[v.size()-1].args;
                int i=0;
                while(p[i+1][0]!='&')++i;
                p[i+1]=NULL;
                printf("Background\n");
            }
            // for(auto x:v)
            // {
            //     int i=0;
            //     while(x.args[i]!=NULL)
            //     {
            //         cout<<x.args[i]<<endl;
            //         ++i;
            //     }
            // }
            execprocess(v,background);
            free(input);

        }
        return 0;
    }