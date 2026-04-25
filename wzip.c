#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


typedef struct {
    int ch;
    int ct;
} rettype;

rettype proc_file(FILE *fp, int last, int ct);
int main(int argc, char**argv) {
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }
    rettype out;
    out.ch=-1;
    out.ct=0;
    FILE *fp;

//    int last = -1, ch, ct = 0;

    for (int i = 1; i < argc; i++) {
        if (! (fp = fopen(argv[i], "r"))) {
            fprintf(stderr, "failed to open file\n");
            exit(1);
        }
        out = proc_file(fp, out.ch, out.ct);
    }
    fwrite(&(out.ct), sizeof(int), 1, stdout);
    fputc(out.ch, stdout);
            
    return 0;

}
rettype proc_file(FILE *fp, int last, int ct){
        int ch = fgetc(fp);
        if (last == -1) last = ch;
        while (ch != EOF) {
            if (ch != last) {
                fwrite(&ct, sizeof(int), 1, stdout);
                fputc(last, stdout);
                last = ch;
                ct = 1;
            } else {
                ct++;
            }
            ch = fgetc(fp);
        }
        rettype out;
        out.ch=last;
        out.ct=ct;
        return out;
}
 
