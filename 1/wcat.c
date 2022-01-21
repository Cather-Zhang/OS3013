#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]) {
    for (size_t i = 1; i < argc; i++) {
        int c;
        FILE *file;
        file = fopen(argv[i], "r");
        if (file) {
            while ((c = getc(file)) != EOF)
                putchar(c);
            fclose(file);
        }
        else {
            printf("wcat: cannot open file\n");
            return 1;   
        }
    }
    return 0;
}
