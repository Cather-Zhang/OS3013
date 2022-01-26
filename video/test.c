#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
    int count = 1;
    if(fork() == 0) { // fork 1
        printf("fork 1 running execl\n");fflush(stdout);
        execl("/usr/bin/ls", "/usr/bin/ls", "-A", NULL);
    }
    else {
        if(fork() == 0) { // fork 2
            printf("fork 2 running execlp\n");
            execlp("ls", "ls", NULL);
        }
        else { // parent
            printf("parent running execvp\n");fflush(stdout);
            char* list[] = {"ls", NULL};
            execvp("ls", list);
        }
    }
    exit(EXIT_SUCCESS);
}

/*
int execl(const char *path, const char *arg, …, NULL);
int execlp(const char *file, const char *arg, …, NULL );
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execle(const char *path, const char *arg, …, NULL, char * const envp[] );
int execve(const char *file, char *const argv[], char *const envp[]);
*/