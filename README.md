# Cliente FTP Concurrente

**Estudiante:** OrtizByron
**Materia:** Computación Distribuida

## 📖 Descripción
Este proyecto consiste en un cliente FTP desarrollado en lenguaje **C** que cumple con el estándar **RFC 959**. El programa permite conectarse a servidores FTP, autenticarse y realizar transferencias de archivos de manera **concurrente** (utilizando procesos hijos). Esto permite al usuario seguir interactuando con la terminal y enviando comandos mientras se realizan subidas o bajadas de archivos en segundo plano.

## 🚀 Características Principales

* **Conexión:** Uso de sockets TCP (Puerto 21 para control).
* **Modos de Transferencia:** Soporta tanto **Modo Pasivo** (PASV, por defecto) como **Modo Activo** (PORT).
* **Concurrencia:** Implementación de `fork()` para manejar transferencias de datos sin bloquear la interfaz de usuario.
* **Comandos Soportados:**
    * `user`, `pass`: Autenticación de usuario.
    * `get` (RETR): Descarga de archivos.
    * `put` (STOR): Subida de archivos.
    * `modo port` / `modo pasv`: Alternar entre modos de transferencia.
    * `mkd`, `dele`, `pwd`: Gestión de directorios y archivos (Extra crédito).
    * `cd`: Cambio de directorio (alias para CWD).
    * `quit`: Desconectar y salir.

## 📂 Archivos Incluidos

| Archivo | Descripción |
| :--- | :--- |
| `OrtizBy-clienteFTP.c` | Código fuente principal del cliente. |
| `connectsock.c` | Librería auxiliar para creación de sockets. |
| `connectTCP.c` | Librería auxiliar para conexión TCP. |
| `errexit.c` | Librería para manejo de errores. |
| `Makefile` | Script para compilación automática. |

## ⚙️ Compilación y Ejecución

### Requisitos
* Sistema Operativo Linux (o WSL en Windows).
* Compilador `gcc`.
* Herramienta `make`.

### Pasos

1.  **Compilar:**
    Ejecuta el siguiente comando en la terminal para generar el ejecutable:
    ```bash
    make
    ```

2.  **Ejecutar:**
    Inicia el cliente indicando la dirección IP del servidor FTP al que deseas conectarte:
    ```bash
    ./OrtizBy-clienteFTP <IP_DEL_SERVIDOR>
    ```
    *Ejemplo:*
    ```bash
    ./OrtizBy-clienteFTP 127.0.0.1
    ```

3.  **Limpiar:**
    Para borrar los archivos objeto (`.o`) y el ejecutable generado:
    ```bash
    make clean
    ```

## ⚠️ Notas de Uso

> **Modos de Transferencia:** Por defecto, el cliente inicia en **Modo Pasivo**. Si necesitas cambiar a Modo Activo (necesario en algunas configuraciones de red), escribe `modo port` dentro de la consola del cliente.

> **Concurrencia:** Al descargar o subir archivos grandes, el prompt `mi_ftp>` aparecerá inmediatamente para que puedas seguir escribiendo comandos. El proceso hijo notificará en pantalla automáticamente cuando la transferencia haya terminado.
