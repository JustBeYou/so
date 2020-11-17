#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void* strrev_thread(void* arg) {
    char* s = arg;
    int len = strlen(s); 
    char* revs = malloc(len + 1);

    for (int i = 0; i < len; ++i) {
        revs[len - i - 1] = s[i];
    }
    revs[len] = 0;

    return (void*)revs;
} 

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <string>", argv[0]);
        exit(-1);
    }

    pthread_t th; 
    if (pthread_create(&th, NULL, &strrev_thread, argv[1]) != 0) {
        perror("failed to create thread");
        exit(-1);
    }

    char *result;
    if (pthread_join(th, (void**) &result) != 0) {
        perror("failed to join");
        exit(-1);
    }
    
    printf("%s\n", result);

    return 0;
}
