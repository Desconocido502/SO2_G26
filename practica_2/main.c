#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h> // Para usleep
#include <time.h>
#include <cjson/cJSON.h>
#include <math.h>

#define MAX_USERS 500
#define MAX_ERRORS 250
#define MAX_OPERATIONS 550

struct user_data {
    int account_number;
    char full_name[100];
    float saldo;
    int line; // Para almacenar el número de línea del JSON
};

struct operation_data {
    int operation_type;
    int account1;
    int account2;
    float amount;
};

struct error_data {
    int line;
    char error_message[200];
};

struct operation_data operations[MAX_OPERATIONS];
int num_operations = 0;

struct error_data errors[MAX_ERRORS];
int num_errors = 0;

struct error_data operation_errors[MAX_ERRORS];
int num_operations_errors = 0;

struct user_data users[MAX_USERS];
int num_users = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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

// Function prototypes
int validar_existencia_cuenta(int account);
int validar_saldo_suficiente(int account, float amount);


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

        // Validar que el saldo sea un número real positivo
        if (saldo->valuedouble <= 0) {
            snprintf(errors[num_errors].error_message, sizeof(errors[num_errors].error_message), "Error en el campo 'saldo' en el registro %d: se espera un saldo positivo", i + 1);
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

void read_operations_json(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Open File");
        return;
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(filesize + 1);
    if (buffer == NULL) {
        perror("Allocate buffer");
        fclose(file);
        return;
    }

    fread(buffer, 1, filesize, file);
    buffer[filesize] = '\0';
    fclose(file);

    cJSON *json = cJSON_Parse(buffer);
    free(buffer);

    if (json == NULL) {
        // Manejo de errores al analizar el JSON
        snprintf(operation_errors[num_operations_errors].error_message, sizeof(operation_errors[num_operations_errors].error_message), "Error parsing JSON in file %s: %s", filename, cJSON_GetErrorPtr());
        operation_errors[num_operations_errors].line = -1;  // Línea desconocida en este contexto
        num_operations_errors++;
        return;
    }

    if (!cJSON_IsArray(json)) {
        fprintf(stderr, "JSON is not an array\n");
        cJSON_Delete(json);
        return;
    }

    for (int i = 0; i < cJSON_GetArraySize(json); ++i) {
        cJSON *item = cJSON_GetArrayItem(json, i);

        cJSON *operation = cJSON_GetObjectItem(item, "operacion");
        cJSON *account1 = cJSON_GetObjectItem(item, "cuenta1");
        cJSON *account2 = cJSON_GetObjectItem(item, "cuenta2");
        cJSON *amount = cJSON_GetObjectItem(item, "monto");

        // Validar la existencia y tipo correcto de los campos
        if (!cJSON_IsNumber(operation) || !cJSON_IsNumber(account1) || !cJSON_IsNumber(amount)) {
            snprintf(operation_errors[num_operations_errors].error_message, sizeof(operation_errors[num_operations_errors].error_message), "Error en el formato del JSON en la operación %d: se esperan números válidos", i + 1);
            operation_errors[num_operations_errors].line = i + 1;
            num_operations_errors++;
            continue;
        }

        int op_type = operation->valueint;
        int acc1 = account1->valueint;
        int acc2 = (account2 && cJSON_IsNumber(account2)) ? account2->valueint : 0; // Cuenta2 puede ser nulo o no un número válido
        float amt = amount->valuedouble;

        // Validar el tipo de operación y los datos asociados
        if (op_type < 1 || op_type > 3) {
            snprintf(operation_errors[num_operations_errors].error_message, sizeof(operation_errors[num_operations_errors].error_message), "Error en la operación %d: tipo de operación inválido", i + 1);
            operation_errors[num_operations_errors].line = i + 1;
            num_operations_errors++;
            continue;
        }

        if (op_type == 1 && acc1 <= 0) {
            snprintf(operation_errors[num_operations_errors].error_message, sizeof(operation_errors[num_operations_errors].error_message), "Error en la operación %d: número de cuenta inválido para depósito", i + 1);
            operation_errors[num_operations_errors].line = i + 1;
            num_operations_errors++;
            continue;
        }

        if (op_type == 2 && acc1 <= 0) {
            snprintf(operation_errors[num_operations_errors].error_message, sizeof(operation_errors[num_operations_errors].error_message), "Error en la operación %d: número de cuenta inválido para retiro", i + 1);
            operation_errors[num_operations_errors].line = i + 1;
            num_operations_errors++;
            continue;
        }

        if (op_type == 3 && (acc1 <= 0 || acc2 <= 0)) {
            snprintf(operation_errors[num_operations_errors].error_message, sizeof(operation_errors[num_operations_errors].error_message), "Error en la operación %d: números de cuenta inválidos para transferencia", i + 1);
            operation_errors[num_operations_errors].line = i + 1;
            num_operations_errors++;
            continue;
        }

        if (amt <= 0 || isnan(amt)) {
            snprintf(operation_errors[num_operations_errors].error_message, sizeof(operation_errors[num_operations_errors].error_message), "Error en la operación %d: monto inválido (debe ser mayor a cero)", i + 1);
            operation_errors[num_operations_errors].line = i + 1;
            num_operations_errors++;
            continue;
        }

        // Simulación de validación de existencia de cuenta y saldo suficiente (a implementar)
        if (!validar_existencia_cuenta(acc1)) {
            snprintf(operation_errors[num_operations_errors].error_message, sizeof(operation_errors[num_operations_errors].error_message), "Error en la operación %d: número de cuenta no existe", i + 1);
            operation_errors[num_operations_errors].line = i + 1;
            num_operations_errors++;
            continue;
        }

        // Simulación de validación de saldo suficiente (a implementar)
        if (op_type == 2 && !validar_saldo_suficiente(acc1, amt)) {
            snprintf(operation_errors[num_operations_errors].error_message, sizeof(operation_errors[num_operations_errors].error_message), "Error en la operación %d: saldo insuficiente para retiro", i + 1);
            operation_errors[num_operations_errors].line = i + 1;
            num_operations_errors++;
            continue;
        }

        // Almacenar la operación válida en el arreglo global protegido por mutex
        pthread_mutex_lock(&mutex);
        // Aquí deberías almacenar la operación en una estructura de datos adecuada
        pthread_mutex_unlock(&mutex);
    }

    cJSON_Delete(json);
}


// Funciones simuladas de validación de cuenta y saldo (deberían ser implementadas según tu lógica de negocio)
int validar_existencia_cuenta(int account) {
    // Recorre la lista de usuarios para verificar la existencia de la cuenta
    for (int i = 0; i < num_users; ++i) {
        if (users[i].account_number == account) {
            return 1; // La cuenta existe
        }
    }
    return 0; // La cuenta no existe
}

int validar_saldo_suficiente(int account, float amount) {
    // Recorre la lista de usuarios para verificar el saldo de la cuenta
    for (int i = 0; i < num_users; ++i) {
        if (users[i].account_number == account) {
            return users[i].saldo >= amount; // Devuelve verdadero si el saldo es suficiente
        }
    }
    return 0; // La cuenta no existe

}


// Función que ejecuta las operaciones en hilos separados
void *execute_operations(void *args) {
    int thread_id = *(int *)args;
    int operaciones_hilo = 0;

    for (int i = thread_id; i < num_operations; i += 4) {
        struct operation_data *op = &operations[i];

        // Realizar operaciones según el tipo de operación (retiro, depósito, transferencia)
        pthread_mutex_lock(&mutex);
        if (op->operation_type == 1) {
            num_depositos++;
        } else if (op->operation_type == 2) {
            num_retiros++;
        } else if (op->operation_type == 3) {
            num_transferencias++;
        }
        pthread_mutex_unlock(&mutex);

        // Contar operaciones por hilo
        operaciones_hilo++;
    }

    // Actualizar el contador global de operaciones por hilo
    pthread_mutex_lock(&mutex);
    if (thread_id == 0) {
        operaciones_hilo1 = operaciones_hilo;
    } else if (thread_id == 1) {
        operaciones_hilo2 = operaciones_hilo;
    } else if (thread_id == 2) {
        operaciones_hilo3 = operaciones_hilo;
    } else if (thread_id == 3) {
        operaciones_hilo4 = operaciones_hilo;
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}


// Función para generar el reporte de carga de usuarios
void generate_operations_report() {
    char nombre[100];
    char fecha[100];
    time_t tiempo_actual;
    struct tm *info_tiempo;
    time(&tiempo_actual);
    info_tiempo = localtime(&tiempo_actual);
    strftime(nombre, sizeof(nombre), "operaciones_%Y_%m_%d-%H_%M_%S.log", info_tiempo);
    strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", info_tiempo);
    FILE *reporte;
    reporte = fopen(nombre, "w");

    if (reporte == NULL) {
        printf("Error al generar el archivo de reporte de operaciones.\n");
        return;
    }

    fprintf(reporte, "------------------------------------------REPORTE DE CARGA DE OPERACIONES------------------------------------------\n");
    fprintf(reporte, "Resumen de operaciones\n");
    fprintf(reporte, "Fecha: %s\n", fecha);
    fprintf(reporte, "Operaciones realizadas:\n");
    fprintf(reporte, "Retiros: %d\n", num_retiros);
    fprintf(reporte, "Depósitos: %d\n", num_depositos);
    fprintf(reporte, "Transferencias: %d\n", num_transferencias);
    fprintf(reporte, "Total: %d\n", num_retiros + num_depositos + num_transferencias);
    fprintf(reporte, "Operaciones por hilo:\n");
    fprintf(reporte, "Hilo #1: %d\n", operaciones_hilo1);
    fprintf(reporte, "Hilo #2: %d\n", operaciones_hilo2);
    fprintf(reporte, "Hilo #3: %d\n", operaciones_hilo3);
    fprintf(reporte, "Hilo #4: %d\n", operaciones_hilo4);
    fprintf(reporte, "Total: %d\n", operaciones_hilo1 + operaciones_hilo2 + operaciones_hilo3 + operaciones_hilo4);
    fprintf(reporte, "Errores:\n");

    // Listar los errores en operaciones
    if (num_errors == 0) {
        fprintf(reporte, "No se encontraron errores durante la carga de operaciones.\n");
    } else {
        for (int i = 0; i < num_errors; ++i) {
            fprintf(reporte, "- Linea #%d: %s\n", operation_errors[i].line, operation_errors[i].error_message);
        }
    }

    fclose(reporte);
    printf("Reporte de operaciones generado: %s\n", nombre);
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

void print_operations() {
    printf("Operaciones cargadas:\n");
    for (int i = 0; i < num_operations; ++i) {
        printf("Operación %d:\n", i + 1);
        printf("Tipo de operación: %d\n", operations[i].operation_type);
        printf("Cuenta 1: %d\n", operations[i].account1);
        printf("Cuenta 2: %d\n", operations[i].account2);
        printf("Monto: %.2f\n", operations[i].amount);
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

// Función para generar el reporte del estado de cuentas en formato JSON
void generate_account_report(struct user_data usuarios[], int num_usuarios) {
    cJSON *root = cJSON_CreateArray();  // Crear un array JSON

    for (int i = 0; i < num_usuarios; ++i) {
        cJSON *usuario_obj = cJSON_CreateObject();  // Crear un objeto JSON para cada usuario

        // Añadir los campos al objeto JSON
        cJSON_AddNumberToObject(usuario_obj, "no_cuenta", usuarios[i].account_number);
        cJSON_AddStringToObject(usuario_obj, "nombre", usuarios[i].full_name);
        cJSON_AddNumberToObject(usuario_obj, "saldo", usuarios[i].saldo);

        // Añadir el objeto usuario al array JSON
        cJSON_AddItemToArray(root, usuario_obj);
    }

    // Convertir la estructura cJSON al formato de texto JSON
    char *json_str = cJSON_Print(root);

    // Crear el archivo de reporte y escribir el contenido JSON
    FILE *report_file = fopen("account_report.json", "w");
    if (!report_file) {
        perror("Error opening report file");
        cJSON_Delete(root);
        return;
    }
    fprintf(report_file, "%s\n", json_str);
    fclose(report_file);

    // Liberar la memoria utilizada por cJSON y la cadena JSON
    cJSON_Delete(root);
    free(json_str);
}

int main() {
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
                getchar();
                read_json_file(route_users);
                print_error_report();
                print_users();
                generate_report(); // Generar reporte al finalizar la carga
                break;
            case '2':
                printf("Ingrese la ruta del archivo de operaciones: ");
                scanf("%s", route_operations);
                getchar(); // Consumir el carácter de nueva línea residual
                read_operations_json(route_operations);

                // Crear hilos para ejecutar las operaciones
                pthread_t threads[4];
                int thread_ids[4] = {0, 1, 2, 3};

                for (int i = 0; i < 4; ++i) {
                    pthread_create(&threads[i], NULL, execute_operations, &thread_ids[i]);
                }

                // Esperar a que todos los hilos terminen
                for (int i = 0; i < 4; ++i) {
                    pthread_join(threads[i], NULL);
                }

                // Mostrar reporte de errores si los hubo
                if (num_errors > 0) {
                    printf("\nErrores encontrados durante la carga de operaciones:\n");
                    for (int i = 0; i < num_errors; ++i) {
                        printf("Registro %d: %s\n", operation_errors[i].line, operation_errors[i].error_message);
                    }
                    // Generar reporte de operaciones
                    generate_operations_report();
                } else {
                    printf("\n¡Operaciones cargadas y ejecutadas correctamente!\n");
                }

                // Limpiar datos para la próxima carga
                num_operations = 0;
                num_errors = 0;

                break;
            case '3':
                printf("Operaciones individuales\n");
                break;
            case '4':
                generate_account_report(users, num_users);
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

