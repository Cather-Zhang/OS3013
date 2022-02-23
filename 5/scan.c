#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINE_SIZE 256

void read_input_vector(const char* filename, int n, int* array);

int *in, *out, **ptr;
int chunkSize, threadNum, size;
int counter = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

void print(int array[]) {
    for (int i = 0; i < size; i ++) {
        printf("%d\n", array[i]);
    }
}

void* add(void* ptr) {
    int index = *(int*)ptr;
    for (int i = 0; i < log2((double)size); i++) {
        int off = (int) pow(2.0, (double) i);
        for (int j = 0; j < chunkSize; j++) {
            if (index + j - off >= 0) {
                out[index + j] = in[index + j] + in[index + j - off];
            }
            else {
                out[index + j] = in[index + j];
            }
        }
        /* Barrier*/
        pthread_mutex_lock(&mutex);
        counter++;
        if(counter == threadNum) {
            counter = 0;
            for (int k = 0; k < size; k++) {
                in[k] = out[k];
            }
            pthread_cond_broadcast(&cond);
        }
        else {
            while (pthread_cond_wait(&cond, &mutex) != 0);
        }
        pthread_mutex_unlock(&mutex);
        /* End of barrier */
    }
    
    
    return (void*)ptr;
}

int main(int argc, char **argv) {
    const char *filename = argv[1];
    size = atoi(argv[2]);
    threadNum = atoi(argv[3]);
    chunkSize = size / threadNum;

    in = (int*) malloc(sizeof(int) * size);
    out = (int*) malloc(sizeof(int) * size);
    read_input_vector(filename, size, in);

    pthread_t *thread = (pthread_t*) malloc(sizeof(pthread_t) * threadNum);
    ptr = (int**) malloc(sizeof(int*) * threadNum);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for (int i = 0; i < threadNum; i++) {
        ptr[i] = (int*) malloc(sizeof(int));
        *ptr[i] = i * chunkSize;
        pthread_create(&thread[i], NULL, &add, ptr[i]);
    }

    for (int i = 0; i < threadNum; i++) {
        void *r;
        pthread_join(thread[i], &r);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    //printf("final array\n");
    for (int i = 0; i < size; i ++) {
        printf("%d\n", in[i]);
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
