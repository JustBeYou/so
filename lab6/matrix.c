#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

int** C;
struct calc_args {
    int** A;
    int** B;
    int i, j, p;
};

int **alloc_matrix(int, int);
int **read_matrix(int, int);
void delete_matrix(int**, int);
int calc_cij(int**, int**, int, int, int);
void* calc_cij_thread(void*);

int main(int argc, char **argv) {
    // Uncomment pentru exemplu
    // freopen("input", "r", stdin);

    int m, p, n;
    scanf("%d %d %d", &m, &p, &n);

    int **A = read_matrix(m, p);
    int **B = read_matrix(p, n);
    C = alloc_matrix(m, n);

    pthread_t threads[m * n];
    struct calc_args args[m * n];
    int thread_id = 0;

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            args[thread_id].A = A;
            args[thread_id].B = B;
            args[thread_id].i = i;
            args[thread_id].j = j;
            args[thread_id].p = p;

            pthread_create(&threads[thread_id], NULL, &calc_cij_thread, &args[thread_id]);
            thread_id++;
        }
    }

    for (int id = 0; id < m * n; ++id) {
        pthread_join(threads[id], NULL);
    } 

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%d ", C[i][j]);
        }
        puts("");
    }

    delete_matrix(A, m);
    delete_matrix(B, p);
    delete_matrix(C, m);

    return 0;
}

int** alloc_matrix(int n, int m) {
    int** M = malloc(sizeof(int*) * n);
    for (int i = 0; i < n; ++i) {
        M[i] = malloc(sizeof(int) * m);
    }
    return M;
}

int** read_matrix(int n, int m) {
    int** M = alloc_matrix(n, m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            scanf("%d", &M[i][j]);
        }
    } 
    return M;
}

void delete_matrix(int** M, int n) {
    for (int i = 0; i < n; ++i) {
        free(M[i]);
    }
    free(M);
}

int calc_cij(int** A, int** B, int i, int j, int p) {
    int c = 0;
    for (int k = 0; k < p; ++k) {
        c += A[i][k] * B[k][j];
    }
    return c;
}

void* calc_cij_thread(void* args) {
    struct calc_args* pargs = (struct calc_args*) args;
    C[pargs->i][pargs->j] = calc_cij(
        pargs->A,
        pargs->B,
        pargs->i,
        pargs->j,
        pargs->p
    );

    return NULL;
}
