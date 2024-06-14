#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <string.h>
#include "users.h"
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include <regex.h>
#include <time.h>

#define THREADS_USERS 3
#define THREADS_OPERATIONS 4
#define MAX_LINE_LENGTH 1000
#define MAX_USERS 500
#define MAX_TRANSACIONES 220
#define MAX_ERRORS 100

struct user_data {
    int account_number;
    char full_name[100];
    float saldo;
    int line; // Para almacenar el número de línea del JSON
};

struct operation {
    int operation;
    int account_1;
    int account_2;
    float monto;
};

typedef struct {
    int thread_id;
    int start_index;
    int end_index;
    int errors;
    double work_time;
    double lock_time;
} ThreadData;

struct error_data {
    int line;
    char error_message[200];
};

struct error_data errors[MAX_ERRORS];
int num_errors = 0;

pthread_mutex_t mutex;
struct user_data users[MAX_USERS];
int num_users;

void* insert_users(void* data) {
    ThreadData* thread_data = (ThreadData*)data;
    int start = thread_data->start_index;
    int end = thread_data->end_index;
    int errors = 0;

    clock_t work_start, work_end;
    clock_t lock_start, lock_end;
    double total_work_time = 0.0;
    double total_lock_time = 0.0;

    for (int i = start; i < end; ++i) {
        work_start = clock();

        lock_start = clock();
        pthread_mutex_lock(&mutex);
        lock_end = clock();

        insertUser(users[i].account_number, users[i].full_name, users[i].saldo);
        pthread_mutex_unlock(&mutex);

        work_end = clock();

        total_work_time += (double)(work_end - work_start) / CLOCKS_PER_SEC * 1000;
        total_lock_time += (double)(lock_end - lock_start) / CLOCKS_PER_SEC * 1000;
    }

    thread_data->errors = errors;
    thread_data->work_time = total_work_time;
    thread_data->lock_time = total_lock_time;

    pthread_exit(NULL);
}

void execute_concurrent_user_insertions() {
    pthread_t threads[THREADS_USERS];
    ThreadData thread_data[THREADS_USERS];
    int users_per_thread = num_users / THREADS_USERS;

    for (int i = 0; i < THREADS_USERS; ++i) {
        thread_data[i].thread_id = i;
        thread_data[i].start_index = i * users_per_thread;
        thread_data[i].end_index = (i + 1) * users_per_thread;
        if (i == THREADS_USERS - 1) {
            thread_data[i].end_index = num_users;
        }

        pthread_create(&threads[i], NULL, insert_users, (void*)&thread_data[i]);
    }

    for (int i = 0; i < THREADS_USERS; ++i) {
        pthread_join(threads[i], NULL);
    }

    int total_errors = 0;
    double total_work_time = 0.0;
    double total_lock_time = 0.0;
    for (int i = 0; i < THREADS_USERS; ++i) {
        total_errors += thread_data[i].errors;
        total_work_time += thread_data[i].work_time;
        total_lock_time += thread_data[i].lock_time;
    }

    printf("Inserción de usuarios completada con %d errores.\n", total_errors);
    printf("Tiempo total de trabajo: %.2f milisegundos\n", total_work_time);
    printf("Tiempo total en seccion critica (con mutex): %.2f milisegundos\n", total_lock_time);
}

void read_json_file(char* filename, struct user_data* users, int* num_users) {
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
        perror("Parse JSON");
        free(buffer);
        return;
    }

    if (!cJSON_IsArray(json)) {
        perror("JSON is not Array");
        cJSON_Delete(json);
        free(buffer);
        return;
    }

    *num_users = cJSON_GetArraySize(json);
    if (*num_users > MAX_USERS) {
        perror("Exceeded maximum users");
        cJSON_Delete(json);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    int user_count = 0;
    for (int i = 0; i < *num_users; ++i) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        cJSON *account_number = cJSON_GetObjectItem(item, "no_cuenta");
        cJSON *full_name = cJSON_GetObjectItem(item, "nombre");
        cJSON *saldo = cJSON_GetObjectItem(item, "saldo");

        if (!cJSON_IsNumber(account_number) || !cJSON_IsString(full_name) || !cJSON_IsNumber(saldo)) {
            snprintf(errors[num_errors].error_message, sizeof(errors[num_errors].error_message), "Invalid JSON format at line %d", i + 1);
            errors[num_errors].line = i + 1;
            num_errors++;
            continue; // Omitir el registro con error y continuar con el siguiente
        }

        users[user_count].account_number = account_number->valueint;
        strncpy(users[user_count].full_name, full_name->valuestring, sizeof(users[user_count].full_name) - 1);
        users[user_count].full_name[sizeof(users[user_count].full_name) - 1] = '\0'; // Ensure null-terminated
        users[user_count].saldo = saldo->valuedouble;
        users[user_count].line = i + 1;
        user_count++;
        printf("Usuario %d: %d, %s, %.2f\n", user_count, users[user_count - 1].account_number, users[user_count - 1].full_name, users[user_count - 1].saldo);
    }

    *num_users = user_count;
    cJSON_Delete(json);
    free(buffer);
}

void print_error_report() {
    if (num_errors == 0) {
        printf("No hubo errores en la carga de usuarios.\n");
    } else {
        printf("Reporte de errores:\n");
        for (int i = 0; i < num_errors; ++i) {
            printf("Línea %d: %s\n", errors[i].line, errors[i].error_message);
        }
    }
}

int main() {
    headerUsers = NULL;
    
    printf("*******************************\n");
    printf("|PRACTICA 2                   |\n");
    printf("|Sistemas Operativos2         |\n");
    printf("|Carlos Soto | 201902502      |\n");
    printf("|Eduardo Agustin | 201801627  |\n");
    printf("*******************************\n\n\n");

    while(true){
        pthread_mutex_init(&mutex, NULL);
        pthread_t threads[3];
        ThreadData thread_data[3];
        pthread_t threadsT[4];
        ThreadData thread_dataT[4];
        int lines_per_thread = MAX_USERS / 3;
        int lines_per_threadT = MAX_TRANSACIONES / 4;
        char option;
        char route_users[2000];
        char route_operations[2000];
        printf("|***********************MENU***********************|\n");
        printf("|1.Cargar usuarios                                 |\n");
        printf("|2.Cargar operaciones                              |\n");
        printf("|3.Operaciones individuales                        |\n");
        printf("|4.Generar estados de cuenta                       |\n");
        printf("|5.Salir                                           |\n");
        printf("|**************************************************|\n");

        scanf("%c", &option);
        getchar(); //

        switch(option){
            case '1':
                printf("Ingrese la ruta del archivo de usuarios: ");
                scanf("%s", route_users);
                getchar();
                read_json_file(route_users, users, &num_users);
                print_error_report();

                clock_t start_time = clock(); // Inicio del contador
                execute_concurrent_user_insertions();

                clock_t end_time = clock(); // Fin del contador
                double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC *1000;
                printf("Tiempo de ejecución: %.2f milisegundos\n", time_spent);
                break;
            case '2':
                printf("Ingrese la ruta del archivo de operaciones: ");
                // Aquí debes agregar el código para manejar la carga de operaciones
                break;
            case '3':
                printf("Operaciones individuales\n");
                // Aquí debes agregar el código para manejar las operaciones individuales
                break;
            case '4':
                printf("Generar estados de cuenta\n");
                // Aquí debes agregar el código para generar estados de cuenta
                break;
            case '5':
                printf("Saliendo...\n");
                exit(0);
            default:
                printf("Opción no válida\n");
                break;
        }
    }
    return 0;
}

//gcc main.c users.c -o main -lcjson
