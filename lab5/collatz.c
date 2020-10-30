#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
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
    int shm_fd = create_shm(args.count); 

    for (int i = 0; i < args.count; ++i) {
       pid_t pid = spawn_collatz_proc(args.numbers[i], i, shm_fd);
       if (pid == 0) return ;
    }
   
    int status;
    while (waitpid(-1, &status, 0) != -1) {}

    printf("Done. Status = %d\n", status);

    int* results = mmap(NULL, SIZE_PER_PROC * args.count, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (results == MAP_FAILED) {
        fprintf(stderr, "Could not mmap. Exiting...\n");
        close(shm_fd);
        exit(-1);
    }

    for (int i = 0; i < args.count; ++i) {
        printf("Results for id = %d n = %d: ", i, args.numbers[i]);
        int* results_page = results + (i * NUMBERS_PER_PROC);
        for (int j = 0; j < NUMBERS_PER_PROC && results_page[j] != 0; ++j) {
            printf("%d ", results_page[j]);
        }
        puts("");
    }

    munmap(results, SIZE_PER_PROC * args.count);
    delete_shm(shm_fd);
}

pid_t spawn_collatz_proc(int n, int id, int shm_fd) {
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Could not fork. Exiting...\n");
        exit(-1);
    }

    if (pid == 0) {
        printf("Processing n = %d on proc %d (id %d)\n", n, getpid(), id);
        int* output = get_sh_output(id, shm_fd);
        collatz(n, 0, output);

        close_sh_output(&output);
        close(shm_fd);
    }

    return pid;
}

int* get_sh_output(int id, int shm_fd) {
    void* ptr = mmap(NULL, SIZE_PER_PROC, PROT_WRITE, MAP_SHARED, shm_fd, id * SIZE_PER_PROC);

    if (ptr == MAP_FAILED) {
        fprintf(stderr, "Could not mmap. Exiting...\n");
        close(shm_fd);
        exit(-1);
    }

    return ptr;
}

void close_sh_output(int** output) {
    munmap(*output, SIZE_PER_PROC);
    *output = NULL;
}

int create_shm(int procs) {
    int fd = shm_open(
            SHARED_FILENAME, 
            O_CREAT | O_RDWR,
            S_IRUSR | S_IWUSR
        );

    if (fd < 0) {
        fprintf(stderr, "Could not create shared mem. Exiting...\n");
        exit(-1);
    }

    int size = SIZE_PER_PROC * procs;
    if (ftruncate(fd, size) == -1) {
        fprintf(stderr, "Could not alloc %d bytes. Exiting...", size);
        shm_unlink(SHARED_FILENAME);
        exit(-1);
    }

    return fd;
}

void delete_shm(int fd) {
    close(fd);
    shm_unlink(SHARED_FILENAME);
}

void collatz(int n, int output_index, int* output) {
    if (output_index == NUMBERS_PER_PROC) {
        fprintf(stderr, "Process %d exceeded its output size limit.\n", getpid());
        return ;
    }

    output[output_index] = n;

    if (n == 1 || n == 0) {
        if (output_index + 1 < NUMBERS_PER_PROC) output[output_index + 1] = 0;
        return ;
    }
    if (n % 2 == 0) return collatz(n / 2, output_index + 1, output);
    else                   collatz(3 * n + 1, output_index + 1, output);
}

