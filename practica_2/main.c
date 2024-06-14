#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <string.h>
#include "users.h"

#define THREADS_USERS 3
#define THREADS_OPERATIONS 4
#define MAX_LINE_LENGTH 1000
#define MAX_USERS 500

struct operation {
    int operation;
    int account_1;
    int account_2;
    float monto;
};

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
        perror("Parse JSON");
        free(buffer);
        return;
    }

    char *formatted_json = cJSON_Print(json);
    if (formatted_json != NULL) {
        printf("%s\n", formatted_json);
        free(formatted_json);
    }

    if (!cJSON_IsArray(json)) {
        perror("JSON is not Array");
        cJSON_Delete(json);
        free(buffer);
        return;
    }

    cJSON_Delete(json);
    free(buffer);
}

int main() {
    char* filename = "test_user.json";
    read_json_file(filename);

    headerUsers = NULL;
    insertUser(123, "Juan Perez", 1000.0);
    insertUser(2, "Maria Lopez", 2000.0);

    printf("Estado inicial de las cuentas:\n");
    statusAccount();

    printf("Eliminando la cuenta 123:\n");
    deleteUser(123);
    statusAccount();

    printf("Realizando un depósito en la cuenta 2:\n");
    submit(2, 500.0);
    statusAccount();

    printf("Realizando un retiro de la cuenta 2:\n");
    removal(2, 700.0, 0);
    statusAccount();

    printf("Realizando una transferencia de la cuenta 2 a la cuenta 3 (que no existe):\n");
    insertUser(3, "Carlos Gomez", 1500.0);
    transfer(2, 3, 300.0, 0);
    statusAccount();

    return 0;
}
