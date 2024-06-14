#include "users.h"

// Inicialización de la cabeza de la lista
struct user* headerUsers = NULL;

void insertUser(int account_number, const char* full_name, float saldo) {
    struct user* new_node = (struct user*)malloc(sizeof(struct user));
    if (new_node == NULL) {
        perror("No se pudo asignar memoria para nuevo usuario");
        return;
    }
    new_node->no_cuenta = account_number;
    strncpy(new_node->full_name, full_name, sizeof(new_node->full_name) - 1);
    new_node->full_name[sizeof(new_node->full_name) - 1] = '\0'; // Asegurar que esté null-terminated
    new_node->saldo = saldo;
    new_node->next = headerUsers;
    headerUsers = new_node;
}

void deleteUser(int account_number) {
    struct user* current = headerUsers;
    struct user* previous = NULL;

    while (current != NULL && current->no_cuenta != account_number) {
        previous = current;
        current = current->next;
    }

    if (current != NULL) {
        if (current == headerUsers) {
            headerUsers = current->next;
        } else {
            previous->next = current->next;
        }

        free(current);
    }
}

bool existAccount(int account_number) {
    struct user* current = headerUsers;
    while (current != NULL) {
        if (current->no_cuenta == account_number) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void statusAccount() {
    struct user* current = headerUsers;
    while (current != NULL) {
        printf("No. Cuenta: %d\n", current->no_cuenta);
        printf("Nombre: %s\n", current->full_name);
        printf("Saldo: %.2f\n", current->saldo);
        printf("\n");
        current = current->next;
    }
}

struct user* getUser(int account_number) {
    struct user* current = headerUsers;
    while (current != NULL) {
        if (current->no_cuenta == account_number) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void submit(int account_number, float amount){
    struct user* current = getUser(account_number);
    if (current != NULL) {
        current->saldo += amount;
    } else {
        perror("No se encontró la cuenta");
    }
}

void removal(int account_number, float amount, int line){
    struct user* current = getUser(account_number);
    char error[200];
    if (current != NULL) {
        if (current->saldo - amount >= 0) {
            current->saldo -= amount;
            if(line == 0){
                printf("Retreat successfully completed!\n");
            }
        } else {
            if(line >= 0){
                sprintf(error,"\t-Line #%d: The balance of  [%d]: <%10.2f> is insufficient to remove <%10.2f> \n", line, account_number, current->saldo, amount);
                printf("%s", error);
            } else {
                printf("Error: The balance of  [%d]: <%10.2f> is insufficient to remove <%10.2f> \n", account_number, current->saldo, amount); 
            }
        }
    } else {
        perror("No se encontró la cuenta");
    }
}

void transfer(int account_number, int account_number2, float amount, int line){
    struct user* source = getUser(account_number);
    struct user* destination = getUser(account_number2);
    char error[200];
    char error_2[200];
    if(destination != NULL){
        if (source != NULL) {
            if (source->saldo - amount >= 0) {
                source->saldo -= amount;
                destination->saldo += amount;
                if(line == 0){
                    printf("Transfer successfully completed!\n");
                }
            } else {
                if(line > 0){
                    sprintf(error,"\t-Line #%d: The balance of  [%d]: <%10.2f> is insufficient to transfer <%10.2f> \n", line, account_number, source->saldo, amount);
                    printf("%s", error);
                } else {
                    printf("Error: The balance of  [%d]: <%10.2f> is insufficient to transfer <%10.2f> \n", account_number, source->saldo, amount); 
                }
            }
        } else {
            perror("No se encontró la cuenta");
        }
    } else {
        if(line > 0){
            sprintf(error_2,"\t-Line #%d: The destination account [%d] does not exist! \n", line, account_number2);
            printf("%s", error_2);
        } else {
            printf("Error: The destination account [%d] does not exist! \n", account_number2); 
        }
    }
}