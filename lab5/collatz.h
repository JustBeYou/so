#ifndef COLLATZ_H
#define COLLATZ_H

#include <sys/types.h>
#include <sys/unistd.h>

#define SHARED_FILENAME "collatz"
#define SIZE_PER_PROC (getpagesize())
#define NUMBERS_PER_PROC (SIZE_PER_PROC / sizeof(int))

struct arguments {
    int count;
    int* numbers;
}; 

struct arguments parse_args(int n, char** raw_args);
void delete_args(struct arguments* args);

void parallel_collatz(struct arguments args);
int create_shm(int procs);

pid_t spawn_collatz_proc(int n, int id, int shm_fd);
int* get_sh_output(int id, int shm_fd);
void collatz(int n, int output_index, int* output);

void close_sh_output(int** output);
void delete_shm(int fd);


#endif
