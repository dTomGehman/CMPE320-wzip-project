#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main(int argc, char**argv) {
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    FILE *fp;

    for (int i=1; i<argc; i++){
        if (! (fp = fopen(argv[i], "r"))) {
            fprintf(stderr, "failed to open file\n");
            exit(1);
        }
        int last, ch, ct=0;
        last=ch=fgetc(fp);
        while (ch != EOF){
            if (ch!=last){
                fwrite(&ct, sizeof(int), 1, stdout);
                fputc(last, stdout);
                last=ch;
                ct=1;
            } else {
                ct++;
            }
            ch = fgetc(fp);
        }
        fwrite(&ct, sizeof(int), 1, stdout);
        fputc(last, stdout);
    }
            

    return 0;

}

