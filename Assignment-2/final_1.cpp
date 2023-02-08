#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <csignal>
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



//g++ -g final_1.cpp -lreadline

using namespace std;

// Parsing Input Command
int child_to_kill;
 
struct command
{
    char *args[128];
    char *outredirect, *inredirect;
};

void sigintHandlerforc(int sig_num)
{
    printf("\n");
	signal(SIGINT, sigintHandlerforc);
	if(child_to_kill!=-1)
    {
        kill(child_to_kill,9);
        child_to_kill=-1;
    }
	fflush(stdout);
    return;
}

void sigintHandlerforz(int sig_num)
{
    printf("\n");
    kill(child_to_kill,SIGCONT);
    raise(SIGCHLD);
    child_to_kill=-1;
    return;
}

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

void handle_sb(command c)
{
    if(strcmp(c.args[1],"0")==0)
        {
            printf("0\n");
            return;
        }
    char buf1[13];
    char buf2[13];
    char buf3[13];
    char buf4[13];
    char f_name[50];
    int flag=0;
    while(1)
    {
        bzero(buf1,13);
        bzero(buf2,13);
        bzero(buf3,13);
        bzero(f_name,50);
        if(flag==0)
        {
           sprintf(f_name,"/proc/%s/stat",c.args[1]);
        //    printf("f_name si %s\n",f_name);
           flag=1;
        }
        else sprintf(f_name,"/proc/%s/stat",buf4);
        //printf("f_name si %s\n",f_name); 
        FILE *f=fopen(f_name,"r");
        bzero(buf4,13);
        fscanf(f,"%s %s %s %s",buf1,buf2,buf3,buf4);
        if(strcmp(buf4,"0")==0)
        {
            printf("0\n");
            break;
        }
        printf("%s\n",buf4);
    }
    return;

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
        child_to_kill=child_pid;
        int stat_loc;
        if (child_pid == 0)
        {
            setpgid(0,0);
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
        signal(SIGINT, sigintHandlerforc);
        signal(SIGTSTP, sigintHandlerforz);
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