#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h> // Incluir el encabezado para sig_atomic_t

//Para controlar practica1.txt
int file_descriptor; 

// Variable compartida para indicar si se recibió la señal SIGINT
volatile sig_atomic_t sigint_received = 0;

//El manejador que detecta el ctrl+c
void ctrlc_handler(int signal) {
    sigint_received = 1; // Establecer la bandera al recibir la señal SIGINT
    close(file_descriptor); // Cerrar el archivo file_descriptor
}

// Función para generar un caracter aleatorio
char generate_random_character() {
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
    file_descriptor = open("practica1.txt", O_RDWR | O_CREAT | O_TRUNC, 0777); 
    if (file_descriptor < 0) {
        perror("Error al abrir practica1.txt");
        exit(EXIT_FAILURE);
    }

    //Controlador de señales para SIGINT (Ctrl+C)
    signal(SIGINT, ctrlc_handler);

    //Hace las llamadas al sistema hasta cuando reciba la señal SIGINT
    while (!sigint_received) {

        // Tiempo de espera entre cada operación
        int time_to_wait  = rand() % 3 + 1;
        /// Tipo de operación a realizar: 1-> write, 2-> read
        int op = rand() % 2 + 1;

        // Realizar la operación correspondiente
        switch (op) {
            case 1: {
                // Para Write
                char random_string[9]; // 8 caracteres alfanuméricos
                for (int i = 0; i < 8; ++i) {
                    random_string[i] = generate_random_character();
                }
                random_string[8] = '\n'; // Agregar el salto de línea
                write(file_descriptor, random_string, 9);
                break;
            }
            case 2: {
                // Para Read
                char buff[8];
                //lseek(file_descriptor, 0, SEEK_SET); // Mover el puntero al inicio del archivo
                read(file_descriptor, buff, 8);
                break;
            }
            default:
                break;
        }
        // Espera a que pase el tiempo especificado
        sleep(time_to_wait);
    }

    return 0;
}