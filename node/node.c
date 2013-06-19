#include "node.h"

job_list * all_reducer = NULL;
job_list * all_mapper = NULL;

char * host = "localhost";
char * port = "9090";

void the_loop(void * arg)
{
    client_s * c = (client_s *)arg;
    char type;
    int size;

    size = recvall(c->fd, (unsigned char *)&type, 1);
    if(size <= 0) {
        close_connection(c);
        printf("Loop : Connection lost ...\n");
        exit(EXIT_FAILURE);
    }
    switch(type) {
        case 1:
            create_reducer(c);
            break;
        case 2:
            create_mapper(c);
            break;
        case 3:
            get_data(c);
            break;
        case 4:
            get_code_reducer(c);
            break;
        case 5:
            get_code_mapper(c);
            break;
        case 6:
            get_requete(c);
            break;
        case 7:
            clean_mapper(c);
            break;
        case 8:
            clean_reducer(c);
            break;
        default:
            /* unknown type */
            close_connection(c);
            exit(EXIT_FAILURE);
    }
}

int my_system(char * command)
{
    int fsize;
    char * ncommand;

    fsize = strlen(command);
    ncommand = malloc(fsize+13);

    memcpy(ncommand, command, fsize);
    memcpy(ncommand+fsize, " 2>/dev/null", 12);
    ncommand[fsize+12] = '\0';
    fsize = system(ncommand);
    free(ncommand);
    return fsize;
}

void clean_reducer(client_s * c)
{
    ID id;
    char command[8+12];

    if(recvall(c->fd, (unsigned char*)id, sizeof(ID)) <= 0) {
        perror("recv id");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    id[8] = '\0';

    memcpy(command, "rm ", 3);
    memcpy(command+3, id, 8);
    memcpy(command+3+8, "_*.node*", 8);
    command[8+11] = '\0';
    my_system(command);
    printf("Clean reducer %s\n", id);
    delete_reducer(id);
}

void clean_mapper(client_s * c)
{
    ID id;
    char command[8+12];

    if(recvall(c->fd, (unsigned char*)id, sizeof(ID)) <= 0) {
        perror("recv id");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    id[8] = '\0';

    memcpy(command, "rm ", 3);
    memcpy(command+3, id, 8);
    memcpy(command+3+8, "_*.node*", 8);
    command[8+11] = '\0';
    my_system(command);
    printf("Clean mapper %s\n", id);
    delete_mapper(id);
}

void get_data(client_s * c)
{
    int size;
    unsigned char * buffer;
    ID id;

    if(recvall(c->fd, (unsigned char*)id, sizeof(ID)) <= 0) {
        perror("recv id");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    id[8] = '\0';
    if(recvall(c->fd, (unsigned char*)&size, 4) <= 0) {
        perror("recv data size");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    buffer = malloc(size);
    if(recvall(c->fd, (unsigned char*)buffer, size) <= 0) {
        perror("recv data");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    save_data(id, "data.node", buffer, size);
    free(buffer);
}

void get_code_reducer(client_s * c)
{
    int size;
    unsigned char * buffer;
    ID id;

    if(recvall(c->fd, (unsigned char*)id, sizeof(ID)) <= 0) {
        perror("recv id");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    id[8] = '\0';
    if(recvall(c->fd, (unsigned char*)&size, 4) <= 0) {
        perror("recv reducer size");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    buffer = malloc(size);
    if(recvall(c->fd, (unsigned char*)buffer, size) <= 0) {
        perror("recv reducer");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    save_data(id, "reducer.node.c", buffer, size);
    size = compile_reducer(id);
    sendall(c->fd, (unsigned char *)&size, 4);
    free(buffer);
}

void get_code_mapper(client_s * c)
{
    int size;
    unsigned char * buffer;
    ID id;

    if(recvall(c->fd, (unsigned char*)id, sizeof(ID)) <= 0) {
        perror("recv id");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    id[8] = '\0';
    if(recvall(c->fd, (unsigned char*)&size, 4) <= 0) {
        perror("recv mapper size");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    buffer = malloc(size);
    if(recvall(c->fd, (unsigned char*)buffer, size) <= 0) {
        perror("recv mapper");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    save_data(id, "mapper.node.c", buffer, size);
    size = compile_mapper(id);
    sendall(c->fd, (unsigned char *)&size, 4);
    free(buffer);
}

void get_requete(client_s * c)
{
    int size;
    unsigned char * buffer;
    ID id;

    if(recvall(c->fd, (unsigned char*)id, sizeof(ID)) <= 0) {
        perror("recv id");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    id[8] = '\0';
    if(recvall(c->fd, (unsigned char*)&size, 4) <= 0) {
        perror("recv requete size");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    buffer = malloc(size);
    if(recvall(c->fd, (unsigned char*)buffer, size) <= 0) {
        perror("recv requete");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    save_data(id, "requete.node.c", buffer, size);
    free(buffer);
    launch_reducer(id);
}

int launch_reducer(ID id)
{
    job_list * job = get_reducer(id);

    job->thread = new_thread(&reducer_task, (void *)job);

    return 0;
}

void * reducer_task(void * arg)
{
    int i=0;
    job_list * job = (job_list *)arg;
    char * N;
    char * filename1;
    char * filename2;
    char * filename3;
    char * command;

    filename1 = gen_filename(job->id, "reducer.node");
    filename2 = gen_filename(job->id, "requete.node");
    filename3 = gen_filename(job->id, "final.node");
    N = itoa(job->n);

    command = malloc(2+strlen(filename1)+10+strlen(N)+1+strlen(host)+1+strlen(port)+1+strlen(filename2)+3+strlen(filename3)+1);
    memcpy(command+i, "./", 2);
    i+=2;
    memcpy(command+i, filename1, strlen(filename1));
    i+=strlen(filename1);
    memcpy(command+i, " ", 1);
    i+=1;
    memcpy(command+i, job->id, 8);
    i+=8;
    memcpy(command+i, " ", 1);
    i+=1;
    memcpy(command+i, N, strlen(N));
    i+=strlen(N);
    memcpy(command+i, " ", 1);
    i+=1;
    memcpy(command+i, host, strlen(host));
    i+=strlen(host);
    memcpy(command+i, " ", 1);
    i+=1;
    memcpy(command+i, port, strlen(port));
    i+=strlen(port);
    memcpy(command+i, " ", 1);
    i+=1;
    memcpy(command+i, filename2, strlen(filename2));
    i+=strlen(filename2);
    memcpy(command+i, " > ", 3);
    i+=3;
    memcpy(command+i, filename3, strlen(filename3));
    i+=strlen(filename3);
    command[i+1] = '\0';

    printf("command : %s\n", command);
    system(command);

    free(N);
    free(filename1);
    free(filename2);
    free(filename3);
    free(command);

    exit_thread();
    return NULL;
}

void create_mapper(client_s * c)
{
    ID id;
    job_list * job = malloc(sizeof(job_list));

    printf("Create a mapper ...\n");
    if(recvall(c->fd, (unsigned char *)id, sizeof(ID)) <= 0){
        perror("Create mapper : recv id");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    id[8] = '\0';
    printf("Mapper's ID : %s\n", id);
    memcpy(job->id, id, sizeof(ID));
    add_mapper(job);
}

void create_reducer(client_s * c)
{
    ID id;
    int n;
    char type;
    job_list * job = malloc(sizeof(job_list));

    printf("Create a reducer ...\n");
    if(recvall(c->fd, (unsigned char *)id, sizeof(ID)) <= 0) {
        perror("Create reducer : recv id");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    id[8] = '\0';
    if(recvall(c->fd, (unsigned char *)&n, sizeof(n)) <= 0) {
        perror("Create reducer : recv n");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    if(recvall(c->fd, (unsigned char *)&type, sizeof(type)) <= 0) {
        perror("Create reducer : recv type");
        close_connection(c);
        exit(EXIT_FAILURE);
    }
    printf("Job's ID : %s\n", id);
    memcpy(job->id, id, sizeof(ID));
    job->n = n;
    job->type = type;
    add_reducer(job);
}

char * gen_filename(ID id, char * type)
{
    char * filename;
    int typesize = strlen(type);
    filename = malloc(8+typesize+2);
    memset(filename, 0, 8+typesize+2);

    memcpy(filename, id, 8);
    filename[8] = '_';
    memcpy(filename+9, type, typesize);
    filename[9+typesize] = '\0';

    return filename;
}

void save_data(ID id, char * type, unsigned char * buffer, int size)
{
    FILE * f;
    char * filename = gen_filename(id, type);

    printf("Data saved in : %s\n", filename);
    f = fopen(filename, "wb");
    if(f == NULL) {
        perror("fopen in save_data");
        exit(EXIT_FAILURE);
    }
    fwrite(buffer, size, 1, f);
    fclose(f);
    free(filename);
}

int compile_reducer(ID id)
{
    int fsize, res;
    char * filename;
    char * command;

    filename = gen_filename(id, "reducer.node.c");
    fsize = strlen(filename);
    command = malloc(fsize+40);
    memcpy(command, "gcc ", 4);
    memcpy(command+4, filename, fsize);
    memcpy(command+4+fsize, " -o ", 4);
    memcpy(command+4+fsize+4, id, 8);
    memcpy(command+4+fsize+4+8, "_reducer.node", 13);
    memcpy(command+4+fsize+4+8+13, " reducer.a", 10);
    command[fsize+39] = '\0';

    res = my_system(command);

    free(filename);
    free(command);
    return res;
}

int compile_mapper(ID id)
{
    int fsize, res;
    char * filename;
    char * command;

    filename = gen_filename(id, "mapper.node.c");
    fsize = strlen(filename);
    command = malloc(fsize+29);
    memcpy(command, "gcc ", 4);
    memcpy(command+4, filename, fsize);
    memcpy(command+4+fsize, " -o ", 4);
    memcpy(command+4+fsize+4, id, 8);
    memcpy(command+4+fsize+4+8, "_mapper.node", 12);
    command[fsize+28] = '\0';

    res = my_system(command);

    free(filename);
    free(command);
    return res;
}

void add_reducer(job_list * job)
{
    if(all_reducer == NULL) {
        all_reducer = job;
        all_reducer->next = job;
        all_reducer->prev = job;
    } else {
        job->next = all_reducer->next;
        job->next->prev = job;
        job->prev = all_reducer;
        all_reducer->next = job;
    }
}

job_list * get_reducer(ID id)
{
    job_list * tmp = all_reducer;
    job_list * res = NULL;
    do {
        if(tmp) {
            if(strncmp(tmp->id, id, sizeof(ID)) == 0) {
                res = tmp;
                break;
            }
            tmp = tmp->next;
        }
    } while(tmp != all_reducer);
    return res;
}

void delete_reducer(ID id)
{
    job_list * tmp = all_reducer;
    do {
        if(tmp) {
            if(strncmp(tmp->id, id, sizeof(ID)) == 0) {
                if(tmp->prev != tmp && tmp->next != tmp) {
                    tmp->prev->next = tmp->next;
                    tmp->next->prev = tmp->prev;
                } else all_reducer = NULL;
                free(tmp);
                break;
            }
            tmp = tmp->next;
        }
    } while(tmp != all_reducer);
}

void add_mapper(job_list * job)
{
    if(all_mapper == NULL) {
        all_mapper = job;
        all_mapper->next = job;
        all_mapper->prev = job;
    } else {
        job->next = all_mapper->next;
        job->next->prev = job;
        job->prev = all_mapper;
        all_mapper->next = job;
    }
}

job_list * get_mapper(ID id)
{
    job_list * tmp = all_mapper;
    job_list * res = NULL;
    do {
        if(tmp) {
            if(strncmp(tmp->id, id, sizeof(ID)) == 0) {
                res = tmp;
                break;
            }
            tmp = tmp->next;
        }
    } while(tmp != all_mapper);
    return res;
}

void delete_mapper(ID id)
{
    job_list * tmp = all_mapper;
    do {
        if(tmp) {
            if(strncmp(tmp->id, id, sizeof(ID)) == 0) {
                if(tmp->prev != tmp && tmp->next != tmp) {
                    tmp->prev->next = tmp->next;
                    tmp->next->prev = tmp->prev;
                } else all_mapper = NULL;
                free(tmp);
                break;
            }
            tmp = tmp->next;
        }
    } while(tmp != all_mapper);
}

void close_connection(client_s * c)
{
    async_del(c->as, c->fd);
    close(c->fd);
}

int main(int argc, const char *argv[])
{
    int sock;
    char type = 2;
    async_s as;
    client_s c;

    sock = sock_connect(host, port);
    async_create(&as, MAX_EVENTS);

    c.fd = sock;
    c.func = &the_loop;
    c.as = &as;
    c.data = NULL;

    async_add(&as, sock, (void *)&c, ASYNC_IN);
    printf("Connected !\n");
    sendall(sock, (unsigned char *)&type, 1);
    loop(&as);
    return 0;
}
