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

// Function declaration with `void` return type
void read_json_file(char* filename) {
    // Open the JSON file for reading
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Open File");
        return;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the entire file into a buffer
    char *buffer = (char*)malloc(filesize + 1);
    if (buffer == NULL) {
        perror("Allocate buffer");
        fclose(file);
        return;
    }
    fread(buffer, 1, filesize, file);
    buffer[filesize] = '\0';

    // Close the file
    fclose(file);

    // Parse the JSON data
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

    // Check if it's a valid JSON array
    if (!cJSON_IsArray(json)) {
        perror("JSON is not Array");
        cJSON_Delete(json);
        free(buffer);
        return;
    }

    // Clean up
    cJSON_Delete(json);
    free(buffer);
}

int main() {
    char* filename = "test_user.json";
    read_json_file(filename);
    // Initialize the list of users
    headerUsers = NULL;
    insertUser(123, "Juan Perez", 1000.0);
    insertUser(2, "Maria Lopez", 2000.0);

    // Print the list of users
    struct user* current = headerUsers;
    while (current != NULL) {
        printf("No. Cuenta: %d\n", current->no_cuenta);
        printf("Nombre: %s\n", current->full_name);
        printf("Saldo: %.2f\n", current->saldo);
        printf("\n");
        current = current->next;
    }

    // Delete a user
    deleteUser(123);

    // Print the list of users after deletion
    current = headerUsers;
    while (current != NULL) {
        printf("No. Cuenta: %d\n", current->no_cuenta);
        printf("Nombre: %s\n", current->full_name);
        printf("Saldo: %.2f\n", current->saldo);
        printf("\n");
        current = current->next;
    }
    return 0;
}
