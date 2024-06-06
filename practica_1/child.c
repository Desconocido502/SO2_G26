#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h> // Incluir el encabezado para sig_atomic_t

int fdch; //Para controlar practica1.txt

// Variable compartida para indicar si se recibió la señal SIGINT
volatile sig_atomic_t sigint_received = 0;

void ctrlc_handler(int signal) {
    sigint_received = 1; // Establecer la bandera al recibir la señal SIGINT
    close(fdch); // Cerrar el archivo fdch
}

int main(int argc, char *argv[]){
    
    fdch = open("practica1.txt", O_RDWR, 0777); //Abro el archivo en modo lectura y escritura


}