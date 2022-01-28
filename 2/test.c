#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char const *argv[]) {
    char * para[5];
    para[0] = strdup("/bin/cat");
    para[1] = strdup("out.txt");
    char *queue[2];
    queue[0] = strdup("/bin/grep");
    queue[1] = strdup("hi");


    int fd[2];//, status, pid;
    pipe(fd);
    pid_t p = fork();
    if(p == 0) {
        //printf("in child 1\n");
        dup2(fd[1], 1);
        close(fd[0]);
        execvp(para[0], para);
    }
    else {
        wait(NULL);
        //printf("back to parent\n");
        dup2(fd[0], 0);
        close(fd[1]);
        printf("executing...\n");
        fflush(stdout);
        execvp(queue[0], queue);
    }

    //close(fd[0]); close(fd[1]);

	//while ((pid = wait(&status)) != -1)
	    //return 0;

}
/*
    int fd[2], status, pid;
    pipe(fd);
    pid_t p = fork();
    if(p == 0) {
        printf("in child 1\n");
        dup2(fd[1], 1);
        close(fd[0]);
        execl("/usr/bin/cat", "/usr/bin/cat", "out.txt", NULL);
    }
    p = fork();
    if(p == 0){
        printf("in child 2\n");
        dup2(fd[0], 0);
        close(fd[1]);
        execl("/usr/bin/wc", "/usr/bin/wc", "-l", NULL);
    }

    close(fd[0]); close(fd[1]);

	while ((pid = wait(&status)) != -1)
	    return 0;
*/