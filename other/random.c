#include "random.h"

void random_bytes(unsigned char * buffer, int size)
{
    FILE * f;

    f = fopen("/dev/urandom", "rb");
    if(f == NULL){
        perror("fopen /dev/urandom");
        exit(EXIT_FAILURE);
    }
    fread(buffer, 1, size, f);
    fclose(f);
}
