#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char const *argv[]) {
    if (fork() != 0) 
        wait(NULL);
    else {
        
<<<<<<< HEAD
        execl("/usr/bin/ls", "/usr/bin/ls", NULL);
        execlp("ls", "ls", NULL);
=======
        //execl("/usr/bin/ls", "/usr/bin/ls", NULL);
        //execlp("ls", "ls", NULL);
>>>>>>> parent of dc9734f (part 3 kinda)
        char* arg[] = {"ls", NULL};
        execvp(arg[0], arg);
    }
    return 0;
}