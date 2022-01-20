#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]) {

    int c;
    if (argc == 1) {
        printf("wunzip: file1 [file2 ...]\n");
        return 1;
    }
    else {
        FILE *in;
        if (argc > 2) {
            int c;
            FILE *write;
            FILE *each;
            write = fopen("combined.txt", "wb");
            for (size_t i = 1; i < argc; i++) {
                each = fopen(argv[i], "rb");
                while ((c = getc(each)) != EOF)
                    fputc(c, write);
            }
            fclose(write);
            in = fopen("combined.txt", "r");
        }
        else in = fopen(argv[1], "rb");
        
        int count = 1;

        FILE *out;
        FILE *read;
        
        out = fopen("result.txt","w");
        while(out == NULL){
            out = fopen("result.txt","w");
        }
        if (in) {
            count = getw(in);
            while (count != EOF) {
                c = getc(in);
                for (size_t i = 0; i < count; i++) {
                    fputc(c, out); 
                }
                count = getw(in);
            }
            fclose(out);
            read = fopen("result.txt","r");
            while ((c = getc(read)) != EOF)
                putchar(c);
            fclose(read);
            fclose(in);
        }
        else {
            printf("wzip: cannot open file\n");
            return 1;   
        }      
    }
    return 0;
}