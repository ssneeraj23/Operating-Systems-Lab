#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <term.h>
#include <fstream>
#include <curses.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h> 
#include <glob.h>
using namespace std;
vector<string>history;
vector<pid_t>child_pids;
int histindex=0;
int rootlen;
string s;

vector<string> glob(const string& pattern) {
    using namespace std;

    // glob struct resides on the stack
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));

    // do the glob operation
    int return_value = glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
    if(return_value != 0) {
        globfree(&glob_result);
        stringstream ss;
        ss << "glob() failed with return_value " << return_value << endl;
        throw std::runtime_error(ss.str());
    }

    // collect all the filenames into a std::list<std::string>
    vector<string> filenames;
    for(size_t i = 0; i < glob_result.gl_pathc; ++i) {
        filenames.push_back(string(glob_result.gl_pathv[i]));
    }

    // cleanup
    globfree(&glob_result);

    // done
    return filenames;
}

// Parsing Input Command

// int uparrow(int count,int key) {
//   // Your custom code here...
//   for(auto x:history)
//   {
//     cout<<x<<"\n";
//   }
//   if(histindex>0)
//   {
//   cout<<"\r";
//   cout<<history[histindex-1];
//   histindex--;
//   }
//   return 0;
// }

// int downarrow(int count,int key) {
//   // Your custom code here...
//   if(histindex<history.size()-1)
//   {
//   cout<<"\r";
//   cout<<history[histindex+1];
//   histindex++;
//   }
//   return 0;
// }

int uparrow(int count,int key){
  // Your custom code for the up arrow key here...
  
  if(histindex>=0)
  {
  printf("\33[2K\r");
  cout<<s;
  cout<<history[histindex];
  if(histindex!=0)
  histindex--;
  }
  return 0;
}

int downarrow(int count,int key) {
  // Your custom code for the down arrow key here...
  if(histindex<history.size()-1)
  {
  printf("\33[2K\r");
  cout<<s;
  cout<<history[histindex+1];
  histindex++;
  }
  else{
    printf("\33[2K\r");
    cout<<s;
  }
  return 0;
}

 
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
    int wildcard=0;
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
            for(int i=0;i<strlen(token2);i++)
            {
                if(token2[i]=='*'||token2[i]=='?')
                {
                    wildcard=1;
                }
            }
            if(wildcard)
            {
                vector<string>reg=glob(token2);
                for(auto x:reg)
                {
                    comm.args[cnt++] = strdup(x.c_str());
                }
            }
            else{
                comm.args[cnt++]=strdup(token2);
            }
        }
        comm.args[cnt] = NULL;
        if (cnt)
            vec.push_back(comm);
        
        wildcard=0;
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

void execprocess(const vector<command> &procs, int background)
{
    int pipefd[2];
    for (int i = 0; i < procs.size(); i++)
    {
        if(strcmp(procs[i].args[0],"cd")==0){handle_cd(procs[i]);continue;}
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
        child_pids.push_back(child_pid);
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

void SIGINT_handler(int signum)
{
	signal(SIGINT,SIGINT_handler);
	for(auto& x:child_pids)
	{
		kill(x,9);
	}
	child_pids.clear();
	fflush(stdout);
}

void SIGTSTP_handler(int signum)
{
    signal(SIGTSTP,SIGTSTP_handler);
    //cntrol+z code
}


//This is main

int main()
    {
        signal(SIGINT,SIGINT_handler);
        signal(SIGTSTP,SIGTSTP_handler);
        vector <command> v;
        char *input;
        pid_t child_pid;
        char direc[1024];
        int background;
        int last;
        history.clear();
        ifstream is("history.txt");
        string str;
        int i=0;
        while(getline(is, str))
        {
            history.push_back(str);
        }
        //rl_command_func_t myreadline;
        //rl_bind_key('\033', rl_startup_hook);
        rl_bind_keyseq("\033[A", uparrow);
    	rl_bind_keyseq("\033[B", downarrow);
        ofstream my_file;
	    my_file.open("history.txt", ios::ios_base::app);
        while (1)
        {
            background=0;
            getcwd(direc, 1024);
            s="new_root@GROUP_32:";
            s.append(direc);
            s.append("# ");
            rootlen=s.length();
            input = readline(s.c_str());
            history.push_back(input);
            histindex=history.size()-1;
            
            if(!my_file)
            {
                cout<<"history.txt file not found"<<endl;
            }
            else
            {
                my_file<<input;
                my_file<<endl;
            }
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
        //my_file.close();
        return 0;
    }
