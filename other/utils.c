#include "utils.h"

int get_nb_line(char * buffer, int size)
{
    int i, res = 0;
    for(i=0; i < size; ++i) {
        if(buffer[i] == '\n') ++res;
    }
    return res;
}

int get_next_line(char * buffer, int size)
{
    int i, res = -1;
    for(i=0; i < size; ++i) {
        if(buffer[i] == '\n') {
            res = i;
            break;
        }
    }
    return res;
}

int get_file_size(FILE * f)
{
    int res;
    fseek(f, 0, SEEK_END);
    res = ftell(f);
    fseek(f, 0, SEEK_SET);
    return res;
}

char * itoa(int i)
{
    char * res;
    char buf[19+2];
    char * p = buf + 19 + 1;
    *p = '\0';

    if (i >= 0) {
        do {
            *--p = '0' + (i % 10);
            i /= 10;
        } while (i != 0);
    } else {
        do {
            *--p = '0' - (i % 10);
            i /= 10;
        } while (i != 0);
        *--p = '-';
    }
    res = malloc(strlen(p)+1);
    memcpy(res, p, strlen(p));
    res[strlen(p)] = '\0';
    return res;
}

