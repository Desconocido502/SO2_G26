// user.h

#ifndef USERS_H
#define USERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definición de la estructura usuario
struct user {
    int no_cuenta;
    char full_name[100];
    float saldo;
    struct user* next;
};

// Declaraciones de funciones
void insertUser(int account_number, const char* full_name, float saldo);
void deleteUser(int account_number);

// Declaración de la cabeza de la lista (debe estar en el .h si se usa extern)
extern struct user* headerUsers;

#endif // USERS_H

