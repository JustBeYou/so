#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "collatz.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <number> <number> ...", argv[0]);
        return -1;
    }

    struct arguments args = parse_args(argc - 1, argv + 1);
    parallel_collatz(args);
    delete_args(&args);

    return 0;
}

struct arguments parse_args(int n, char** raw_args) {
    struct arguments args;
    args.count = n;
    args.numbers = malloc(sizeof(int) * n);

    for (int i = 0; i < n; ++i) {
       args.numbers[i] = atoi(raw_args[i]); 
    }

    return args;
}

void delete_args(struct arguments* args) {
    free(args->numbers);
    args->count = 0;
    args->numbers = NULL;
}

void parallel_collatz(struct arguments args) {
    pid_t pids[args.count];
    for (int i = 0; i < args.count; ++i) {
       pids[i] = spawn_collatz_proc(args.numbers[i]);
       if (pids[i] == 0) return ;
    }
   
    int status;
    wait(&status);
    printf("Done. Terminal will flush. Status = %d\n", status);
}

pid_t spawn_collatz_proc(int n) {
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Could not fork. Exiting...\n");
        exit(-1);
    }

    if (pid == 0) {
        printf("Processing n = %d on proc %d\n", n, getpid());
        collatz(n);
    }

    return pid;
}

void collatz(int n) {
    printf("%d ", n);
    if (n == 1 || n == 0) {
        puts(".");
        return ;
    }

    if (n % 2 == 0) return collatz(n / 2);
    else                   collatz(3 * n + 1);
}

