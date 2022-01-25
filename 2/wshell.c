#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <libgen.h>

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

        curr = type_prompt();
        //printf("%s ", queue[0]);
        //printf("%d \n", isand);
        if (queue[0] != NULL) {
            if ((isand == 1) ) {
                printf("return code %d \n", rc);
                isand = -1;
                if (rc == 0) {
                    printf("&& operation, rc = 0\n");
                    strcpy(command, queue[0]);
                    for (int i = 0; i < 10; i++) {
                        if (queue[i] == NULL) break;
                        else {
                            strcpy(para[i], queue[i]);
                            queue[i] = NULL;
                        }       
                    }
                }
                else {
                    printf("&& operation, rc != 0\n");
                    for (int i = 0; i < 10; i++) {
                        queue[i] = NULL;
                    }
                    continue;     
                }
            }
            else if ((isand == 0) && (rc == 0)) {
                isand = -1;
                strcpy(command, queue[0]);
                for (int i = 0; i < 10; i++) {
                    if (queue[i] != NULL){
                        strcpy(para[i], queue[i]);
                        queue[i] = NULL;
                    }
                }
                continue;
            }
        }
        else {
            isand = read_command(command, para, index, history, queue);
            index++;
        }

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
            if (fork() != 0)
                wait(NULL);
            else {
                char* c;
                c = strstr(command, "/bin/");
                if (!c)
                    strcpy(cmd, "/bin/");
                strcat(cmd, command);
                if (execve(cmd, para, envp) == -1){
                    rc = -1;
                    printf("wshell: could not execute command: %s\n", command);
                    break;
                }
            }
            rc = 0;
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
