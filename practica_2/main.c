#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h> // Para usleep
#include <time.h>
#include <cjson/cJSON.h>
#include <math.h>
#include "users.h"

#define MAX_USERS 500
#define MAX_ERRORS 250
#define MAX_OPERATIONS 550
#define THREADS_USERS 3
#define THREADS_OPERATIONS 4

pthread_mutex_t lock;

// Variables para contar usuarios cargados por cada hilo
int usuarios_leidos;
int usuariosHilo1 = 0;
int usuariosHilo2 = 0;
int usuariosHilo3 = 0;

// Variables para contar operaciones de cada tipo
int num_retiros = 0;
int num_depositos = 0;
int num_transferencias = 0;

// Variables para contar operaciones por hilo
int operaciones_leidas;
int operaciones_hilo1 = 0;
int operaciones_hilo2 = 0;
int operaciones_hilo3 = 0;
int operaciones_hilo4 = 0;

// Variables para almacenar errores
char errores[MAX_ERRORS][100];
int num_errores = 0;

// Estructura para pasar datos a cada hilo
typedef struct thread_info {
  int id;                       // Identificador del hilo
  FILE *archivo;                // Puntero al archivo JSON
  pthread_mutex_t mutex;        // Mutex para sincronización
} ThreadInfo;

struct operation_data {
    int operation_type;
    int account1;
    int account2;
    float amount;
};

// Declaración de funciones
void* loadUsers(void* arg);

//* SUBMENU para las operaciones individuales
void submenu() {
    char option;
    int account1, account2;
    float amount;
    
    printf("|***********************OPERACIONES INDIVIDUALES***********************|\n");
    printf("|1.Deposito                                                            |\n");
    printf("|2.Retiro                                                              |\n");
    printf("|3.Transferencia                                                       |\n");
    printf("|4.Consultar cuenta                                                    |\n");
    printf("|5.Regresar                                                            |\n");
    printf("|**********************************************************************|\n");

    scanf(" %c", &option);

    switch (option) {
        case '1':
            printf("*****************DEPOSITO*****************\n");
            printf("Ingrese el número de cuenta: ");
            scanf("%d", &account1);
            printf("Ingrese el monto a depositar: ");
            scanf("%f", &amount);
            // Aquí puedes llamar a la función de depósito con account1 y amount
            break;
        case '2':
            printf("*****************RETIRO*****************\n");
            printf("Ingrese el número de cuenta: ");
            scanf("%d", &account1);
            printf("Ingrese el monto a retirar: ");
            scanf("%f", &amount);
            // Aquí puedes llamar a la función de retiro con account1 y amount
            break;
        case '3':
            printf("*****************TRANSFERENCIA*****************\n");
            printf("Ingrese el número de cuenta de origen: ");
            scanf("%d", &account1);
            printf("Ingrese el número de cuenta de destino: ");
            scanf("%d", &account2);
            printf("Ingrese el monto a transferir: ");
            scanf("%f", &amount);
            // Aquí puedes llamar a la función de transferencia con account1, account2 y amount
            break;
        case '4':
            printf("*****************CONSULTAR CUENTA*****************\n");
            printf("Ingrese el número de cuenta: ");
            scanf("%d", &account1);
            // Aquí puedes llamar a la función de consulta con account1
            break;
        case '5':
            printf("Regresando al menú principal...\n");
            return;
        default:
            printf("Opción no válida\n");
            break;
    }
}

void* loadUsers(void* arg) {
    ThreadInfo *thread_hilo = (ThreadInfo *)arg;
    cJSON *json;

    // Leer y parsear el archivo JSON
    fseek(thread_hilo->archivo, 0, SEEK_END);
    long filesize = ftell(thread_hilo->archivo);
    fseek(thread_hilo->archivo, 0, SEEK_SET);

    char *buffer = (char*)malloc(filesize + 1);
    if (buffer == NULL) {
        perror("Allocate buffer");
        fclose(thread_hilo->archivo);
        return NULL;
    }
    fread(buffer, 1, filesize, thread_hilo->archivo);
    buffer[filesize] = '\0';

    json = cJSON_Parse(buffer);
    if (json == NULL) {
        fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        free(buffer);
        return NULL;
    }

    if (!cJSON_IsArray(json)) {
        fprintf(stderr, "JSON is not an array\n");
        cJSON_Delete(json);
        free(buffer);
        return NULL;
    }

    // Dividir el trabajo entre hilos
    int num_users = cJSON_GetArraySize(json);
    int users_per_thread = num_users / THREADS_USERS;
    int start_index = thread_hilo->id * users_per_thread;
    int end_index = (thread_hilo->id == THREADS_USERS - 1) ? num_users : start_index + users_per_thread;

    for (int i = start_index; i < end_index; ++i) {
        cJSON *item = cJSON_GetArrayItem(json, i);

        // Validar que 'item' sea un objeto JSON válido
        if (!cJSON_IsObject(item)) {
            sprintf(errores[num_errores], "Error en el formato del objeto JSON en el registro %d: no es un objeto JSON válido", i + 1);
            continue;
        }

        cJSON *account_number = cJSON_GetObjectItem(item, "no_cuenta");
        cJSON *full_name = cJSON_GetObjectItem(item, "nombre");
        cJSON *saldo = cJSON_GetObjectItem(item, "saldo");

        // Validar la existencia y tipo correcto de los campos
        if (!cJSON_IsNumber(account_number) || !cJSON_IsString(full_name) || !cJSON_IsNumber(saldo)) {
            fprintf(stderr, "Invalid operation object\n");
            continue;
        }

        int acc_number = account_number->valueint;
        const char* name = full_name->valuestring;
        float balance = saldo->valuedouble;


        // Validaciones adicionales
        if (acc_number <= 0) {
            sprintf(errores[num_errores], "Linea #%d: Numero de cuenta no es un numero entero positivo", i + 1);
            num_errores++;
            continue;
        }

        if (balance < 0) {
            sprintf(errores[num_errores], "Linea #%d: Saldo no puede ser menor a 0", i + 1);
            num_errores++;
            continue;
        }

        insertUser(acc_number, name, balance);
        //printf("Hilo %d - No. Cuenta: %d, Nombre: %s, Saldo: %.2f\n", thread_hilo->id, acc_number, name, balance);

        // Actualizar contador de usuarios leídos
        usuarios_leidos++;
        if (thread_hilo->id == 0) {
            usuariosHilo1++;
        } else if (thread_hilo->id == 1) {
            usuariosHilo2++;
        } else {
            usuariosHilo3++;
        }
    }

    cJSON_Delete(json);
    free(buffer);
    fclose(thread_hilo->archivo);

    return NULL;
}

void generateReportLoadUsers(){
    char filename[100];
    time_t current_time;
    struct tm *timeinfo;

    time(&current_time);
    timeinfo = localtime(&current_time);

    strftime(filename, sizeof(filename), "carga_%Y-%m-%d_%H-%M-%S.log", timeinfo);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error al crear archivo de reporte");
        return;
    }

    fprintf(file, "Carga de usuarios\n");
    fprintf(file, "Fecha: %d-%02d-%02d %02d:%02d:%02d\n", 
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    fprintf(file, "Usuarios Cargados:\n");
    fprintf(file, "Hilo #1: %d\n", usuariosHilo1);
    fprintf(file, "Hilo #2: %d\n", usuariosHilo2);
    fprintf(file, "Hilo #3: %d\n", usuariosHilo3);
    fprintf(file, "Total: %d\n", usuariosHilo1 + usuariosHilo2 + usuariosHilo3);

    if (num_errores > 0) {
        fprintf(file, "Errores:\n");
        for (int i = 0; i < num_errores; ++i) {
            fprintf(file, "%s\n", errores[i]);
        }
    }

    fclose(file);
}


// MENU principal
void menu(){
    char route_users[200];
    char route_operations[200];

    printf("*******************************\n");
    printf("|PRACTICA 2                   |\n");
    printf("|Sistemas Operativos2         |\n");
    printf("|Carlos Soto | 201902502      |\n");
    printf("|Eduardo Agustin | 201801627  |\n");
    printf("*******************************\n\n\n");

    while (1) {
        char option;
        printf("|***********************MENU***********************|\n");
        printf("|1.Cargar usuarios                                 |\n");
        printf("|2.Cargar operaciones                              |\n");
        printf("|3.Operaciones individuales                        |\n");
        printf("|4.Generar estados de cuenta                       |\n");
        printf("|5.Salir                                           |\n");
        printf("|**************************************************|\n");

        scanf(" %c", &option);

        switch (option) {
            case '1':
                printf("Ingrese la ruta del archivo de usuarios: ");
                scanf("%s", route_users);
                getchar(); // Consumir el carácter de nueva línea residual
                // Crear 3 hilos
                ThreadInfo threads[THREADS_USERS];
                pthread_t tid_hilos[THREADS_USERS];
                for (int i = 0; i < THREADS_USERS; i++) {
                    threads[i].id = i;
                    threads[i].archivo = fopen(route_users, "r");
                    pthread_mutex_init(&threads[i].mutex, NULL);
                    pthread_create(&tid_hilos[i], NULL, loadUsers, &threads[i]);
                }
                // Esperar a que finalicen los hilos
                for (int i = 0; i < THREADS_USERS; i++) {
                    pthread_join(tid_hilos[i], NULL);
                }

                printUsers();
                generateReportLoadUsers();
                break;
            case '2':
                printf("Ingrese la ruta del archivo de operaciones: ");
                scanf("%s", route_operations);
                getchar(); // Consumir el carácter de nueva línea residual
                break;
            case '3':
                printf("Operaciones individuales\n");
                submenu();
                break;
            case '4':
                printf("Generar estados de cuenta\n");
                break;
            case '5':
                printf("Saliendo...\n");
                exit(0);
            default:
                printf("Opción no válida\n");
                break;
        }
    }
}

int main() {
    headerUsers = NULL;
    pthread_mutex_init(&lock, NULL);
    menu();
    return 0;
}