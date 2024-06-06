#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <string.h>
#include <sys/wait.h>

int main() {

    pid_t pid = fork();

    if(pid == -1){
        perror("fork");
        exit(1);
    }

    if(pid){
        // proceso Padre 
        printf("Soy el proceso padre - %d\n", pid);
        printf("MI PID es: %d\n", getpid());

        int status;
        wait(&status);

        if(WIFEXITED(status)){
            printf("\n El proceso hijo termino con estado: %d\n", WIFEXITED(status));
        } else{
            printf("\n Error, El proceso hijo termino con estado: %d\n", status);
        }

        printf("Terminando el proceso padre\n");

    } else {
        // proceso Hijo
        // printf("Soy el proceso hijo - %d\n", pid);
        // printf("MI PID es: %d\n", getpid());
        char *arg_Ptr[4]; //arreglo de punteros de char
        arg_Ptr[0] =  " child.bin";
        arg_Ptr[1] = " Hola";
        arg_Ptr[2] = " Soy el proceso hijo! ";
        arg_Ptr[3] = NULL;

        execv("/home/sopes2/Documents/GitHub/SO2_G26/Practica_1/child.bin", arg_Ptr);


    }

    return 0;
}