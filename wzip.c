#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main(int argc, char**argv[]) {
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    return 0;

}

