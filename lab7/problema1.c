#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_RESOURCES 5
#define OK 0
#define FAILED -1

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
int available_resources = MAX_RESOURCES;

int decrease_count(int count) {
    int ret = OK;

    pthread_mutex_lock(&mtx);
    if (available_resources < count) {
        printf("failed to take %d from %d\n", count, available_resources);
        ret = FAILED;
    } else {
        printf("Got %d, remaining %d\n", count, available_resources - count);
        available_resources -= count;
    }
    pthread_mutex_unlock(&mtx);

    return ret;
}

int increase_count(int count) {
    pthread_mutex_lock(&mtx);
    available_resources += count;
    pthread_mutex_unlock(&mtx);
    return OK;
} 

void* thfunc(void* arg) {
    pthread_mutex_lock(&mtx);
    int res = rand() % MAX_RESOURCES+1;
    pthread_mutex_unlock(&mtx);
    printf("want %d\n", res);
    for (int i = 0; i < 10; i++) {
        while (decrease_count(res) == FAILED) {}  
        increase_count(res);      
        sleep(1);
    }
}

int main() {
    srand(time(0));

    pthread_t th[10];
    for (int i = 0; i < 10; i++) {
       pthread_create(&th[i], NULL, &thfunc, NULL); 
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(th[i], NULL);
    }

    return 0;
} 