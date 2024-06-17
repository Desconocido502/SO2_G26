# SO2 - Práctica 2 -- MULTITHREADING -- JUN2024

### Grupo 26

<table>
    <thead>
        <tr>
            <th>Nombre</th>
            <th>Carnet</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <th>Carlos Eduardo Soto Marroquin</th>
            <th>201902502</th>
        </tr>
        <tr>
            <th>Eduardo René Agustin Mendoza</th>
            <th>201801627</th>
        </tr>
    </tbody>
</table>

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <cjson/cJSON.h>
#include <math.h>
#include "users.h"
```

<ol>
  <li><p align="justify"><b>stdio.h</b>: Esta librería proporciona funciones para entrada y salida estándar, como printf() y scanf(), así como manipulación de archivos con funciones como fopen() y fclose().</p></li>

  <li><p align="justify"><b>stdlib.h</b>: Contiene funciones para la gestión de memoria dinámica, conversión de tipos de datos y otras utilidades generales, como malloc() y free() para la gestión de la memoria dinámica y atoi() para convertir cadenas a enteros.</p></li>

  <li><p align="justify"><b>string.h</b>: Ofrece funciones para manipular cadenas de caracteres, como strlen() para obtener la longitud de una cadena y strcpy() para copiar cadenas.</p></li>

  <li><p align="justify"><b>pthread.h</b>: Proporciona una API para crear y gestionar hilos de ejecución, así como mecanismos de sincronización como mutexes.</p></li>

  <li><p align="justify"><b>unistd.h</b>: Proporciona acceso a las llamadas al sistema y API de Unix, como operaciones de suspensión, creación de procesos, y comunicación entre procesos.</p></li>

  <li><p align="justify"><b>time.h</b>: Proporciona funciones para obtener y manipular la hora y fecha del sistema, así como medir el tiempo de ejecución del programa. </p></li>

  <li><p align="justify"><b>cjson/cJSON.h</b>: Librería para el manejo de archivos JSON en C.</p></li>

  <li><p align="justify"><b>math.h</b>: Proporciona funciones matemáticas comunes, como sqrt() para la raíz cuadrada y pow() para la potencia.</p></li>

  <li><p align="justify"><b>users.h</b>: Archivo de cabecera que contiene la definición de la estructura de datos para los usuarios.</p></li>

</ol>

## Descripción sobre la práctica

Las corporaciones utilizan computadoras de alto rendimiento llamados mainframes para aplicaciones que dependen de la escalabilidad y la confiabilidad. Por ejemplo, una institución bancaria podría utilizar un mainframe para albergar la base de datos de las cuentas de sus 
clientes, para las cuales se pueden enviar transacciones desde cualquiera de los miles de cajeros automáticos en todo el mundo.

La práctica consiste en realizar una aplicación en consola en C que permita almacenar los datos 
de usuario de un banco, así como poder realizar operaciones monetarias como depósitos, retiros y transacciones.


# Menú

Está opción nos desplegara varias opciones por las cuales nosotros podremos ir navegando durante el flujo del programa.

![Menu](/practica_2/Imagenes/Docu.jpeg)

## Carga Masiva de Usuarios 
Al iniciar la ejecución de la aplicación se podrá realizar una carga masiva de usuarios. Esta se realizará por medio de un archivo JSON proporcionado por el usuario por medio de la ruta del archivo, este tendrá

la siguiente sintaxis:

Donde: 

* no_cuenta: Este es el numero de cuenta del usuario y deberá de ser único en el sistema.
   
* nombre: Nombre del usuario. 
  
* saldo: Saldo actual que posee la cuenta. 

La carga de estos datos deberá de realizarse en 3 hilos de forma paralela.

Durante la carga el sistema deberá de ser capaz de capturar los registros con error, omitirlos y listarlos en un reporte (ver sección de Reportes).

![CARGA_U](/practica_2/Imagenes/Usuarios.png)

## Carga Masiva de Operaciones

Debido a que los mainframes no realizan solo una operación a la vez se realizara una carga masiva de 
operaciones que el sistema deberá de realizar de manera concurrente en 4 hilos. Esta carga se realizará 
por medio de un JSON con la siguiente sintaxis:

Donde:

* Operacion: Esta será la operación que se realizará, estas pueden ser:
  
  * 1 -> Deposito 
  * 2 -> Retiro 
  * 3 -> Transferencia 

* cuenta1:
  * Deposito: Si la operación es ‘deposito’ está será la cuenta donde se depositará el dinero. 
  * Retiro: Si la operación es ‘retiro’ está será la cuenta de donde se retirará el dinero. 
  * Transferencia: Si la operación es ‘transferencia’ esta es la cuenta de donde se retirará el monto a transferir. 

* cuenta2:  
  * Deposito: Si la operación es ‘deposito’ no se utiliza este campo. 
  * Retiro: Si la operación es ‘retiro’ no se utiliza este campo. 
  * Transferencia: Si la operación es ‘transferencia’ esta es la cuenta de donde se depositará el monto transferido. 
* Monto: Cantidad de dinero que será retirado, depositado o transferido. 

![Cargar_T](/practica_2/Imagenes/Transacciones.png)

## Reportes 
Para una empresa como un banco es importante tener reportes sobre la ejecución para que se puedan 
asegurar del correcto funcionamiento del sistema, los reportes que se solicitaran son: 

### Estado de cuentas 
Desde el menú, un operador podrá generar un reporte que mostrara la información de los usuarios, este 
será escrito en un JSON y deberá mostrar la siguiente información: 

![Reportes](/practica_2/Imagenes/Reportes.png)

* Donde: 
  * no_cuenta: Este es el número de cuenta del usuario y deberá de ser único en el sistema. 
  * nombre: Nombre del usuario. 
  * saldo: Saldo actual que posee la cuenta. 

Al tener esto podremos ir cargandolos al sistema 

![Cargar](/practica_2/Imagenes/Docu2.jpeg)

Y con esto ya tendríamos la carga de los log de salida

![Cargas](/practica_2/Imagenes/Carga.png)

![Operaciones](/practica_2/Imagenes/Operaciones.png)

## Reporte de carga de usuarios 
El reporte de carga se realizará de manera automática al terminar la carga masiva de usuarios en un 
archivo llamado “carga_ yyyy_MM_dd-HH_mm_ss .log”, teniendo la fecha en la que se generó el 
reporte. El contenido del reporte es 

![Log1](/practica_2/Imagenes/Log1.png)

## Reporte de carga de operaciones masivas 
Cuando se realice una carga masiva de operaciones se realizará de manera automática un reporte de las 
operaciones realizadas en un archivo llamado “operaciones_ yyyy_MM_dd-HH_mm_ss .log”, teniendo la 
fecha en la que se generó el reporte. El contenido del reporte es:

![Log2](/practica_2/Imagenes/Log2.png)