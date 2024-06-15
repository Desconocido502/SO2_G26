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
int usuariosHilo1 = 0;
int usuariosHilo2 = 0;
int usuariosHilo3 = 0;

// Variables para contar operaciones de cada tipo
int num_retiros = 0;
int num_depositos = 0;
int num_transferencias = 0;

// Variables para contar operaciones por hilo
int operaciones_hilo1 = 0;
int operaciones_hilo2 = 0;
int operaciones_hilo3 = 0;
int operaciones_hilo4 = 0;

// Estructura para pasar datos a cada hilo
typedef struct thread_info {
  int id;                       // Identificador del hilo
  FILE *archivo;                // Puntero al archivo JSON
  pthread_mutex_t mutex;        // Mutex para sincronización
  int *linea_leida_global;      // Puntero a la variable compartida
} ThreadInfo;

struct operation_data {
    int operation_type;
    int account1;
    int account2;
    float amount;
};

// Declaración de funciones
void user_load(cJSON *json);
void operation_load(cJSON *json);
void read_json_file(char* filename, char* tipo);

// Metodo para leer el archivo JSON
void read_json_file(char* filename, char* tipo) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Open File");
        return;
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char*)malloc(filesize + 1);
    if (buffer == NULL) {
        perror("Allocate buffer");
        fclose(file);
        return;
    }
    fread(buffer, 1, filesize, file);
    buffer[filesize] = '\0';
    fclose(file);

    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL) {
        fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        free(buffer);
        return;
    }

    if (!cJSON_IsArray(json)) {
        fprintf(stderr, "JSON is not an array\n");
        cJSON_Delete(json);
        free(buffer);
        return;
    }

    if(tipo == "user"){
        user_load(json);
    }else if(tipo == "operation"){
        operation_load(json);
    }

    cJSON_Delete(json);
    free(buffer);
}

void user_load (cJSON *json){
    for (int i = 0; i < cJSON_GetArraySize(json); ++i) {
        cJSON *item = cJSON_GetArrayItem(json, i);

        // Validar que 'item' sea un objeto JSON válido
        if (!cJSON_IsObject(item)) {
            fprintf(stderr, "Invalid operation object\n");
            continue; // Omitir el registro con error y continuar con el siguiente
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

        printf("No. Cuenta: %d\n", acc_number);
        printf("Nombre: %s\n", name);
        printf("Saldo: %.2f\n", balance);

    }

}

void operation_load (cJSON *json){
    
    for (int i = 0; i < cJSON_GetArraySize(json); ++i) {
        cJSON *item = cJSON_GetArrayItem(json, i);

        cJSON *operation = cJSON_GetObjectItem(item, "operacion");
        cJSON *account1 = cJSON_GetObjectItem(item, "cuenta1");
        cJSON *account2 = cJSON_GetObjectItem(item, "cuenta2");
        cJSON *amount = cJSON_GetObjectItem(item, "monto");

        // Validar la existencia y tipo correcto de los campos
        if (!cJSON_IsNumber(operation) || !cJSON_IsNumber(account1) || !cJSON_IsNumber(amount)) {
            fprintf(stderr, "Invalid operation object\n");
            continue;;
        }

        int op_type = operation->valueint;
        int acc1 = account1->valueint;
        int acc2 = (account2 && cJSON_IsNumber(account2)) ? account2->valueint : 0; // Cuenta2 puede ser nulo o no un número válido
        float amt = amount->valuedouble;

        printf("Operación: %d\n", op_type);
        printf("Cuenta 1: %d\n", acc1);
        printf("Cuenta 2: %d\n", acc2);
        printf("Monto: %.2f\n", amt);

    }

}

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
                read_json_file(route_users, "user");
                break;
            case '2':
                printf("Ingrese la ruta del archivo de operaciones: ");
                scanf("%s", route_operations);
                getchar(); // Consumir el carácter de nueva línea residual
                read_json_file(route_operations, "operation");
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
    menu();
    return 0;
}