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

void handle_cd(command c)
{
    char direc[1024];
    
    if (c.args[1]==NULL)
    {
        chdir(getenv("HOME"));
        if ((getcwd(direc, sizeof(direc))) == NULL)
        {
            perror("getcwd() error");
            return;
        }
        // printf("%s",direc);
        return;
    }
    else
    {
            if (chdir(c.args[1]) != 0)
            {
                perror("chdir() error");
                return;
            }
            else
            {
                if ((getcwd(direc, sizeof(direc))) == NULL)
                {
                    perror("getcwd() error");
                    return;
                }
            }
    }
}
void handle_sb(command c){
    int child_pid=fork();
    if(!child_pid){
        if(strcmp(c.args[1],"-suggest")==0){
            // find the group id
            printf("PID of trojan : %d\n", getpgid(c.args[2]);
        }
        else{
            execl("/bin/ps", "ps", "-o","ppid=",c.args[1], NULL);
        }
    }
    else{
        waitpid(child_pid, &stat_loc, WUNTRACED);
    }
    
}

void execprocess(const vector<command> &procs, int background)
{
    int pipefd[2];
    for (int i = 0; i < procs.size(); i++)
    {
        if(strcmp(procs[i].args[0],"cd")==0){handle_cd(procs[i]);continue;}
        if(strcmp(procs[i].args[0],"sb")==0){handle_sb(procs[i]);continue;}
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
        pid_t child_pid = fork();
        int stat_loc;
        if (child_pid == 0)
        {
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
            execvp(procs[i].args[0], procs[i].args);
            perror("Exec error: ");
            exit(EXIT_FAILURE);
        }
        if(!background)waitpid(child_pid, &stat_loc, WUNTRACED);
        if (i + 1 < procs.size())
            close(outfd);
    }
    
}


//This is main

int main()
    {
        rl_bind_keyseq("\001",rl_beg_of_line);
        rl_bind_keyseq("\005",rl_end_of_line);
        vector <command> v;
        char *input;
        pid_t child_pid;
        char direc[1024];
        char temp1[1080];
        int background;
        int last;
        while (1)
        {
            background=0;
            getcwd(direc, 1024);
            sprintf(temp1,"new_root@GROUP_32:%s#", direc);
            input = readline(temp1);
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
            execprocess(v,background);
            free(input);

        }
        return 0;
    }