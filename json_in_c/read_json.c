#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

int main() {
    // Open the file
    FILE *fp = fopen("data.json", "r");
    if (fp == NULL) {
        printf("Error: Unable to open the file.\n");
        return 1;
    }

    // Determine file size
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Allocate memory to read the file contents into a string
    char *buffer = (char *)malloc(length + 1);
    if (buffer == NULL) {
        printf("Error: Unable to allocate memory.\n");
        fclose(fp);
        return 1;
    }

    // Read the file contents into the buffer
    fread(buffer, 1, length, fp);
    buffer[length] = '\0'; // Null-terminate the string
    fclose(fp);

    // Parse the JSON data
    cJSON *json = cJSON_Parse(buffer);
    free(buffer); // Free the buffer after parsing
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        return 1;
    }

    // Access the JSON data
    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        printf("Name: %s\n", name->valuestring);
    }

    cJSON *age = cJSON_GetObjectItemCaseSensitive(json, "age");
    if (cJSON_IsNumber(age)) {
        printf("Age: %d\n", age->valueint);
    }

    cJSON *email = cJSON_GetObjectItemCaseSensitive(json, "email");
    if (cJSON_IsString(email) && (email->valuestring != NULL)) {
        printf("Email: %s\n", email->valuestring);
    }

    // Delete the JSON object
    cJSON_Delete(json);
    return 0;
}
