# PROYECTO - MANEJO DE MEMORIA - SO2 - JUN2024

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

## Librerias Utilizadas

<table>
    <tr>
        <th>Biblioteca</th>
        <th>Descripción</th>
    </tr>
    <tr>
        <td>stdio.h</td>
        <td>Proporciona funcionalidades para realizar operaciones de entrada y salida, como leer y escribir en archivos.</td>
    </tr>
    <tr>
        <td>stdlib.h</td>
        <td>Incluye funciones para la gestión de memoria, control de procesos, conversiones y otras utilidades.</td>
    </tr>
    <tr>
        <td>string.h</td>
        <td>Contiene funciones para manipular y operar con cadenas de caracteres.</td>
    </tr>
    <tr>
        <td>unistd.h</td>
        <td>Define constantes, tipos y funciones para realizar operaciones del sistema operativo, como manipulación de archivos.</td>
    </tr>
    <tr>
        <td>time.h</td>
        <td>Ofrece funciones para manipular y medir el tiempo.</td>
    </tr>
    <tr>
        <td>sys/types.h</td>
        <td>Define tipos de datos usados en llamadas al sistema.</td>
    </tr>
    <tr>
        <td>sys/wait.h</td>
        <td>Proporciona macros relacionadas con la terminación del proceso.</td>
    </tr>
    <tr>
        <td>errno.h</td>
        <td>Define macros para reportar errores producidos durante la ejecución de llamadas al sistema o funciones de la librería estándar.</td>
    </tr>
    <tr>
        <td>mysql/mysql.h</td>
        <td>Incluye las funciones necesarias para interactuar con bases de datos MySQL desde C.</td>
    </tr>
</table>


### Descripción

_La gestión de la memoria es un aspecto crítico del rendimiento del sistema, ya que garantiza la utilización eficiente de los recursos y mantiene la estabilidad bajo diferentes cargas de trabajo. Al monitorear exhaustivamente la memoria, se puede obtener información valiosa sobre el comportamiento de su sistema, identificar posibles cuellos de botella y optimizar la asignación de recursos._

## Arquitectura

![Arq](/project_/Img/Arqui.png)

El script de SystemTap se encargará de capturar las solicitudes de memoria de todos los procesos activos. 
Estas solicitudes son realizadas por medio de estas llamadas al sistema:

```c
* void *mmap(void addr[.length], size_t length, int prot, int flags, int fd, off_t offset);
* int munmap(void addr[.length], size_t length);
```
  
mmap() crea una nueva asignación en el espacio de direcciones virtuales del proceso de llamada. La dirección inicial para la nueva asignación se especifica en addr. El argumento de length especifica la 
longitud del mapeo (que debe ser mayor que 0).

La llamada al sistema munmap() elimina las asignaciones para el rango de direcciones especificado.

La captura de estas llamadas se realizaran por medio de un script de SystemTap, que es una herramienta para la instrumentación dinámica para sistemas operativos basados en el Linux por el cual los administradores del sistema pueden extraer, filtrar y resumir datos para permitir el diagnóstico de problemas complejos de rendimiento o funcionales.

Cada vez que mmap() o munmap() sean llamadas por algún proceso el script deberá de escribir obtener los datos necesarios para poder realizar un análisis del uso de memoria (PID, longitud del segmento de memoria, fecha de la solicitud, etc.)

## Funcionamiento

1. Conexión a la base de datos

![connect_db](/project_/Img/Connect.png)

2. Generación de Script SystemTap

![Sys](/project_/Img/Sys.png)

3. Llamadas al sistema

![Call_sys](/project_/Img/call.png)

Con esto pudimos reunir los requisitos solicitados y los fuimos insertando en una base de datos sql donde estaremos almacenando la información para cuando estemos haciendo las llamadas desde nuestra api la cual se conforma con el backend realizado en Node y el frontend en React, ya que elegimos estas dos tecnologías por la fácilidad para poder consumir los endpoints, por lo cual nuestro dashboard se ve de la siguiente manera

![Dash](/project_/Img/Dash.jpeg)

## Conclusiones

* Se logró obtener la información necesaria para poder realizar un análisis del uso de memoria de los procesos activos en el sistema.
* Se logró almacenar la información en una base de datos MySQL.
* Se logró consumir la información almacenada en la base de datos por medio de una API REST.
* Se logró visualizar la información en un dashboard.