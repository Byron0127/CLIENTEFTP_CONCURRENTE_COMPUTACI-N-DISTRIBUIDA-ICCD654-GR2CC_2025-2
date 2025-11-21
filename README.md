Cliente FTP Concurrente

Estudiante: OrtizBy

Materia: Computación Distribuida

Descripción

Este proyecto consiste en un cliente FTP desarrollado en lenguaje C que cumple con el estándar RFC 959. El programa permite conectarse a servidores FTP, autenticarse y realizar transferencias de archivos de manera concurrente (usando procesos hijos), lo que permite al usuario seguir interactuando con la terminal mientras se realizan subidas o bajadas de archivos.

Características Principales

Conexión: Uso de sockets TCP (Puerto 21 para control).

Modos de Transferencia: Soporta tanto Modo Pasivo (PASV, por defecto) como Modo Activo (PORT).

Concurrencia: Implementación de fork() para manejar transferencias de datos en segundo plano sin bloquear la interfaz.

Comandos Soportados:

user, pass: Autenticación.

get (RETR): Descarga de archivos.

put (STOR): Subida de archivos.

modo port / modo pasv: Alternar entre modos de transferencia.

mkd, dele, pwd: Gestión de directorios y archivos (Extra crédito).

cd: Cambio de directorio (alias para CWD).

quit: Salir.

Archivos Incluidos

OrtizBy-clienteFTP.c: Código fuente principal del cliente.

connectsock.c, connectTCP.c, errexit.c: Librerías auxiliares para la gestión de conexiones de red.

Makefile: Script para compilación automática.

Compilación y Ejecución

Requisitos

Sistema Operativo Linux (o WSL).

Compilador gcc.

make.

Pasos

Compilar:
Ejecuta el siguiente comando en la terminal:

make


Ejecutar:
Inicia el cliente indicando la IP del servidor FTP:

./OrtizBy-clienteFTP <IP_DEL_SERVIDOR>


Ejemplo: ./OrtizBy-clienteFTP 127.0.0.1

Limpiar:
Para borrar los archivos objeto y el ejecutable:

make clean


Notas de Uso

Por defecto, el cliente inicia en Modo Pasivo. Si necesitas cambiar a Modo Activo, escribe modo port dentro del cliente.

Al descargar o subir archivos grandes, el prompt mi_ftp> aparecerá inmediatamente gracias a la concurrencia. El proceso hijo notificará cuando la transferencia termine.
