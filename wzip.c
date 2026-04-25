#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/mman.h>

typedef struct {
    int ch;
    int ct;
} rettype;

typedef struct {
    int ch;
    int ct;
    long startpos;
    long endpos; //not inclusive
    long retlength;
    uint8_t*buffer;
    char*src;
} rettype2;

rettype proc_file(FILE *fp, int last, int ct);
rettype proc_file_threaded(FILE *fp, int last, int ct, long fsize);

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
        struct stat filestats;
        fstat(fileno(fp), &filestats);
        if (filestats.st_size < 4096) 
            out = proc_file(fp, out.ch, out.ct);
        else{
//            printf("erly\n");
            out = proc_file_threaded(fp, out.ch, out.ct, filestats.st_size);
        }
        
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

void*worker(void*arg){
    int ch, last, ct;
    rettype2*r = (rettype2*) arg;
 //   printf("start %ld\n", r->startpos);
    r->retlength=0;
    int srcpos=r->startpos;
    //worst-case scenario:  5 bytes of output for 1 byte input
    r->buffer = malloc(5 * (r->endpos-r->startpos));
    if (!r->buffer) printf("malloc fail\n");

    last = ch = r->src[srcpos];
    ct=1;
    srcpos++;
    while (srcpos<r->endpos) {
        if (ch != last) {
            memcpy(r->buffer+r->retlength, &ct, sizeof(int));
            r->buffer[r->retlength+4]= (char)last;
        //    fwrite(&ct, sizeof(int), 1, stdout);
        //    fputc(last, stdout);
            last = ch;
            ct = 1;
            r->retlength+=5;
        } else {
            ct++;
        }
        srcpos++;
        ch = r->src[srcpos];
    }
    r->ch=last;
    r->ct=ct;
    return NULL;
}
/*
typedef struct {
    int ch;
    int ct;
    long startpos;
    long endpos; //not inclusive
    long retlength;
    uint8_t*buffer;
    char*src;
} rettype2;
*/

rettype proc_file_threaded(FILE *fp, int last, int ct, long fsize){
    char*src = mmap(0, fsize, PROT_READ, MAP_PRIVATE, fileno(fp), 0);
    if (!src) printf("mmap fail\n");

    /*rettype2 r1, r2, r3;
    long mid1 = fsize/3;
    long mid2 = mid1 * 2;
    r1.startpos = 0;r2.startpos = mid1; r3.startpos=mid2;
    r1.endpos = mid1; r2.endpos=mid2; r3.endpos=fsize;
    r1.src=r2.src=r3.src=src;


    pthread_t p1, p2, p3;
    pthread_create(&p1, NULL, worker, (void*) &r1);
    pthread_create(&p2, NULL, worker, (void*) &r2);
    pthread_create(&p3, NULL, worker, (void*) &r3);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
   */

    rettype2 r1;
    r1.startpos = 0;
    r1.endpos = fsize;
    r1.src=src;

    pthread_t p1;
    pthread_create(&p1, NULL, worker, (void*) &r1);
    pthread_join(p1, NULL);

    char chedge=(char) r1.buffer[4];
    if (chedge == last) { //continuous strain, combine
        int iedge; 
        memcpy(&iedge, r1.buffer, sizeof(int) );
        iedge+=ct;
        memcpy(r1.buffer, &iedge, sizeof(int) );
    } else if (last != -1){ // two different strains
        fwrite(&ct, sizeof(int), 1, stdout);
        fputc(last, stdout);
    }


    fwrite(r1.buffer, 1, r1.retlength, stdout);
    
    rettype out;
    out.ch=r1.ch;
    out.ct=r1.ct;
    return out;
}

