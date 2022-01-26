#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/types.h>

void print_cmd(char*para[]);
char* type_prompt();
int read_command(char cmd[], char *par[], int ind, char *his[], char *queue[]);

void print_his(char *his[], int index) ;

int main(int argc, char const *argv[]) {
    char* curr = "";
    char buffer[100];
    char *home = getcwd(buffer, 100);
    char *envp[] = {(char *) "PATH=/bin", 0};
    int rc;


    int index = 0, isand = -1;
    while (1) {
        char command[20], cmd[20], *para[10], *history[10], *queue[10];
        for (size_t i = 0; i < 10; i++) {
            para[i] = NULL;
        }

        if (queue[0] != NULL) {
            //&& operation
            if ((isand == 1) ) {
                isand = -1;
                if (rc == 0) {
                    //return true, should execute
                    strcpy(command, queue[0]);
                    for (size_t j = 0; j < 10; j++) {             
                        if (queue[j] == NULL) break;
                        else {
                            para[j] = strdup(queue[j]);
                        }       
                    }
                    //empty queue
                    for (int k = 0; k < 10; k ++) {
                        queue[k] = NULL;
                    }
                }
                else {
                    //return fail, skip loop
                    for (int i = 0; i < 10; i++) {
                        queue[i] = NULL;
                    }
                    continue;     
                }
            }
            else if (isand == 0) {
                isand = -1;
                if (rc == 0) {
                    //return true, skip loop;
                    for (int i = 0; i < 10; i++) {
                        queue[i] = NULL;
                    }
                    continue;   
                }
                else {
                    //return fail, execute;
                    strcpy(command, queue[0]);
                    for (size_t j = 0; j < 10; j++) {             
                        if (queue[j] == NULL) break;
                        else {
                            para[j] = strdup(queue[j]);
                        }       
                    }
                    for (int k = 0; k < 10; k ++) {
                        queue[k] = NULL;
                    }
                }
            }
        }

        //when there is nothing in queue
        else {
            //initialize queue
            for (int i = 0; i < 10; i++) {
                queue[i] = NULL;
            }   
            curr = type_prompt();
            isand = read_command(command, para, index, history, queue);
            index++;
        }
        
        if(strcmp(command, "exit") == 0) {
            return 0;
        }

        //special case for cd
        if(strcmp(command, "cd") == 0){
            if (para[1] == NULL) {
                strcpy(curr, home);
                chdir(curr);
            } 
            else if(para[2] != NULL) {
                printf("wshell: cd: too many arguments\n");
                fflush(stdout);
            }
            else{
                strcpy(curr, para[1]);
                if(chdir(curr) == -1)
                    printf("wshell: no such directory: %s\n", curr);
                para[1] = curr;
                fflush(stdout);
            }
        }
        //special case for pwd
        else if(strcmp(command, "pwd") == 0){
            char buffer[100];
            char *path = getcwd(buffer, 100);
            printf("%s\n", path);
            rc = 0;
            fflush(stdout);
        } 
        else if (strcmp(command, "echo") == 0) {
            for (size_t i = 1; i < 10; i++) {
                if (para[i] == NULL) break;
                if (para[i+1] == NULL) printf("%s", para[i]);
                else printf("%s ", para[i]);
            }
            printf("\n");
            rc = 0;
            fflush(stdout);
        }
        else if (strcmp(command, "history") == 0) {
            if (index < 10) {
                for (size_t i = 0; i < index; i++) {           
                    printf("%s\n", history[i]);
                }
            }
            else {
                for (size_t i = index; i < index + 10; i++) {           
                    size_t a = i % 10;
                    printf("%s\n", history[a]);
                }
            }
            fflush(stdout);
        }
        else {
            int pid = fork();
            int status;
            if (pid != 0)
                waitpid(-1, &status, 0);
            else {
                char* c;
                c = strstr(command, "/bin/");
                if (!c)
                    strcpy(cmd, "/bin/");
                strcat(cmd, command);
                if (execve(cmd, para, envp) == -1){
                    rc = -1;
                    printf("wshell: could not execute command: %s\n", command);
                    exit(0);
                }
            }

            rc = WEXITSTATUS(status);
            //printf("raw status: %d\n", status);
            //printf("external exit code: %d\n", WIFEXITED(status));
            //printf("WEXITSTATUS(status): %d\n", WEXITSTATUS(status));
        }  
    }
    return 0;
}

char* type_prompt() {
    char buffer[100];
    char *path = getcwd(buffer, 100);
    char *dir = basename(path);
    
    printf("%s$ ", dir);
    fflush(stdout);
    return path;
}

int read_command(char*cmd, char*par[], int index, char *his[], char *queue[]){
    char line[1024] = "";
    int count = 0;
    int i = 0, j = 0, isand;

    int c = fgetc(stdin);
    while(c != '\n'){
        line[count++] = (char)c;
        c = fgetc(stdin);
    }
    if(count == 1) return -1;
    his[index % 10] = strdup (line);
    if(!isatty(fileno(stdin))){
        printf("%s\n", line);
        fflush(stdout);
    }
    
    char* p = strtok(line, " \n");

    while (p != NULL)
    {
        if ((strcmp(p, "&&") == 0) || (strcmp(p, "||") == 0)) {
            if (strcmp(p, "&&") == 0) isand = 1;
            else isand = 0;
            p = strtok (NULL, " \n");
            while (p != NULL) {
                queue[j++] = strdup(p);
                p = strtok (NULL, " \n");
            }
        }
        else par[i++] = strdup(p);
        p = strtok (NULL, " \n");
    }
    strcpy(cmd, par[0]);
    par[i] = NULL;
    return isand;
}



 void print_cmd(char*para[]){
    printf("%s", para[0]);
    if (para[1] == NULL) {
        printf("\n");
        return;
    }
    for (size_t i = 1; i < 10; i++) {
        if (para[i] == NULL) break;
        printf(" %s", para[i]);
    }
    printf("\n");
}