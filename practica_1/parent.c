#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>

int calls = 0;
int c_open = 0;
int c_write = 0;
int c_read = 0;

// Variable compartida para indicar si se recibió la señal SIGINT
volatile sig_atomic_t sigint_received = 0;

void ctrlc_handler(int signal) {
    // Se establece la bandera al recibir la señal SIGINT
    sigint_received = 1; 
}

// Función para monitorear las llamadas al sistema de los procesos
void monitor_syscalls(int pid1, int pid2) {
    char command[100];
    sprintf(command, "%s %d %d %s", "sudo stap trace.stp ", pid1, pid2, " > syscalls.log");
    system(command);
}

//Justamente luego de recibir el ctrl+c
void stat(int pid1, int pid2, int monitor, int fd) {
    printf("\n");
    printf("-----------------------\n");
    printf("|Terminando el programa|\n");
    printf("-----------------------\n");
    char buff[1024];
    char *action;
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buff, sizeof(buff))) > 0) {
        // Busca las acciones en el buffer
        char *ptr = buff;
        while ((action = strtok(ptr, "\n")) != NULL) {
            //La siguiente llamada a strtok deberia de recibir NULL
            ptr = NULL; 
            // Aument el contador dependiendo de la acción
            if (strstr(action, "read") != NULL) {
                c_read++;
            } else if (strstr(action, "write") != NULL) {
                c_write++;
            }
        }
    }
    calls = c_open + c_read + c_write;

    // Imprimir la tabla
    printf("_____________________________________________________\n");
    printf("| Llamadas write | Llamadas read |        Total      |\n");
    printf("|----------------|---------------|-------------------|\n");
    printf("| %-14d | %-13d | %-17d |\n", c_write, c_read, calls);
    printf("-----------------------------------------------------\n");
    close(fd);
    kill(pid1,SIGKILL);
    kill(pid2,SIGKILL);
    kill(monitor, SIGKILL);
    exit(0);
    
}

int main(){
    // 
    signal(SIGINT, ctrlc_handler);

    //Se abre el archivo syscalls.log, en caso de que no exista, y si existe, se vacía
    int fd = open("syscalls.log", O_RDWR | O_CREAT | O_TRUNC, 0777);
    //Tmabien se abre practica1.txt que será utilizado por los hijos pero se cierra acá
    int file_descriptor = open("practica1.txt",  O_CREAT | O_TRUNC, 0777);
    close(file_descriptor);

    pid_t pid1 = fork(); //Se realiza el fork 1
    
    if(pid1 == -1){
        perror("fork");
        exit(1);
    }

    if(pid1 == 0){

        /*Se escriben los argumentos para el proceso hijo*/
        char *arg_Ptr[4];
        arg_Ptr[0] = " child.bin";
        arg_Ptr[1] = " Hola, ";
        arg_Ptr[2] = " Soy el proceso hijo! ";
        arg_Ptr[3] = NULL; // El ultimo indice de argv debe siempre de ser NULL

        //Se ejecuta el proceso hijo
        execv("/home/sopes2/Documents/GitHub/SO2_G26/practica_1/child.bin", arg_Ptr);

    } else {
        pid_t pid2 = fork(); //Se realiza el fork 2

        if(pid2 == -1){
            perror("fork");
            exit(1);
        }

        if(pid2 == 0){
            // Es el egundo proceso hijo
           // printf("Soy el proceso hijo NO.2\n");

            /*Se escriben los argumentos para el proceso hijo*/
            char *arg_Ptr[4];
            arg_Ptr[0] = " child.bin";
            arg_Ptr[1] = " Hola! ";
            arg_Ptr[2] = " Soy el proceso hijo! ";
            arg_Ptr[3] = NULL; // El ultimo indice de argv siempre debe de ser NULL

            /*Se ejecuta el ejecutable del proceso hijo*/
            execv("/home/sopes2/Documents/GitHub/SO2_G26/practica_1/child.bin", arg_Ptr);
        }else {

            //printf("Soy el proceso padre\n");
            // Crear un hilo para monitorear las llamadas al sistema
            pid_t monitor = fork();
            if(monitor == 0 ){
                monitor_syscalls(pid1,pid2);
            }

            //Esperar a que los procesos hijos terminen
            int status;
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);
            waitpid(monitor, &status, 0);
            stat(pid1,pid2,monitor,fd);

        }
    }
    return 0;
}