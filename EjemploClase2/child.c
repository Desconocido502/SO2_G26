#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // printf("\nSoy el proceso hijo\n");
    // printf("MI PID es: %d\n", getpid());

    printf("Argumento 1: %s\n", argv[1]);
    printf("Argumento 1: %s\n", argv[2]);

    sleep(4);

    exit(0);
}