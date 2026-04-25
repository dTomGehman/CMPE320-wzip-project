#ifndef wzip
#define wzip
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
#endif

