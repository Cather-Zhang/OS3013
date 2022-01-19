#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]) {
    if (argc == 1) printf("wgrep: searchterm [file ...]");
    char* str = argv[1];
    for (size_t i = 2; i < argc; i++) {
        int c;
        FILE *file;
        file = fopen(argv[i], "r");
        if (file) {
            while ((c = getc(file)) != "\n")
                putchar(c);
            fclose(file);
        }
        else {
            printf("wgrep: cannot open file\n");
            return 1;   
        }
    }
    return 0;
}

