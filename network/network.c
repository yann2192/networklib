#include "network.h"


void loop(async_s * as)
{
    int nb, i;
    client_s * n;
    printf("[+] Loop ...\n\n");
    while(1) {
        nb = async_wait(as);
        for(i=0; i < nb; ++i){
            n = as->ev[i].data.ptr;
            if(n->func != NULL)
                n->func((void *)n);
        }
    }
}

