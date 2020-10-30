#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    }

    if (pid == 0) {
        printf("I am the child %d\n", getpid());
        execve("/usr/bin/ls", argv, NULL);
    } else {
        printf("I am the master %d\n", pid);
    }

    return 0;
}
