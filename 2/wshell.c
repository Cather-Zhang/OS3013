#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <libgen.h>


void type_prompt() {
    char buffer[100];
    char *path = getcwd(buffer, 100);
    char *dir = basename(path);
    printf("%s$ ", dir);
}

void read_command(char cmd[], char *par[]){

}

int main(int argc, char const *argv[])
{
    char cmd[100], command[100], *para[20];
    char *envp[] = {(char *) "PATH=/bin", 0};
    while (1) {
        type_prompt();
        read_command(command, para);
        if (fork() != 0)
            wait(NULL);
        else {
            strcat(cmd, command);
            execve(cmd, para, envp);
        }
        if (strcmp(command, "exit") == 0)
            break;
    }
    return 0;
}



