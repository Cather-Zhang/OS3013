#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]) {

    int c;
    if (argc == 1) {
        printf("wzip: File1 [File2 ...]\n");
        return 1;
    }
    else {
        FILE *in;
        if (argc > 2) {
            int c;
            FILE *write;
            FILE *each;
            write = fopen("combined.txt", "w");
            for (size_t i = 2; i < argc; i++) {
                each = fopen(argv[i], "r");
                while ((c = getc(each)) != EOF) {
                    fputc(c, write);
                }
            }
            fclose(write);
            in = fopen("combined.txt", "r");
        }
        else in = fopen(argv[1], "r");
        
        int c1, c2;
        int count = 1;

        FILE *out;
        FILE *read;
        
        out = fopen("result.txt","wb");
        if (in) {

            c1 = getc(in);
            while (c1 != EOF) {
                if (c1 == (c2 = getc(in))) {
                    count ++;
                    c1 = c2;
                }
                else {
                    putw(count, out);
                    fputc(c1, out);
                    count = 1;
                    c1 = c2;
                }
            }
            fclose(out);
            read = fopen("result.txt","rb");
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