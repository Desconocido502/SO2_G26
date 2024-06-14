// user.h

#ifndef USERS_H
#define USERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

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
bool existAccount(int account_number);
void statusAccount();
struct user* getUser(int account_number);  // Utilizar struct aquí
void submit(int account_number, float amount);
void removal(int account_number, float amount, int line);
void transfer(int account_number, int account_number2, float amount, int line);

// Declaración de la cabeza de la lista (debe estar en el .h si se usa extern)
extern struct user* headerUsers;

#endif // USERS_H


