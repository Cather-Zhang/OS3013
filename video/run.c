#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
    int count = 1;
    while(count % 11 != 0) {
        count++;
        count *= 2;
    }
    printf("%d", count);
    return 0;
}