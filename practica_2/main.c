#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <cjson/cJSON.h>

#define MAX_USERS 500
#define MAX_ERRORS 100

struct user_data {
    int account_number;
    char full_name[100];
    float saldo;
    int line; // Para almacenar el número de línea del JSON
};

struct error_data {
    int line;
    char error_message[200];
};

struct error_data errors[MAX_ERRORS];
int num_errors = 0;

struct user_data users[MAX_USERS];
int num_users = 0;

pthread_mutex_t mutex;
// Variables para contar usuarios cargados por cada hilo
int usuariosHilo1 = 0;
int usuariosHilo2 = 0;
int usuariosHilo3 = 0;

void read_json_file(char* filename) {
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

    // Arrays para controlar números de cuenta duplicados
    int *account_numbers = (int *)malloc(cJSON_GetArraySize(json) * sizeof(int));
    if (account_numbers == NULL) {
        perror("Allocate account_numbers");
        cJSON_Delete(json);
        free(buffer);
        return;
    }

    memset(account_numbers, 0, cJSON_GetArraySize(json) * sizeof(int));

    int user_count = 0;
    for (int i = 0; i < cJSON_GetArraySize(json); ++i) {
        cJSON *item = cJSON_GetArrayItem(json, i);

        // Validar que 'item' sea un objeto JSON válido
        if (!cJSON_IsObject(item)) {
            snprintf(errors[num_errors].error_message, sizeof(errors[num_errors].error_message), "Error en el formato del objeto JSON en el registro %d: no es un objeto JSON válido", i + 1);
            errors[num_errors].line = i + 1;
            num_errors++;
            continue; // Omitir el registro con error y continuar con el siguiente
        }

        cJSON *account_number = cJSON_GetObjectItem(item, "no_cuenta");
        cJSON *full_name = cJSON_GetObjectItem(item, "nombre");
        cJSON *saldo = cJSON_GetObjectItem(item, "saldo");

        // Validar la existencia y tipo correcto de los campos
        if (account_number == NULL || !cJSON_IsNumber(account_number)) {
            snprintf(errors[num_errors].error_message, sizeof(errors[num_errors].error_message), "Error en el campo 'no_cuenta' en el registro %d: se espera un número de cuenta válido", i + 1);
            errors[num_errors].line = i + 1;
            num_errors++;
            continue; // Omitir el registro con error y continuar con el siguiente
        }

        if (full_name == NULL || !cJSON_IsString(full_name)) {
            snprintf(errors[num_errors].error_message, sizeof(errors[num_errors].error_message), "Error en el campo 'nombre' en el registro %d: se espera un nombre válido", i + 1);
            errors[num_errors].line = i + 1;
            num_errors++;
            continue; // Omitir el registro con error y continuar con el siguiente
        }

        if (saldo == NULL || !cJSON_IsNumber(saldo) || saldo->valuedouble < 0) {
            snprintf(errors[num_errors].error_message, sizeof(errors[num_errors].error_message), "Error en el campo 'saldo' en el registro %d: se espera un saldo válido y positivo", i + 1);
            errors[num_errors].line = i + 1;
            num_errors++;
            continue; // Omitir el registro con error y continuar con el siguiente
        }

        // Validar número de cuenta duplicado
        if (account_numbers[account_number->valueint] != 0) {
            snprintf(errors[num_errors].error_message, sizeof(errors[num_errors].error_message), "Error en el registro %d: número de cuenta duplicado", i + 1);
            errors[num_errors].line = i + 1;
            num_errors++;
            continue; // Omitir el registro con error y continuar con el siguiente
        }

        account_numbers[account_number->valueint] = 1; // Marcar número de cuenta como utilizado

        // Almacenar los datos válidos en la estructura de usuarios
        users[user_count].account_number = account_number->valueint;
        strncpy(users[user_count].full_name, full_name->valuestring, sizeof(users[user_count].full_name) - 1);
        users[user_count].full_name[sizeof(users[user_count].full_name) - 1] = '\0'; // Asegurar que esté terminado en nulo
        users[user_count].saldo = saldo->valuedouble;
        users[user_count].line = i + 1;
        user_count++;

        // Contar usuarios por hilo
        if (user_count % 3 == 1) {
            usuariosHilo1++;
        } else if (user_count % 3 == 2) {
            usuariosHilo2++;
        } else {
            usuariosHilo3++;
        }
    }

    num_users = user_count;

    cJSON_Delete(json);
    free(buffer);
    free(account_numbers); // Liberar la memoria utilizada por account_numbers
}


void generate_report() {
    char nombre[100];
    char fecha[100];
    time_t tiempo_actual;
    struct tm *info_tiempo;
    time(&tiempo_actual);
    info_tiempo = localtime(&tiempo_actual);
    strftime(nombre, sizeof(nombre), "carga_%Y_%m_%d-%H_%M_%S.log", info_tiempo);
    strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", info_tiempo);
    FILE *reporte;
    reporte = fopen(nombre, "w");

    if (reporte == NULL) {
        printf("Error al generar el archivo de reportes de usuario.\n");
        return;
    }

    fprintf(reporte, "------------------------------------------CARGA DE USUARIOS------------------------------------------\n");
    fprintf(reporte, "Fecha: %s\n", fecha);
    fprintf(reporte, "Usuarios Cargados:\n");
    fprintf(reporte, "Hilo #1: %d\n", usuariosHilo1);
    fprintf(reporte, "Hilo #2: %d\n", usuariosHilo2);
    fprintf(reporte, "Hilo #3: %d\n", usuariosHilo3);
    fprintf(reporte, "Total: %d\n", num_users);
    fprintf(reporte, "Errores:\n");
    
    if (num_errors == 0) {
        fprintf(reporte, "No se encontraron errores durante la carga de usuarios.\n");
    } else {
        for (int i = 0; i < num_errors; ++i) {
            fprintf(reporte, "-\n");
            fprintf(reporte, "Linea #%d: %s\n", errors[i].line, errors[i].error_message);
        }
    }

    fclose(reporte);
    printf("Reporte generado: %s\n", nombre);
}



void print_users() {
    printf("Usuarios cargados:\n");
    for (int i = 0; i < num_users; ++i) {
        printf("Usuario %d:\n", i + 1);
        printf("Número de cuenta: %d\n", users[i].account_number);
        printf("Nombre completo: %s\n", users[i].full_name);
        printf("Saldo: %.2f\n", users[i].saldo);
        printf("Línea en JSON: %d\n", users[i].line);
        printf("\n");
    }
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
    char route_users[200];

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
                getchar();
                read_json_file(route_users);
                print_error_report();
                print_users();
                generate_report(); // Generar reporte al finalizar la carga
                break;
            case '2':
                printf("Cargar operaciones\n");
                break;
            case '3':
                printf("Operaciones individuales\n");
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

    return 0;
}

