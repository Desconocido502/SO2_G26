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

typedef struct {
    char mensaje[256];
} Error;

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
Error errores[MAX_ERRORS];
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
            sprintf(errores[num_errores].mensaje, "Error en el formato del objeto JSON en el registro %d: no es un objeto JSON válido", i + 1);
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
            sprintf(errores[num_errores].mensaje, "Linea #%d: Numero de cuenta no es un numero entero positivo", i + 1);
            num_errores++;
            continue;
        }

        if (balance < 0) {
            sprintf(errores[num_errores].mensaje, "Linea #%d: Saldo no puede ser menor a 0", i + 1);
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
            fprintf(file, "%s\n", errores[i].mensaje);
        }
    }

    fclose(file);
    printf("¡Usuarios cargados!\n");
}

void* loadOperations(void* arg) {
    ThreadInfo *thread_hilo = (ThreadInfo *)arg;
    cJSON *json;

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
        fclose(thread_hilo->archivo);
        return NULL;
    }

    if (!cJSON_IsArray(json)) {
        fprintf(stderr, "JSON is not an array\n");
        cJSON_Delete(json);
        free(buffer);
        fclose(thread_hilo->archivo);
        return NULL;
    }

    int num_operations = cJSON_GetArraySize(json);
    int operations_per_thread = num_operations / THREADS_OPERATIONS;
    int start_index = thread_hilo->id * operations_per_thread;
    int end_index = (thread_hilo->id == THREADS_OPERATIONS - 1) ? num_operations : start_index + operations_per_thread;

    for (int i = start_index; i < end_index; ++i) {
        cJSON *item = cJSON_GetArrayItem(json, i);

        if (!cJSON_IsObject(item)) {
            snprintf(errores[num_errores++].mensaje, sizeof(errores[num_errores].mensaje), "Error en el formato del objeto JSON en el registro %d: no es un objeto JSON válido", i + 1);
            continue;
        }

        cJSON *operation_type = cJSON_GetObjectItem(item, "operacion");
        cJSON *account1 = cJSON_GetObjectItem(item, "cuenta1");
        cJSON *account2 = cJSON_GetObjectItem(item, "cuenta2");
        cJSON *amount = cJSON_GetObjectItem(item, "monto");

        if (!cJSON_IsNumber(operation_type) || !cJSON_IsNumber(account1) || !cJSON_IsNumber(amount)) {
            snprintf(errores[num_errores++].mensaje, sizeof(errores[num_errores].mensaje), "Invalid operation object at line %d", i + 1);
            continue;
        }

        int op_type = operation_type->valueint;
        int acc1 = account1->valueint;
        int acc2 = (account2 != NULL) ? account2->valueint : 0;
        float monto = amount->valuedouble;

        if (op_type < 1 || op_type > 4) {
            snprintf(errores[num_errores++].mensaje, sizeof(errores[num_errores].mensaje), "Linea #%d: Tipo de operacion no es valido", i + 1);
            continue;
        }

        if (acc1 <= 0) {
            snprintf(errores[num_errores++].mensaje, sizeof(errores[num_errores].mensaje), "Linea #%d: Numero de cuenta no es un numero entero positivo", i + 1);
            continue;
        }

        if (monto <= 0) {
            snprintf(errores[num_errores++].mensaje, sizeof(errores[num_errores].mensaje), "Linea #%d: Monto no es un numero positivo", i + 1);
            continue;
        }

        if (op_type == 3 && acc2 <= 0) {
            snprintf(errores[num_errores++].mensaje, sizeof(errores[num_errores].mensaje), "Linea #%d: Numero de cuenta destino no es un numero entero positivo", i + 1);
            continue;
        }

        if (op_type == 1) {
            submit(acc1, monto);
            num_depositos++;
        } else if (op_type == 2) {
            removal(acc1, monto, i + 1);
            num_retiros++;
        } else if (op_type == 3) {
            transfer(acc1, acc2, monto, i + 1);
            num_transferencias++;
        }

        operaciones_leidas++;

        printf("Hilo %d - Operacion %d: Tipo: %d, Cuenta 1: %d, Cuenta 2: %d, Monto: %.2f\n", thread_hilo->id, i + 1, op_type, acc1, acc2, monto);

        if (thread_hilo->id == 0) {
            operaciones_hilo1++;
        } else if (thread_hilo->id == 1) {
            operaciones_hilo2++;
        } else if (thread_hilo->id == 2) {
            operaciones_hilo3++;
        } else {
            operaciones_hilo4++;
        }
    }

    cJSON_Delete(json);
    free(buffer);
    fclose(thread_hilo->archivo);

    return NULL;
}


void generateReportLoadOperations(){
    char filename[100];
    time_t current_time;
    struct tm *timeinfo;

    time(&current_time);
    timeinfo = localtime(&current_time);

    strftime(filename, sizeof(filename), "operaciones_%Y-%m-%d_%H-%M-%S.log", timeinfo);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error al crear archivo de reporte");
        return;
    }

    fprintf(file, "Operaciones\n");
    fprintf(file, "Fecha: %d-%02d-%02d %02d:%02d:%02d\n", 
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    fprintf(file, "Operaciones Realizadas:\n");
    fprintf(file, "Hilo #1: %d\n", operaciones_hilo1);
    fprintf(file, "Hilo #2: %d\n", operaciones_hilo2);
    fprintf(file, "Hilo #3: %d\n", operaciones_hilo3);
    fprintf(file, "Hilo #4: %d\n", operaciones_hilo4);
    fprintf(file, "Total: %d\n", operaciones_hilo1 + operaciones_hilo2 + operaciones_hilo3 + operaciones_hilo4);
    fprintf(file, "Depositos: %d\n", num_depositos);
    fprintf(file, "Retiros: %d\n", num_retiros);
    fprintf(file, "Transferencias: %d\n", num_transferencias);

    if (num_errores > 0) {
        fprintf(file, "Errores:\n");
        for (int i = 0; i < num_errores; ++i) {
            fprintf(file, "%s\n", errores[i].mensaje);
        }
    }

    fclose(file);
    printf("¡Operaciones cargadas!\n");
}

void generate_account_report(){
    cJSON *root = cJSON_CreateArray();  // Crear un array JSON
    struct user* current = headerUsers;
    while (current != NULL) {
        cJSON *usuario_obj = cJSON_CreateObject();  // Crear un objeto JSON para cada usuario

        // agregar los campos al object
        cJSON_AddNumberToObject(usuario_obj, "no_cuenta", current->no_cuenta);
        cJSON_AddStringToObject(usuario_obj, "nombre", current->full_name);
        cJSON_AddNumberToObject(usuario_obj, "saldo", current->saldo);

        // Agregar el objeto al array
        cJSON_AddItemToArray(root, usuario_obj);
        current = current->next;
    }

    //convertir la estructura cJSON al formato de texto JSON
    char *json_str = cJSON_Print(root);

    //crear el archivo de reporte y escribir el contenido JSON
    FILE *report_file = fopen("account_report.json", "w");
    if(!report_file){
        perror("Error al crear el archivo de reporte");
        cJSON_Delete(root);
        return;
    }
    fprintf(report_file, "%s", json_str);
    fclose(report_file);

    //liberar la memoria utilizada
    cJSON_Delete(root);
    free(json_str);
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
                ThreadInfo threads_operation[THREADS_OPERATIONS];
                pthread_t tid_hilos_operation[THREADS_OPERATIONS];
                for (int i = 0; i < THREADS_OPERATIONS; i++) {
                    threads_operation[i].id = i;
                    threads_operation[i].archivo = fopen(route_operations, "r");
                    pthread_mutex_init(&threads_operation[i].mutex, NULL);
                    pthread_create(&tid_hilos_operation[i], NULL, loadOperations, &threads_operation[i]);
                }

                // Esperar a que finalicen los hilos
                for (int i = 0; i < THREADS_OPERATIONS; i++) {
                    pthread_join(tid_hilos_operation[i], NULL);
                }

                generateReportLoadOperations();
                break;
            case '3':
                printf("Operaciones individuales\n");
                submenu();
                break;
            case '4':
                printf("Generar estados de cuenta\n");
                generate_account_report();
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