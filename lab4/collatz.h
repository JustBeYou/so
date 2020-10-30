#ifndef COLLATZ_H
#define COLLATZ_H

#include <sys/types.h>

struct arguments {
    int count;
    int* numbers;
}; 

struct arguments parse_args(int n, char** raw_args);
void delete_args(struct arguments* args);

void parallel_collatz(struct arguments args);
pid_t spawn_collatz_proc(int n);

void collatz(int n);

#endif
