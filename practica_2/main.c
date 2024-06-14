#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h> // Para usleep
#include <time.h>
#include <cjson/cJSON.h>
#include <math.h>
#include "users.h"

#define MAX_USERS 500
#define MAX_ERRORS 250
#define MAX_OPERATIONS 550
#define THREADS_USERS 3
#define THREADS_OPERATIONS 4

//* SUBMENU para las operaciones individuales
void submenu() {
    char option;
    int account1, account2;
    float amount;
    
    printf("|***********************OPERACIONES INDIVIDUALES***********************|\n");
    printf("|1.Deposito                                                            |\n");
    printf("|2.Retiro                                                              |\n");
    printf("|3.Transferencia                                                       |\n");
    printf("|4.Consultar cuenta                                                    |\n");
    printf("|5.Regresar                                                            |\n");
    printf("|**********************************************************************|\n");

    scanf(" %c", &option);

    switch (option) {
        case '1':
            printf("*****************DEPOSITO*****************\n");
            printf("Ingrese el número de cuenta: ");
            scanf("%d", &account1);
            printf("Ingrese el monto a depositar: ");
            scanf("%f", &amount);
            // Aquí puedes llamar a la función de depósito con account1 y amount
            break;
        case '2':
            printf("*****************RETIRO*****************\n");
            printf("Ingrese el número de cuenta: ");
            scanf("%d", &account1);
            printf("Ingrese el monto a retirar: ");
            scanf("%f", &amount);
            // Aquí puedes llamar a la función de retiro con account1 y amount
            break;
        case '3':
            printf("*****************TRANSFERENCIA*****************\n");
            printf("Ingrese el número de cuenta de origen: ");
            scanf("%d", &account1);
            printf("Ingrese el número de cuenta de destino: ");
            scanf("%d", &account2);
            printf("Ingrese el monto a transferir: ");
            scanf("%f", &amount);
            // Aquí puedes llamar a la función de transferencia con account1, account2 y amount
            break;
        case '4':
            printf("*****************CONSULTAR CUENTA*****************\n");
            printf("Ingrese el número de cuenta: ");
            scanf("%d", &account1);
            // Aquí puedes llamar a la función de consulta con account1
            break;
        case '5':
            printf("Regresando al menú principal...\n");
            return;
        default:
            printf("Opción no válida\n");
            break;
    }
}

// MENU principal
void menu(){
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
                getchar(); // Consumir el carácter de nueva línea residual
                break;
            case '2':
                printf("Ingrese la ruta del archivo de operaciones: ");
                scanf("%s", route_operations);
                getchar(); // Consumir el carácter de nueva línea residual
                break;
            case '3':
                printf("Operaciones individuales\n");
                submenu();
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
}

int main() {
    headerUsers = NULL;
    menu();
    return 0;
}