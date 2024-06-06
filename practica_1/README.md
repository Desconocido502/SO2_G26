## Manual técnico

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

### Parent


### Child