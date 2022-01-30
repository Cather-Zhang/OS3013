#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void print_cmd(char*para[]);
char* type_prompt();
int read_command(char cmd[], char *par[], int ind, char *his[], char *queue[], char *pipeIn[], char *jobs[], int jobcount);
void run_pipe(char*l[20], char*r[20], char**envp);

void print_his(char *his[], int index) ;
void print_jobs(int pid[], char* cmd[], int count);

int main(int argc, char const *argv[]) {
    char* curr = "";
    char buffer[100];
    char *home = getcwd(buffer, 100);
    char *envp[] = {(char *) "PATH=/bin", 0};
    int rc;
    int re = -1;
    int e = -1;
    int index = 0, isand = -1, jobnum = 0;
    char* op = ""; // operator
    char* output = ""; // file output for >, >>
    char* input[10]; // input for pipe
    while (1) {
        char command[20], cmd[20], *para[10], *history[10], *queue[10], *pipein[10], *jobcmd[256];
        pid_t jobpid[256]; // store process ids
        for (size_t i = 0; i < 10; i++) {
            para[i] = NULL;
            pipein[i] = NULL;
        }

        if (isand == 0 || isand == 1) {
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
            //loop through the jobpid and print out all finished jobs and set them to -1
            //and remove them from the job lists (jobpid and jobcmd)
            curr = type_prompt();
            isand = read_command(command, para, index, history, queue, pipein, jobcmd, jobnum);
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
            char buffer[100] = "";
            char *path = "";
            path = getcwd(buffer, 100);
            printf("%s\n", path);
            fflush(stdout);
        } 
        else if (strcmp(command, "echo") == 0) {
            if (para[1] != NULL)
                printf("%s", para[1]);
            printf("\n");
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
        else if (strcmp(command, "jobs") == 0) {
            print_jobs(jobpid,jobcmd,jobnum);
        }
        //all external
        else {
            if (isand == 2) {
                int fd[2];//, status, pid;
                pipe(fd);
                int status1, status2;
                pid_t pid = fork();
                if(pid == 0) {   
                    char* c;
                    c = strstr(command, "/bin/");
                    if (!c)
                        strcpy(cmd, "/bin/");
                    strcat(cmd, command);

                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[0]);
                    close(fd[1]);
                    execve(cmd, para, envp);
                }
                pid_t pid2 = fork();
                if(pid2 == 0) {
                    char *comm = strdup(pipein[0]);
                    strcpy(cmd, "/bin/");
                    strcat(cmd, comm);
                    //printf("back to parent\n");
                    dup2(fd[0], STDIN_FILENO);
                    close(fd[0]);
                    close(fd[1]);
                    //printf("executing...\n");
                    execve(cmd, pipein,envp);
                }
                close(fd[0]);
                close(fd[1]);
                waitpid(pid, &status1, 0);
                waitpid(pid2, &status2, 0);
                rc = 0;
                fflush(stdout);
                continue;

            }

            else if (isand == -1 || isand == 3) {
                int pid = fork();
                int status;
                if (pid != 0)
                    waitpid(pid, &status, 0);
                else {
                    re = -1;
                    output = "";

                    //looking for >, >>
                    //before > or >> is the content
                    //after is the file

                    if (isand == 3) {
                    for (int i = 1; i < 10; i++) {
                            if(para[i] == NULL) break;
                            //printf("%s\n", para[i]);
                            if (strcmp(para[i],">") == 0 || strcmp(para[i],">>") == 0){
                                op = strdup(para[i]);
                                re = i;
                                output = strdup(para[i+1]);    
                                input[0] = strdup(para[0]);
                                input[i+1] = NULL;
                                break;
                            }

                            input[i] = para[i];
                        }
                    }

                    char* c;
                    c = strstr(command, "/bin/");
                    if (!c)
                        strcpy(cmd, "/bin/");
                    strcat(cmd, command);
                    if(re == -1) e = execve(cmd, para, envp);

                    else if (strcmp(op, ">") == 0 || strcmp(op, ">>") == 0){
                        int args = O_RDWR | O_CREAT;
                        if(strcmp(op, ">>") == 0) args |= O_APPEND;
                        int fd = open(output, args, S_IRUSR | S_IWUSR);
                        dup2(fd, 1);
                        e = execve(cmd, input, envp);
                    }
                    
                    
                    if (e == -1){
                        rc = -1;
                        printf("wshell: could not execute command: %s\n", command);
                        fclose(stdout);
                        exit(0);
                    }  
                }  
            rc = WEXITSTATUS(status);    
            //printf("raw status: %d\n", status);
            //printf("external exit code: %d\n", WIFEXITED(status));
            //printf("WEXITSTATUS(status): %d\n", WEXITSTATUS(status));
            }

            else if (isand == 4) {
                
                isand = -1;
                pid_t pid = fork();
                if(pid == 0) {   
                    char* c;
                    c = strstr(command, "/bin/");
                    if (!c)
                        strcpy(cmd, "/bin/");
                    strcat(cmd, command);

                    execve(cmd, para, envp);
                }
                else {
                    jobpid[jobnum++] = pid;
                    //printf("child %d pid: %d\n", jobnum,  pid);
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

int read_command(char*cmd, char*par[], int index, char *his[], char *queue[], char *pipeIn[], char *jobs[], int jobcount){
    char line[1024] = "";
    int count = 0;
    int isand = -1;

    int c = fgetc(stdin);
    while(c != '\n'){
        line[count++] = (char)c;
        c = fgetc(stdin);
    }
    if(count == 1) return -1;
    his[index % 10] = strdup (line);
    if(!isatty(fileno(stdin))){
        for (int i = 0; i < count; i++) 
            printf("%c", line[i]);
        printf("\n");
        fflush(stdout);
    }
    char *str, *p;
    str = strstr(line, ">");
    if (str) isand = 3;
    
    
    p = strtok(line, " \n");
    int i = 0, j = 0, k = 0;
    

    while (p != NULL)
    {
        if (strcmp(p, "echo") == 0) {
            par[i++] = strdup(p);
            p = strtok (NULL, "\n");
            if (p != NULL)
                par[i++] = strdup(p);
            else break;
        }
        
        else if (strcmp(p, "&") == 0) {
            isand = 4;
            p = strtok (NULL, "\n");
        }
    
        else if ((strcmp(p, "&&") == 0) || (strcmp(p, "||") == 0) || (strcmp(p, "|") == 0)) {
            if (strcmp(p, "&&") == 0) isand = 1;
            else if (strcmp(p, "||") == 0) isand = 0;
            else isand = 2;

            p = strtok (NULL, " \n");
            if (strcmp(p, "echo") == 0) {
                queue[0] = strdup(p);
                p = strtok (NULL, "\n");
                queue[1] = strdup(p);
            }
            else {
                while (p != NULL) {
                    if (isand == 2) pipeIn[k++] = strdup(p);
                    else queue[j++] = strdup(p);
                    p = strtok (NULL, " \n");
                }
            }
        }      
        else par[i++] = strdup(p);
        p = strtok (NULL, " \n");
    }

    strcpy(cmd, par[0]);
    /*
    if (strcmp(par[i-1], "&") == 0) {
        par[i-1] = NULL;
        isand = 4;
    }  */
    par[i] = NULL;
    if (isand == 4){
        char*j = "";
        j = strdup(par[0]);
        for(int k = 1; k < i; k++){
            j = strcat(j, " ");
            j = strcat(j, par[k]);
        }
        jobs[jobcount] = strdup(j);
    } 
    //print_cmd(par);
    //print_cmd(pipeIn);
    //printf("isand: %d\n", isand);
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
    fflush(stdout);
}

void print_jobs(int pid[], char* cmd[], int count) {
    for (int i = 0; i < count; i++) {
        if (pid[i] != -1) {
            printf("%d: %s\n", i, cmd[i]);
        }
    }
    fflush(stdout);
}

/*

void run_pipe(char*l[20], char*r[20], char**envp){
    int fd[2], status, pid;
    pipe(fd);
    pid_t p = fork();
    if(p == 0) {
        //printf("in child 1\n");
        dup2(fd[1], 1);
        close(fd[0]);
        execve(l[0], l, envp);
    }
    p = fork();
    if(p == 0){
        //printf("in child 2\n");
        dup2(fd[0], 0);
        close(fd[1]);
        execve(r[0], r, envp);
    }

    close(fd[0]); close(fd[1]);

	while ((pid = wait(&status)) != -1)
	    exit(0);
}

*/