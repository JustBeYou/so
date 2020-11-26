#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

/* Am creat doua bariere pentru a exemplifica doua
 * metode diferite de rezolvare 
 */
unsigned int N;

pthread_mutex_t barrier_1_mtx;
sem_t barrier_1_sem;
unsigned int barrier_1_total = 0;
void barrier_1();

pthread_mutex_t barrier_2_mtx;
pthread_cond_t barrier_2_cond;
unsigned int barrier_2_total = 0;
void barrier_2();

struct arg_t {
    unsigned int id;
};
void* th_func(void*);

int main() {
    printf("N = ");
    scanf("%u", &N);

    /*
     * Prima bariera foloseste un mutex si un semafor:
     * 1. thread-ul blocheaza mutex-ul, apoi incrementeaza numaratoarea
     * 2. daca nu este al N-lea thread, asteapta la semafor
     * 3. daca este al N-lea thread, incrementeaza semaforul cu 1
     * 4. fiecare thread eliberat va incrementa semaforul cu 1 permitand inca unui thread sa treaca
     * 5. toate threadurile au trecut simultan bariera
     */
    pthread_mutex_init(&barrier_1_mtx, NULL);
    sem_init(&barrier_1_sem, 0, 0);

    /*
     * A doua bariera foloseste un mutex conditionat:
     * 1. blocheaza mutex-ul si incrementeaza numaratoarea
     * 2. daca nu este al N-lea thread, elibereaza mutex-ul si asteapta realizarea conditiei
     * 3. daca este al N-lea thread, trimite un semnal ca s-a realizat conditia
     * 4. toate threadurile au trecut simultan bariera
     */
    pthread_mutex_init(&barrier_2_mtx, NULL);
    pthread_cond_init(&barrier_2_cond, NULL);

    pthread_t threads[N];
    struct arg_t args[N];
    for (unsigned int i = 0; i < N; ++i) {
        args[i].id = i;
        pthread_create(&threads[i], NULL, &th_func, &args[i]);
    }

    for (unsigned int i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void barrier_1() {
    pthread_mutex_lock(&barrier_1_mtx);
    if (++barrier_1_total == N) sem_post(&barrier_1_sem);
    pthread_mutex_unlock(&barrier_1_mtx);

    sem_wait(&barrier_1_sem);
    sem_post(&barrier_1_sem);
}

void barrier_2() {
    pthread_mutex_lock(&barrier_2_mtx);
    if (++barrier_2_total == N) pthread_cond_broadcast(&barrier_2_cond);
    else                        pthread_cond_wait(&barrier_2_cond, &barrier_2_mtx);
    pthread_mutex_unlock(&barrier_2_mtx);
}

void* th_func(void* data) {
    struct arg_t* arg = (struct arg_t*) data;

    printf("th %u ENTER barrier 1.\n", arg->id);
    barrier_1();
    printf("th %u EXIT  barrier 1.\n", arg->id);
  
    printf("th %u ENTER barrier 2.\n", arg->id);
    barrier_2();
    printf("th %u EXIT  barrier 2.\n", arg->id);
    
    return NULL;
}
