#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h> 
#include <string.h>

#define THREADS_USERS 3
#define THREADS_OPERATIONS 4
#define MAX_LINE_LENGTH 1000
#define MAX_USERS 500

struct data_struct {
    int no_cuenta;
    char full_name[100];
    float saldo;
    struct usuario* next;
};

struct operation {
    int operation;
    int account_1;
    int account_2;
    float monto;
};

int main(){


    return 0;
}