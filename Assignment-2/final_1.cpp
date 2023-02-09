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
#include <glob.h>


//g++ -g final_1.cpp -lreadline

using namespace std;

// Parsing Input Command
int child_to_kill=-1;
int command_count=0;
struct command
{
    char *args[128];
    char *outredirect, *inredirect;
};

void sigintHandlerforc(int sig_num)
{
    printf("\n");
	if(child_to_kill!=-1)
    {
        kill(child_to_kill,9);
        child_to_kill=-1;
    }
    if(command_count<=1)return;
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


vector<string> get_all_matches(char *a)
{
    glob_t r;
    vector<string> s;
    glob(a,GLOB_TILDE,NULL,&r);
    for(int i=0;i<r.gl_pathc;++i)s.push_back(r.gl_pathv[i]);
    globfree(&r);
    return s;
}

    

command handle_wild_cards(command c)
{
    command f;
    f.inredirect=c.inredirect;
    f.outredirect=c.outredirect;
    int i=1;
    int j=1;
    f.args[0]=c.args[0];
    while(c.args[i]!=NULL)
    {
        if(!(strchr(c.args[i],'?')==NULL && strchr(c.args[i],'*')==NULL))
        {
              vector<string> s=get_all_matches(c.args[i]);
              if(s.size()==0)
              {
                f.args[j]=c.args[i];
                ++i;++j;
              }
              else
              {
                ++i;
                 for(auto x:s)
                 {
                    
                    f.args[j]=(char*)x.c_str();
                    ++j;
                 }
              }
        }
        else
        {
            f.args[j]=c.args[i];
                ++i;++j;
        }

    }
    f.args[j]=NULL;
    return f;
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
    for(auto &x:vec)x=handle_wild_cards(x);
    return vec;
}

void handle_delep(command c)
{

    int stat_loc;
    pid_t child_pid;
    char *choice;
    char temp[200];
    command new_command;
    new_command.outredirect = new_command.inredirect = NULL;

    // execute the "lsof -t FILE_PATH" command to know the processes which are using the file or have a lock on the file
    new_command.args[0] = strdup("lsof");
    new_command.args[1] = strdup("-t");
    new_command.args[2] = strdup(c.args[1]);
    new_command.args[3] = NULL;

    // return if the file specified in the delep command does not exist
    if (access(new_command.args[2], F_OK) != 0)
    {
        printf("delep: No such file exists \n");
        return;
    }

    child_pid = fork();

    if (child_pid == 0)
    {
        /* Never returns if the call is successful */
        execvp(new_command.args[0], new_command.args);
        exit(EXIT_FAILURE);
    }
    else
    {
        // should not wait if we running a program in background
        waitpid(child_pid, &stat_loc, WUNTRACED);

        // Reading the output PIDs on stdout using pipe and killing them
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            perror("Pipe error: ");
        }

        pid_t pid = fork();

        if (pid == 0)
        {

            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
            execvp(new_command.args[0], new_command.args);
            exit(EXIT_FAILURE);
        }
        else
        {

            int proc_id;
            int status;
            char buf[8];
            char result[33];
            ssize_t nread;
            int flag = 0;

            waitpid(pid, &status, WUNTRACED);
            close(pipefd[1]);

            result[0] = '\0';
            int prompt_flag = 0;

            // Coverting the PID strings read from stdout to integers
            while ((nread = read(pipefd[0], buf, sizeof(buf))) > 0)
            {
                flag = 0;
                if (!prompt_flag)
                {
                    prompt_flag = 1;
                    sprintf(temp, "The above process(es) are using this file or have a lock on it.\nDo you want to delete all the above process(es) and delete the file? [Y/n]: ");
                    choice = readline(temp);
                    if (*choice == 'n' || *choice == 'N' || !strcmp(choice, "No") || !strcmp(choice, "NO") || !strcmp(choice, "no") || !strcmp(choice, "nO"))
                    {
                        printf("Bye...\n");
                        return;
                    }
                    else if (*choice == 'y' || *choice == 'Y' || !strcmp(choice, "yes") || !strcmp(choice, "Yes") || !strcmp(choice, "yEs") || !strcmp(choice, "yeS") || !strcmp(choice, "YEs") || !strcmp(choice, "yES") || !strcmp(choice, "YeS") || !strcmp(choice, "YES"))
                    {
                    }
                    else
                    {
                        printf("Invalid Choice...\n"); // return if user enters invalid choice
                        return;
                    }
                }

                if (nread == sizeof(buf) && buf[nread - 1] != '\n')
                    flag = 1;

                // Killing all the processes one by one
                for (int i = 0; i < nread; i++)
                {
                    if (buf[i] == '\n')
                    {
                        proc_id = atoi(result);
                        if (kill(proc_id, SIGKILL) != 0)
                        {
                            perror("kill");
                        }
                        result[0] = '\0';
                        if (*choice == 'n' or *choice == 'N')
                        {
                            printf("Bye\n");
                            return;
                        }
                    }
                    else
                    {
                        char c[2];
                        c[0] = buf[i];
                        c[1] = '\0';
                        strcat(result, c);
                    }
                }
                if (!flag)
                    result[0] = '\0';
            }
            close(pipefd[0]);
            // Prints this if there is no process using the file or have a lock on it
            if (nread == 0 & prompt_flag == 0)
            {
                printf("Currently no process is using the file or have a lock on it...\n");
                return;
            }
        }

        // Deleting the file using unlink function after killing all the processes using the file or have a lock on it
        unlink(new_command.args[2]);
        printf("The file is removed successfully..\n");
    }
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

int parent_pid(pid_t pid){
    pid_t parent_pid;
    char file[1024];
    char buff[1024];

    snprintf(file, sizeof(file), "/proc/%d/status", pid);

    FILE *fp = fopen(file, "r");
    if (fp==NULL) {
        perror("fopen");
        return 1;
    }

    while (fgets(buff, sizeof(buff), fp)) {
        if (strncmp(buff, "PPid", 4) == 0) {
            
            char t[1024];
            strcpy(t,buff+6);
            parent_pid=atoi(t);
            
            break;
        }
    }

    fclose(fp);
    return parent_pid;
}

int cpu_usage_time(pid_t pid){
    char line[1024];
    char file[1024];
    sprintf(file, "/proc/%d/stat", pid);

    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }
    
    if (fgets(line, sizeof(line), fp) == NULL) {
        perror("Error reading file");
        fclose(fp);
        return 1;
    }
    fclose(fp);

    int utime, stime;
    sscanf(line, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %d %d", &utime, &stime);
    int total_time = utime + stime;
    return total_time;
}

int detected_malware(pid_t pid){
    int first=-1;
    pid_t ppid=parent_pid(pid);
    pid_t final_pid=pid;
    int min_time=cpu_usage_time(pid);
    while(ppid>=1){
        //ppid=parent_pid(pid);
        
            int tp=cpu_usage_time(pid);
            //int tpp=cpu_usage_time(final_pid);
            //printf("PID: %d   Time: %d\n",pid,tp);
            //printf("PPID: %d   Time: %d\n",ppid,tpp);
            if(min_time>tp){
                min_time=tp;
                final_pid=pid;
            }
            pid=ppid;
            ppid=parent_pid(ppid);
    }

    //printf("Detected malware pid:%d\n",pid);
    pid_t pgid=getpgid(final_pid);
    if(pgid==final_pid){
        return final_pid;
    }
    else{
        return first;
    }
}



void handle_sb(command c)
{
    if(strcmp(c.args[1],"-suggest")==0){
        pid_t pid=atoi(c.args[2]);
        pid=detected_malware(pid);
        if(pid==-1){
            printf("No valid malware detected\n");
        }
        else{
            printf("Detected malware pid:%d\n",pid);
        }
        return;
    }
    else{
        if(strcmp(c.args[1],"0")==0)
        {
            printf("0\n");
                return;
        }
        /*char buf1[13];
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
        }*/
        pid_t pid=atoi(c.args[1]);
        printf("Parent and Grandparent PIDS:\n");
        while(pid>=0){
            pid_t ppid=parent_pid(pid);
            printf("%d\n",ppid);
            pid=ppid;
        }
        return;
    }
}


void execprocess(const vector<command> &procs, int background)
{
    int pipefd[2];
    for (int i = 0; i < procs.size(); i++)
    {
        if(strcmp(procs[i].args[0],"cd")==0){handle_cd(procs[i]);continue;}
        if(strcmp(procs[i].args[0],"sb")==0){handle_sb(procs[i]);continue;}
	if(strcmp(procs[i].args[0], "delep")==0){handle_delep(procs[i]); continue;}
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
        else child_to_kill=-1;
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
            ++command_count;
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
