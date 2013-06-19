#ifndef __UTILS__
#define __UTILS__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int get_nb_line(char * buffer, int size);
int get_next_line(char * buffer, int size);
int get_file_size(FILE * f);
char * itoa(int i);

#endif
