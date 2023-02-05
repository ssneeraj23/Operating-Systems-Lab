#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <term.h>
#include <curses.h>
#include "/usr/include/readline/readline.h"
#include <sys/wait.h>

char **get_input(char * line){
    char ** cmd=malloc(8*sizeof(char*));
    char *delimiter=" ";
    
    int i=0;
    char * token=strtok(line, delimiter);
    while(token!=NULL){
        cmd[i++]=token;
        token=strtok(NULL,delimiter);
    }
    cmd[i++]=NULL;
    return cmd;
}
void handle_cd(char * input){
    char direc[1024];
    if(strcmp(input,"cd")==0){
        chdir(getenv("HOME"));
        if((getcwd(direc,sizeof(direc)))==NULL){
						perror("getcwd() error");
						return;
					}
        //printf("%s",direc);
        return;
    }
    else{
        char token[5];
        char directory[1024];
        strncpy(token,input,3);
		if(strcmp(token,"cd ")==0){
			strcpy(directory,input+3);
            printf("directory: %s\n",directory);
			if(chdir(directory)!=0){
				perror("chdir() error");			
				return ;
			}
			else{
				if((getcwd(direc,sizeof(direc)))==NULL){
					perror("getcwd() error");
					return;
				}
            }
		}
    }
}

int main() {
    char **command;
    char *input;
    pid_t child_pid;
    int stat_loc;
    char direc[1024];

    while (1) {
        getcwd(direc,1024);
        printf("new_root@GROUP_32:%s",direc);
        input = readline("# ");
        command = get_input(input);

        if (!command[0]) {      /* Handle empty commands */
            free(input);
            free(command);
            continue;
        }
        if(strcmp(command[0],"cd")==0){
            handle_cd(input);
            continue;
        }
        child_pid = fork();
        if (child_pid == 0) {
            /* Never returns if the call is successful */
            execvp(command[0], command);
            printf("This won't be printed if execvp is successul\n");
        } else {
            waitpid(child_pid, &stat_loc, WUNTRACED);
            // should not wait if we running a program in background
        }

        free(input);
        free(command);
    }

    return 0;
}

