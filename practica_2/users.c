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
