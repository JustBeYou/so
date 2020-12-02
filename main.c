#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/* Mailul de la facultate */
char *email = "mihail.feraru@s.unibuc.ro";

void subiect()
{
    int i = 0;
    int suma = 0;
    while ('\0' != email[i])
    {
        suma = suma + (email[i] % 4);
        i++;
    }
    suma = suma % 4;
    switch (suma)
    {
    case 0:
        puts("Trebuie sa fac lab 5, Ex 2");
        break;
    case 1:
        puts("Trebuie sa fac lab 6, Ex 2");
        break;
    case 2:
        puts("Trebuie sa fac lab 7, Ex 1");
        break;
    case 3:
        puts("Trebuie sa fac lab 7, Ex 2");
        break;
    default:
        puts("Oricare din cele de mai sus");
        break;
    }
}

typedef int** int_matrix;
typedef int*  int_matrix_line;

int_matrix A, B, C;

int_matrix alloc_matrix(int n, int m) {
    int_matrix new_matrix = malloc(sizeof(int_matrix_line) * n);
    for (int i = 0; i < n; i++) {
        new_matrix[i] = malloc(sizeof(int) * m);
    }
    return new_matrix;
}

void random_content(int_matrix matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            matrix[i][j] = rand() % 100 + 1;
        }
    }
}

void delete_matrix(int_matrix* matrix_ptr, int n) {
    int_matrix matrix = *matrix_ptr;
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
    *matrix_ptr = NULL;
}

typedef struct {
    int i, j, p;
} args_t;

void* sum_ij(void* args_ptr) {
    args_t* args = args_ptr;

    for (int k = 0; k < args->p; k++) {
        C[args->i][args->j] += A[args->i][k] * B[k][args->j];
    }

    free(args_ptr);
    return NULL;
}

void print_matrix(int_matrix matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%d ", matrix[i][j]);
        }
        puts("");
    }
}

int main(int argc, char *argv[])
{
    subiect();
    srand(time(0));

    int m, p, n;
    printf("m = "); scanf("%d", &m);
    printf("p = "); scanf("%d", &p);
    printf("n = "); scanf("%d", &n);

    A = alloc_matrix(m, p);
    B = alloc_matrix(p, n);
    C = alloc_matrix(m, n);

    random_content(A, m, p);
    random_content(B, p, n);

    puts("A =");
    print_matrix(A, m, p);
    puts("B = ");
    print_matrix(B, p, n);

    pthread_t threads[m][n];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            args_t* args = malloc(sizeof(args));
            args->i = i;
            args->j = j;
            args->p = p;
            pthread_create(&threads[i][j], NULL, sum_ij, (void*)args);
        }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            pthread_join(threads[i][j], NULL);
        }
    }

    puts("C = ");
    print_matrix(C, m, n);

    delete_matrix(&A, m);
    delete_matrix(&B, p);
    delete_matrix(&C, m);

    return 0;
}
