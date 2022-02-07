#include "goatmalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

node_t *head;
size_t allosize;
/*
int main(int argc, char **argv) {
    init(5000);
    destroy();

    return 0;   
}
*/

int init(size_t size) {
    printf("Initializing arena:\n");
    size_t page_size = getpagesize();
    allosize = size;
    printf("...requested size %ld bytes\n", size);
    if (size > MAX_ARENA_SIZE) {
        printf("...error: requested size larger than MAX_ARENA_SIZE (2147483647)\n");
        return ERR_BAD_ARGUMENTS;
    }
    printf("...pagesize is %ld bytes\n", page_size);
    if (size % page_size != 0) {
        printf("...adjusting size with page boundaries\n");
        int scale = size / page_size + 1;
        allosize = scale * page_size;
        printf("...adjusted size is %ld bytes\n", allosize);       
    }
    printf("...mapping arena with mmap()\n");
    int fd = open("/dev/zero",O_RDWR);
    head = mmap(NULL, allosize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

    printf("...arena starts at %p\n", head);
    printf("...arena ends at %p\n", ((void*)((char*)head + allosize)));

    printf("...initializing header for initial free chunk\n");
    head->is_free = 1;
    head->size = allosize;
    head->fwd = NULL;
    head->bwd = NULL;
    printf("...header size is %ld bytes\n", sizeof(*head));
    return ((int)allosize);
}

int destroy() {
    printf("Destroying Arena:\n");
    printf("...unmapping arena with munmap()\n");
    int a = munmap(head, allosize);
    if (a == 0) return 0;
    else {
        printf("...error: cannot destroy unintialized arena. Setting error status\n");
        return ERR_UNINITIALIZED;
    }
}
