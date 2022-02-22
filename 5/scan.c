#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>


#define MAX_LINE_SIZE 256


void read_input_vector(const char* filename, int n, int* array);

int main(int argc, char **argv) {
    const char *filename = argv[1];
    int size = atoi(argv[2]);
    int threadNum = atoi(argv[3]);

    int* arr = malloc(sizeof(int) * size);
    read_input_vector(filename, size, arr);

    printf("file name is: %s\n", filename);
    printf("reading file...\n");
    for (int i=0; i < size; i++) {
        printf("%d\n", arr[i]); 
    } 
    printf("size of array is: %d\n", size);
    printf("number of threads to be generated is: %d\n", threadNum);
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
