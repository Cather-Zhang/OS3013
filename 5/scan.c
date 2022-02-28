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
int chunkSize, threadNum, size, counter = 0;
//pthread_mutex_t mutex;
//pthread_cond_t cond;

sem_t mutex;
sem_t barrier1;
sem_t barrier2;

void* add(void* ptr) {
    int index = *(int*)ptr;
    for (int i = 1; i <= size/2; i = i * 2) {
        for (int j = 0; j < chunkSize; j++) {
            if (index + j - i >= 0) {
                out[index + j] = in[index + j] + in[index + j - i];
            }
            else {
                out[index + j] = in[index + j];
            }
        }
        
        /* Barrier*/
        /*pthread_mutex_lock(&mutex);
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
        pthread_mutex_unlock(&mutex);*/
        /* End of barrier */


        /*phased semaphore barrier*/
        // mutex 1, barrier1 0, barrier2 1

        sem_wait(&mutex); 
        counter++;
        if (counter == threadNum) {
            sem_wait(&barrier2); 
            sem_post(&barrier1); 

            for (int k = 0; k < size; k++) {
                in[k] = out[k];
            }
        }
        sem_post(&mutex); 

        sem_wait(&barrier1); 
        sem_post(&barrier1); 

        sem_wait(&mutex); 
        counter--;
      
        if (counter == 0) {
            sem_wait(&barrier1); 
            sem_post(&barrier2); 
        }
        sem_post(&mutex); 

        sem_wait(&barrier2); 
        sem_post(&barrier2);
        /*end of barrier*/
        
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

    //pthread_mutex_init(&mutex, NULL);
    //pthread_cond_init(&cond, NULL);
    sem_init(&mutex, 0, 1);
    sem_init(&barrier1, 0, 0);
    sem_init(&barrier2, 0, 1);


    for (int i = 0; i < threadNum; i++) {
        ptr[i] = (int*) malloc(sizeof(int));
        *ptr[i] = i * chunkSize;
        pthread_create(&thread[i], NULL, &add, ptr[i]);
    }

    for (int i = 0; i < threadNum; i++) {
        void *r;
        pthread_join(thread[i], &r);
    }

    //pthread_mutex_destroy(&mutex);
    //pthread_cond_destroy(&cond);
    sem_destroy(&mutex);
    sem_destroy(&barrier1);
    sem_destroy(&barrier2);

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
