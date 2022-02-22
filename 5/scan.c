#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>


#define MAX_LINE_SIZE 256


void read_input_vector(const char* filename, int n, int* array);

int *in, *out;
int chunkSize;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void* add(void* ptr) {
    pthread_mutex_lock(&mut);
    int index = *(int*)ptr;
    printf("index: %d\n", index);
    out[index] = in[index];
    for (int i = 1; i < chunkSize; i++) {
        out[index + i] = out[index + i - 1]  + in[index + i];
    }
    *(int*) ptr = out[index + chunkSize];
    pthread_mutex_unlock(&mut);
    return (void*)ptr;
}

int main(int argc, char **argv) {
    printf("start reading arguments\n");
    const char *filename = argv[1];
    int size = atoi(argv[2]);
    int threadNum = atoi(argv[3]);
    chunkSize = size / threadNum;
    printf("chunk size is: %d\n", chunkSize);
    printf("finished reading arguments\n");

    in = (int*) malloc(sizeof(int) * size);
    out = (int*) malloc(sizeof(int) * size);
    read_input_vector(filename, size, in);

    printf("file name is: %s\n", filename);
    printf("reading file...\n");
    for (int i=0; i < size; i++) {
        printf("%d\n", in[i]); 
    } 
    printf("size of array is: %d\n", size);
    printf("number of threads to be generated is: %d\n", threadNum);

    pthread_t *thread = (pthread_t*) malloc(sizeof(pthread_t) * threadNum);
    for (int i = 0; i < threadNum; i++) {
        int* a = malloc(sizeof(int));
        *a = i * chunkSize;
        pthread_create(&thread[i], NULL, &add, a);
    }

    for (int i = 0; i < threadNum; i++) {
        void *r;
        pthread_join(thread[i], &r);
        if (i != 0) {
            for (int j = 0; j < chunkSize; j++)
            out[i * chunkSize + 1] += (*(int*)r);
        }
    }
    for (int i = 0; i < size; i ++) {
        printf("%d\n", out[i]);
    }
}



void read_input_vector(const char* filename, int n, int* array)
{
  FILE *fp;
  char *line = malloc(MAX_LINE_SIZE+1);
  size_t len = MAX_LINE_SIZE;
  ssize_t read;

  fp = strcmp(filename, "-") ? fopen(filename, "r") : stdin;

  assert(fp != NULL && line != NULL);

  int index = 0;

  while ((read = getline(&line, &len, fp)) != -1)
  {
    array[index] = atoi(line);
    index++;
  }

  free(line);
  fclose(fp);
}
