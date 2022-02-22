#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    //invalid memory access 
    int* a = (int*)malloc(sizeof(int));
    *a = 10;
    free(a);

    //missing allocation
    //free(a);

    //uninitialized memory
    int q = 2;
    int p = 1+q;
    printf("%d", p);

    //memory leak
    char* name = (char*)malloc(sizeof(char)*5); 
    free(name);
    return 0;
}
