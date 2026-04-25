#include "wzip.h"

rettype proc_file(FILE *fp, int last, int ct) {
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
        out.ch = last;
        out.ct = ct;
        return out;
}

void* worker(void* arg) {
    int ch, last, ct;
    rettype2* r = (rettype2*) arg;
    r->retlength = 0;
    int srcpos = r->startpos;
    //worst - case scenario:  5 bytes of output for 1 byte input
    r->buffer = malloc(5 * (r->endpos - r->startpos));
    if (!r->buffer) printf("malloc fail\n");

    last = ch = r->src[srcpos];
    ct = 1;
    srcpos++;
    while (srcpos < r->endpos) {
        if (ch != last) {
            memcpy(r->buffer + r->retlength, &ct, sizeof(int));
            r->buffer[r->retlength + 4]= (char)last;
            last = ch;
            ct = 1;
            r->retlength += 5;
        } else {
            ct++;
        }
        srcpos++;
        ch = r->src[srcpos];
    }
    r->ch = last;
    r->ct = ct;
    return NULL;
}

rettype proc_file_threaded(FILE *fp, int last, int ct, long fsize) {
    char* src = mmap(0, fsize, PROT_READ, MAP_PRIVATE, fileno(fp), 0);
    if (!src) printf("mmap fail\n");

    rettype2 r[3];
    long mid1 = fsize/3;
    long mid2 = mid1 * 2;
    r[0].startpos = 0; r[1].startpos = mid1; r[2].startpos = mid2;
    r[0].endpos = mid1; r[1].endpos = mid2; r[2].endpos = fsize;
    r[0].src = r[1].src = r[2].src = src;

    pthread_t p[3];
    for (int i = 0; i < 3; i++) {
        pthread_create(p + i, NULL, worker, (void*) (r + i));
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(p[i], NULL);
    }
    for (int i = 0; i < 3; i++) {
        char chedge = (char) r[i].buffer[4];
        if (chedge == last) { //continuous strain, combine
            int iedge; 
            memcpy(&iedge, r[i].buffer, sizeof(int) );
            iedge += ct;
            memcpy(r[i].buffer, &iedge, sizeof(int) );
        } else if (last != -1) { // two different strains
            fwrite(&ct, sizeof(int), 1, stdout);
            fputc(last, stdout);
        }

        fwrite(r[i].buffer, 1, r[i].retlength, stdout);
        last = r[i].ch; ct = r[i].ct;
    }
 

    rettype out;
    out.ch = last;
    out.ct = ct;
    return out;
}

