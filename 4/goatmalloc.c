#include <stddef.h>
#include ".goatmalloc_h"
#include "goatmalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

node_t *head;
size_t allosize = 0;
int statusno = 0;

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
        return buff;
    }
    printf("...looking for free chunk of >= %ld bytes\n", size);

    //checking there is enough free space in the chunk
    node_t *prev = head;
    unsigned short hasChunk = 0;
    while (prev != NULL) {
        if (prev->is_free && prev->size >= size) {
            hasChunk = 1;
            break;
        }
        prev = prev->fwd;
    }
    if (!hasChunk) {
        printf("...no such free chunk exists\n...setting error code\n");
        statusno = ERR_OUT_OF_MEMORY;
        buff = NULL;
        return buff;
    }
    else{
    //start allocating
    prev = head;
    node_t *newHead;
    size_t available = 0;
    while (prev != NULL) {
        if (prev->size >= size && prev->is_free) {
            available = prev->size;
            printf("...found free chunk of %ld bytes with header at %p\n", prev->size, prev);
            printf("...free chunk->fwd currently points to %p\n", prev->fwd);
            printf("...free chunk->bwd currently points to %p\n", prev->bwd);
            printf("...checking if splitting is required\n");
            if (prev->size == size) {
                printf("...splitting not required\n");
                printf("...updating chunk header at %p\n", prev);
                prev->is_free = 0;
                newHead = prev;
                break;
            }
            else if (prev->size - size <= 32) {
                printf("...splitting not possible\n");
                prev->is_free = 0;
                newHead = prev;
                break;
            }
            else {
                printf("...splitting free chunk\n");
                printf("...updating chunk header at %p\n", prev);
                prev -> size = size;
                newHead = (node_t *)((char*)prev + sizeof(*head) + size);
                newHead->size = available - size - sizeof(node_t);
                newHead->bwd = prev;
                newHead->is_free = 1;
                prev -> fwd = newHead;
                prev -> is_free = 0;        
                break;
            }
        }
        prev = prev->fwd;
    }
    printf("...being careful with my pointer arthimetic and void pointer casting\n");
    printf("...allocation starts at %p\n", (void*)((char*)prev+sizeof(node_t)));
    buff = (void*)((char*)prev+sizeof(node_t));
    return buff;
    }
}

void wfree(void *ptr) {
    printf("Freeing allocated memory:\n");
    printf("...supplied pointer %p\n:", ptr);
    printf("...being careful with my pointer arthimetic and void pointer casting\n");
    node_t *header = ptr - sizeof(node_t);
    printf("...accessing chunk header at %p\n", header);
    printf("...chunk of size %ld\n", header->size);
    header->is_free = 1;
    printf("...checking if coalescing is needed\n");
    size_t totalFree = 0;
    
    if (header->fwd != NULL &&  header->bwd != NULL && 
    header->fwd->is_free && header->bwd->is_free) {
        printf("...col. case 1: previous, current, and next chunks all free.\n");
        totalFree = header->size + header->bwd->size + header->fwd->size + 2*sizeof(node_t);
        header->bwd->size = totalFree;
        if (header->fwd->fwd != NULL) {
            header->fwd->fwd->bwd = header->bwd;
            header->bwd->fwd = header->fwd->fwd;
        }
        else {
            header->bwd->fwd = NULL;
        }
        return;    
    }
    else if (header->bwd != NULL && header->bwd->is_free) {
            printf("...col. case 2: previous and current chunks free.\n");
            totalFree = header->size + header->bwd->size + sizeof(node_t);
            header->bwd->size = totalFree;
            if (header->fwd != NULL){
                header->bwd->fwd = header->fwd;
                header->fwd->bwd = header->bwd;
            }
            else {
                header->bwd->fwd = NULL;
            }
            return;
        }
    else if (header->fwd != NULL && header->fwd->is_free) {
        printf("...col. case 3: current and next chunks free.\n");
        totalFree = header->size + header->fwd->size + sizeof(node_t);
        header->size = totalFree;
        if(header->fwd->fwd != NULL) {
            header->fwd = header->fwd->fwd;
            header->fwd->fwd->bwd = header;
        }
        else {
            header->fwd = NULL;
        }
        return;
    }

}

