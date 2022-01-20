#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]) {

    int c;
    if (argc == 1) {
        printf("wzip: File1 [File2 ...]\n");
        return 1;
    }
    else {
        for (size_t i = 1; i < argc; i++) {
            unsigned char c1, c2;
            int count = 1;

            FILE *in;
            FILE *out;
            FILE *read;
            in = fopen(argv[i], "r");
            out = fopen("result.txt","wb");
            if (in) {

                c1 = getc(in);
                while (c1 != EOF) {
                    if (c1 == (c2 = getc(in))) {
                        count ++;
                        c1 = c2;
                    }
                    else {
                        printf("%d%c\n", count, c1);                    
                        fwrite(&count,sizeof(count),1,out);
                        fwrite(&c1,sizeof(c1),1,out);
                        count = 1;
                        c1 = c2;
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
                printf("wzip: cannot open file\n");
                return 1;   
            }
        }
    }
    return 0;
}