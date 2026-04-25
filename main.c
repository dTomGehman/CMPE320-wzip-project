#include "wzip.h"

int main(int argc, char**argv) {
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }
    rettype out;
    out.ch = -1;
    out.ct = 0;
    FILE *fp;


    for (int i = 1; i < argc; i++) {
        if (! (fp = fopen(argv[i], "r"))) {
            fprintf(stderr, "failed to open file\n");
            exit(1);
        }
        struct stat filestats;
        fstat(fileno(fp), &filestats);
        if (filestats.st_size < 4096) 
            out = proc_file(fp, out.ch, out.ct);
        else{
            out = proc_file_threaded(fp, out.ch, out.ct, filestats.st_size);
        }
        fclose(fp);
        
    }
    fwrite(&(out.ct), sizeof(int), 1, stdout);
    fputc(out.ch, stdout);
    return 0;

}
