#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 256

void read_input_vector(const char* filename, int n, int* array);

int *in, *out, **end;
int chunkSize;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void* add(void* ptr) {
    pthread_mutex_lock(&mut);
    int index = *(int*)ptr;
    out[index] = in[index];
    for (int i = 1; i < chunkSize; i++) {
        out[index + i] = out[index + i - 1]  + in[index + i];
    }
    *(int*) ptr = out[index + chunkSize - 1];
    pthread_mutex_unlock(&mut);
    return (void*)ptr;
}

int main(int argc, char **argv) {
    const char *filename = argv[1];
    int size = atoi(argv[2]);
    int threadNum = atoi(argv[3]);
    chunkSize = size / threadNum;

    in = (int*) malloc(sizeof(int) * size);
    out = (int*) malloc(sizeof(int) * size);
    read_input_vector(filename, size, in);

    pthread_t *thread = (pthread_t*) malloc(sizeof(pthread_t) * threadNum);
    end = (int**) malloc(sizeof(int*) * threadNum);
    for (int i = 0; i < threadNum; i++) {
        end[i] = (int*) malloc(sizeof(int));
        *end[i] = i * chunkSize;
        pthread_create(&thread[i], NULL, &add, end[i]);
    }

    for (int i = 0; i < threadNum; i++) {
        void *r;
        pthread_join(thread[i], &r);
        if (i != 0) {
            for (int j = 0; j < chunkSize; j++) {
                int add = 0;
                for (int k = 0; k < i; k++) 
                    add += *(int*)end[k];
                out[i * chunkSize + j] += add;
            }
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
