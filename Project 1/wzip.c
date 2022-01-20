#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]) {
    printf("found %d arguments\n", argc);
    int c;
    if (argc == 1) {
        printf("wzip: File1 [File2 ...]\n");
        return 1;
    }
    else {
        for (size_t i = 1; i < argc; i++) {
            int c1, c2, count = 1;
            FILE *in;
            FILE *out;
            FILE *read;
            unsigned char buffer[5];
            in = fopen(argv[i], "r");
            if (in) {
                c1 = getc(in);
                while (c1 != EOF) {
                    if (c1 == (c2 = getc(in))) {
                        count ++;
                        c1 = c2;
                    }
                    else {
                        buffer[0] = count;
                        buffer[1] = (count >> 8) & 0xFF;
                        buffer[2] = (count >> 16) & 0xFF;
                        buffer[3] = (count >> 24) & 0xFF;
                        buffer[4] = c1;
                        out = fopen("result.txt","wb");
                        fwrite(buffer,sizeof(buffer),1,out);
                        count = 1;
                    }
                }
                read = fopen("result.txt","rb");
                while ((c = getc(read)) != EOF)
                    putchar(c);
                fclose(read);
                fclose(out);
                fclose(in);
            }
            else {
                printf("wcat: cannot open file\n");
                return 1;   
            }
        }
    }
    return 0;
}