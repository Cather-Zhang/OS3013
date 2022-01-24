#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <libgen.h>


char* type_prompt() {
    char buffer[100];
    char *path = getcwd(buffer, 100);
    char *dir = basename(path);
    
    printf("%s$ ", dir);
    fflush(stdout);
    return path;
}

void read_command(char cmd[], char *par[]) {
    char line[1024];
    int count = 0, i = 0, j = 0;
    char* array[100], *pch;

    //read a line
    for(;;){
        int c = fgetc(stdin);
        line[count++] = (char)c;
        if(c == '\n') break;
    }
    if(count == 1) return;
    pch = strtok(line, " \n");

    while(pch != NULL){
        array[i++] = strdup(pch);
        pch = strtok(NULL, " \n");
    }
    //first word is command
    strcpy(cmd, array[0]);

    for(j = 0; j < i; j++){
        par[j] = array[j];
    }
    par[i] = NULL;
}

void print_cmd(char*command, char*para[]){
    printf("running: %s\n", command);
    for (size_t i = 0; i < 20; i++) {
        if (para[i] == NULL) break;
        printf("p%ld: %s\n", i, para[i]);
    }
}

int main(int argc, char const *argv[]) {
    char cmd[100], command[100], *para[20];
    char *envp[] = {(char *) "PATH=/bin", 0};
    char* curr = "";
    while (1) {
        curr = type_prompt();
        read_command(command, para);
        if(!isatty(fileno(stdin))){
            printf("%s\n", command);
            fflush(stdout);
        }
        if(strcmp(command, "exit") == 0) {
            return 0;
        }
        if (fork() != 0)
            wait(NULL);
        else {
            strcpy(cmd, "/bin/");
            strcat(cmd, command);

            //special case for cd
            if(strcmp(command, "cd") == 0){
                strcpy(curr, para[1]);
                chdir(curr);
                para[1] = curr;
            }
            //special case for pwd
            if(strcmp(command, "pwd") == 0){
                //printf("%s\n", curr);
                //fflush(stdout);
                //break;
            }

            //execute cmd with para
            execve(cmd, para, envp);
        }
    }
    return 0;
}

