#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h> // Incluir el encabezado para sig_atomic_t

//Para controlar practica1.txt
int fdch; 

// Variable compartida para indicar si se recibió la señal SIGINT
volatile sig_atomic_t sigint_received = 0;

void ctrlc_handler(int signal) {
    sigint_received = 1; // Establecer la bandera al recibir la señal SIGINT
    close(fdch); // Cerrar el archivo fdch
}

char random_char() {
    const char charset[] = "0123456789"
                           "abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    const int charset_size = sizeof(charset) - 1;
    return charset[rand() % charset_size];
}

int main(int argc, char *argv[]){
    // Inicializar la semilla para rand()
    srand(time(NULL) + getpid());
    
    //Abrir el archivo en modo lectura y escritura
    fdch = open("practica1.txt", O_RDWR | O_CREAT | O_TRUNC, 0777); 
    if (fdch < 0) {
        perror("Error al abrir practica1.txt");
        exit(EXIT_FAILURE);
    }

    //Controlador de señales para SIGINT (Ctrl+C)
    signal(SIGINT, ctrlc_handler);

    //Hace las llamadas al sistema hasta cuando reciba la señal SIGINT
    while (!sigint_received) {

        // Tiempo de espera entre cada operación
        int time_to_wait  = rand() % 3 + 1;
        // Tipo de operación a realizar: 1-> Open, 2-> write, 3-> Read
        int op = rand() % 3 + 1;

        // Realizar la operación correspondiente
        switch (op) {
            case 1: {
                //Para Open
                close(fdch);
                fdch = open("practica1.txt", O_RDWR);
                if (fdch < 0) {
                    perror("Error al reabrir practica1.txt");
                    exit(EXIT_FAILURE);
                }
                break;
            }
            case 2: {
                //Para Write
                char random_string[8]; // 8 caracteres alfanuméricos
                for (int i = 0; i < 8; ++i) {
                    random_string[i] = random_char();
                }
                write(fdch, random_string, 8);
                break;
            }
            case 3: {
                // Para Read
                char buff[8];
                read(fdch, buff, 8);
                break;
            }
            default:
                break;
        }
        // Espera a que pase el tiempo especificado
        sleep(time_to_wait ); 
    }

    return 0;
}