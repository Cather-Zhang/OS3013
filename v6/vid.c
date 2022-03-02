#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;

volatile int count_a = 0, count_b = 0;

void *go(void *args) {
	int* int_args = (int *)args;
	int index = int_args[0];
	
	if (index) {
		pthread_mutex_lock(&lock_a);
		count_a++;
		sleep(1);
		//pthread_mutex_lock(&lock_b);
		count_b++;
		sleep(1);
		//pthread_mutex_unlock(&lock_b);
		pthread_mutex_unlock(&lock_a);
	} else {
		pthread_mutex_lock(&lock_a);
		count_a++;
		sleep(1);
		//pthread_mutex_lock(&lock_a);
		count_b++;
		sleep(1);
		//pthread_mutex_unlock(&lock_a);
		pthread_mutex_unlock(&lock_a);
	}
	printf("Thread %d finishing: %d %d\n", index, count_a, count_b);
	pthread_exit(NULL);
}

int main() {
	pthread_t thread_1;
	pthread_t thread_2;
	int args_1[]= {0};
	int args_2[] = {1};
	
	pthread_create(&thread_1, NULL, go, (void *)&args_1);
	pthread_create(&thread_2, NULL, go, (void *)&args_2);
	
	pthread_join(thread_1, NULL);
	pthread_join(thread_2, NULL);
}