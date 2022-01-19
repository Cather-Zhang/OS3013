#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    char* c;
    //printf("found %d arguments\n", argc);
    if (argc == 1) {
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    }
    //printf("need to search %s\n", argv[1]);
    /**
     * @brief When not passing in a file, wgrep should read from standard input
     * 
     */
    if (argc == 2) {
        printf("Enter here: \n");      
        char *str = NULL;
        while (fgets(str, 256, stdin) != NULL) {
            c = strstr(str, argv[1]);
            if (c) printf("%s", str);   
        }

        printf("\n");
        return 0;
    }
    for (size_t i = 2; i < argc; i++) {
        //printf("opening file %s\n", argv[i]);
        
        FILE *file;
        file = fopen(argv[i], "r");
        if (file) {
            char *line = NULL;
            size_t len = 0;
            while(getline(&line, &len, file) != -1) {
                c = strstr(line, argv[1]);
                if (c) printf("%s", line);  
            }
            free(line);
            printf("\n");
            fclose(file);
        }
        else {
            printf("wgrep: cannot open file\n");
            return 1;   
        }
    }
    return 0;
}

