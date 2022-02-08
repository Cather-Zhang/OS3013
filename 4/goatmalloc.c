#include "goatmalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

node_t *head;
size_t allosize = 0;
int statusno = 0;
//void print_header(node_t *header);

/*
int main(int argc, char **argv) {
    init(4096);
    walloc(100);
    destroy();

    return 0;   
}*/


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
    head->size = allosize - sizeof(*head);
    head->fwd = NULL;
    head->bwd = NULL;
    printf("...header size is %ld bytes\n", sizeof(*head));
    //print_header(head);
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

void* walloc(size_t size) {
    printf("Allocating memory:\n");
    void *buff = NULL;
    if (allosize == 0)  {
        printf("Error: Unitialized. Setting status code\n");
        statusno = ERR_UNINITIALIZED;
        return NULL;
    }
    printf("...looking for free chunk of >= %ld bytes\n", size);

    //checking there is enough free space in the chunk
    node_t *prev = head;
    size_t totalFree = 0;
    while (prev != NULL) {
        if (prev->is_free) 
            totalFree += prev->size;
        prev = prev->fwd;
    }
    if (totalFree < size) {
        printf("...no such free chunk exists\n...setting error code\n");
        statusno = ERR_OUT_OF_MEMORY;
        return NULL;
    }

    //start allocating
    prev = head;
    node_t *newHead;
    while (prev != NULL) {
        if (prev ->size >= size) {
            printf("...found free chunk of %ld bytes with header at %p\n", prev->size, prev);
            printf("...free chunk->fwd currently points to %p\n", prev->fwd);
            printf("...free chunk->bwd currently points to %p\n", prev->bwd);
            printf("...checking if splitting is required\n");
            if (prev->size == size) {
                printf("...splitting not required\n");
                printf("...updating chunk header at %p\n", prev);
                prev->is_free = 0;
                newHead = prev;
            }
            else {
                printf("...splitting free chunk\n");
                printf("...updating chunk header at %p\n", prev);
                newHead = prev;
                newHead -> size = size;
                newHead -> bwd = prev->bwd;
                prev = (node_t *)((char*)newHead + sizeof(*head) + size);
                prev->size -= size - sizeof(*head);
                prev->bwd = newHead;
                newHead -> fwd = prev;
                newHead -> is_free = 0;
                break;
            }
        }
        prev = prev->fwd;
    }
    //print_header(head);
    printf("...being careful with my pointer arthimetic and void pointer casting\n");
    printf("...allocation starts at %p\n", (void*)((char*)newHead+sizeof(*head)));
    buff = (void*)newHead;
    return buff;
}

void wfree(void *ptr) {

}

/*
void print_header(node_t *header){
  //Note: These printf statements may produce a segmentation fault if the buff
  //pointer is incorrect, e.g., if buff points to the start of the arena.
  printf("Header->size: %lu\n", header->size);
  printf("Header->fwd: %p\n", header->fwd);
  printf("Header->bwd: %p\n", header->bwd);
  printf("Header->is_free: %d\n", header->is_free);
}
*/