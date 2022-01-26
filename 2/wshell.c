#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <libgen.h>

void print_cmd(char*para[]);
char* type_prompt();
void read_command(char cmd[], char *par[]);

int main(int argc, char const *argv[]) {
    char* curr = "";
    char buffer[100];
    char *home = getcwd(buffer, 100);
    char *envp[] = {(char *) "PATH=/bin", 0};
    while (1) {
        char command[100], cmd[100], *para[20];

        curr = type_prompt();

        read_command(command, para);

        if(!isatty(fileno(stdin))){
            print_cmd(para);
            fflush(stdout);
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
            }
        }
        //special case for pwd
        else if(strcmp(command, "pwd") == 0){
            char buffer[100];
            char *path = getcwd(buffer, 100);
            printf("%s\n", path);
            fflush(stdout);
        } 
        else if (strcmp(command, "echo") == 0) {
            for (size_t i = 1; i < 20; i++) {
                if (para[i] == NULL) break;
                if (para[i+1] == NULL) printf("%s", para[i]);
                else printf("%s ", para[i]);
            }
            printf("\n");
        }
             
        else {
            if (fork() != 0)
                wait(NULL);
            else {
                char* c;
                c = strstr(command, "/bin/");
                if (!c)
                    strcpy(cmd, "/bin/");
                strcat(cmd, command);
                if (execve(cmd, para, envp) == -1){
                    printf("wshell: could not execute command: %s\n", command);
                    break;
                }
            }
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

void read_command(char*cmd, char*par[]){
    char line[1024] = "";
    int count = 0;
    int i = 0;

    int c = fgetc(stdin);
    while(c != '\n'){
        line[count++] = (char)c;
        c = fgetc(stdin);
    }
    if(count == 1) return;
    char* p = strtok(line, " \n");
    while(p != NULL) {
        par[i++] = strdup(p);
        p = strtok(NULL, " \n");
    }
    strcpy(cmd, par[0]);
    par[i] = NULL;
}

void print_cmd(char*para[]){
    printf("%s", para[0]);
    if (para[1] == NULL) {
        printf("\n");
        return;
    }

    for (size_t i = 1; i < 20; i++) {
        if (para[i] == NULL) break;
        printf(" %s", para[i]);
    }
    printf("\n");
}