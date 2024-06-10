## MANUAL TECNICO -- 1ra Práctica Llamadas al Sistema -- SISTEMAS OPERATIVOS 2

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

## Librerias utilizadas en la practica

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
```

### Explicacion de las libreria utilizadas

<ol>
  <li><p align="justify"><b>stdio.h</b>: Esta librería proporciona funciones para entrada y salida estándar, como printf() y scanf(), así como manipulación de archivos con funciones como fopen() y fclose().</p></li>
  <li><p align="justify"><b>unistd.h</b>:Proporciona acceso a numerosas funciones del sistema operativo POSIX relacionadas con la gestión de procesos, directorios y archivos, así como para la gestión de recursos del sistema, como fork() para crear procesos y exec() para ejecutar programas.</p></li>
  <li><p align="justify"><b>stdlib.h</b>:Contiene funciones para la gestión de memoria dinámica, conversión de tipos de datos y otras utilidades generales, como malloc() y free() para la gestión de la memoria dinámica y atoi() para convertir cadenas a enteros.</p></li>
  <li><p align="justify"><b>string.h</b>:Ofrece funciones para manipular cadenas de caracteres, como strlen() para obtener la longitud de una cadena y strcpy() para copiar cadenas.</p></li>
  <li><p align="justify"><b>sys/wait.h</b>:Contiene funciones y macros relacionadas con la espera y el control de procesos, como wait() y waitpid() para esperar a que un proceso hijo termine.</p></li>
  <li><p align="justify"><b>signal.h</b>:Permite la manipulación de señales del sistema, como SIGINT para la señal de interrupción (Ctrl+C) y SIGTERM para la señal de terminación.</p></li>
  <li><p align="justify"><b>errno.h</b>:Esta librería define la variable errno, que se utiliza para informar sobre errores durante las llamadas a funciones del sistema y proporciona macros para identificar los diferentes códigos de error.</p></li>
</ol>


## Descripción sobre la práctica

La práctica consiste en la implementación de un programa en C que crea dos procesos hijos y un proceso de monitorización de llamadas al sistema. Los procesos hijos ejecutan un programa que realiza llamadas al sistema read(), write() y lseek() sobre un archivo, y el proceso de monitorización captura y registra estas llamadas en un archivo de log. El programa principal espera hasta recibir una señal SIGINT (Ctrl+C) para finalizar, momento en el que procesa las estadísticas de las llamadas al sistema y termina los procesos relacionados.


## Proceceos Especificos

### Parent

Esta parte sera el proceso principal para el sistema, el cual iniciara con la función fork() por lo cual podremos crear dos procesos hijos los cuales seran monitoreados al hacer las llamadas al sistema con eso se debe de crear un archivo llamado _syscall.log_ el cual se deberá de vacia cada vez que el programa se ejecute o se crea al iniciar la ejecucicón

* Para cada llamada que se intercepte, en el archivo de texto se deberá de escribir: 
`Proceso <<pid>> : <<llamada realizada>> (<<fecha y hora de la llamada>>)`

Este proceso se terminara al mandar una señal `SIGINT (CTRL+C)` desde la terminal, por lo cual el proceso tendra que tener está señal e imprimir una tabla con los datos solicitados 

### Child

Este procesos hijo tendra un ciclo donde realizara llamadas al sistema y para poder llevar este control se crea un archivo de texto llamado `Practica1.txt` el cual se vacía al iniciar nuevamente el programa o se crea al inicio

Una vez en ejecución el proceso llamara al sistema aleatoreamente una de estas llamadas.

* `Open()`: Llamada cuando se abra el archivo
* `write()`: Escribira en un archivo de texto al azar con 8 caracteres alfanuméricos 
* `read()`: Leera los 8 caracteres

Estas se realizaron en un lapso de 1 a 3 segundos, con esto tendriamos la simulación de un subproceso que realiza las llamdas al sistema.

## SystemTap

Es una herramienta que nos ayuda para la recolección de información del Kernel para poder monitorear el sistema operativo

Para poder instalar SystemTap se debe de ejecutar el siguiente comando en la terminal

```bash
sudo apt-get install systemtap
```

Para poder ejecutar el archivo de SystemTap se debe de ejecutar el siguiente comando en la terminal

```bash
sudo stap -g monitor_syscalls.stp -x <<pid1>> -x <<pid2>>
```
Esta herramienta la utilizaremos para poder llamar el script desde el proceso padre así con eso podre mostrar los datos solicitados.