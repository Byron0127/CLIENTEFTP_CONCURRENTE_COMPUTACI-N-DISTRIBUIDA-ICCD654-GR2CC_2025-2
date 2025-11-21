/* * archivo: OrtizBy-clienteFTP.c
 * estudiante: Ortiz Byron
 * descripcion: cliente ftp concurrente (version final corregida
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <errno.h>

#define BUFSIZE 1024

// funciones externas
int connectTCP(const char *host, const char *service);
int errexit(const char *format, ...);

int sock_control;      
char buffer[BUFSIZE];  

// lee del socket y devuelve el codigo (como pro ejemplo el 220)
int leer_respuesta() {
    memset(buffer, 0, BUFSIZE);
    int n = recv(sock_control, buffer, BUFSIZE - 1, 0);
    if (n > 0) {
        printf("%s", buffer);
        return atoi(buffer); 
    } else if (n == 0) {
        exit(1); // servidor cerro
    }
    return 0;
}

// VERSION BLINDADA DE MODO PASIVO
int conectar_modo_pasivo() {
    char cmd[] = "PASV\r\n";
    send(sock_control, cmd, strlen(cmd), 0);

    memset(buffer, 0, BUFSIZE);
    recv(sock_control, buffer, BUFSIZE - 1, 0);
    
    
    // Si recibimos un 226 (fin de transferencia anterior) o 15 lo ignoramos
    // y leemos de nuevo buscando la respuesta al PASV que empieza con 227
    if (atoi(buffer) == 226 || atoi(buffer) == 426 || atoi(buffer) == 225) {
        printf("[sync] ignorando mensaje anterior del servidor: %s", buffer);
        memset(buffer, 0, BUFSIZE);
        recv(sock_control, buffer, BUFSIZE - 1, 0);
    }
    
    printf("%s", buffer); // Imprimimos la respuesta real deberia ser 227

    char *p = strchr(buffer, '(');
    if (p == NULL) return -1;
    p++;

    int h1, h2, h3, h4, p1, p2;
    sscanf(p, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2);

    char ip_remota[20];
    sprintf(ip_remota, "%d.%d.%d.%d", h1, h2, h3, h4);
    
    int puerto_num = (p1 * 256) + p2; // la logica esta aqui para parsear en el modo pass
    char puerto_str[10];
    sprintf(puerto_str, "%d", puerto_num);

    printf("conectando datos pasv a %s:%s...\n", ip_remota, puerto_str);
    return connectTCP(ip_remota, puerto_str);
}

// VERSION BLINDADA DEl MODO ACTIVO
int preparar_modo_activo(int *sock_escucha) {
    struct sockaddr_in sin, mi_addr;
    socklen_t len = sizeof(mi_addr);
    int s;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return -1;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = 0;

    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("error bind");
        return -1;
    }

    if (getsockname(sock_control, (struct sockaddr *)&mi_addr, &len) < 0) return -1;
    struct sockaddr_in temp_addr;
    len = sizeof(temp_addr);
    if (getsockname(s, (struct sockaddr *)&temp_addr, &len) < 0) return -1;

    int puerto_local = ntohs(temp_addr.sin_port);
    unsigned char *ip = (unsigned char *)&mi_addr.sin_addr.s_addr;
    
    listen(s, 1);
    *sock_escucha = s;

    char cmd_port[128];
    sprintf(cmd_port, "PORT %d,%d,%d,%d,%d,%d\r\n",
            ip[0], ip[1], ip[2], ip[3],
            puerto_local / 256, puerto_local % 256);
    
    printf("enviando: %s", cmd_port);
    send(sock_control, cmd_port, strlen(cmd_port), 0);
    
    // para evitar desfases o en todocaso saltar con el enter 
    memset(buffer, 0, BUFSIZE);
    recv(sock_control, buffer, BUFSIZE - 1, 0);
    printf("%s", buffer);

    // Si leemos un 226 viejo, leemos OTRA VEZ buscando el 200 del PORT para evitar los errores de salida de comando o sincorinzacion
    if (atoi(buffer) == 226 || atoi(buffer) == 426) {
        printf("[sync] ignorando mensaje 226 viejo...\n");
        memset(buffer, 0, BUFSIZE);
        recv(sock_control, buffer, BUFSIZE - 1, 0);
        printf("%s", buffer);
    }

    if (atoi(buffer) != 200) {
        close(s);
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) errexit("uso: %s <ip>\n", argv[0]);

    char *host = argv[1];
    char entrada[BUFSIZE];
    char comando[32], argumento[256];
    int modo_activo = 0;

    sock_control = connectTCP(host, "21");
    leer_respuesta(); 
    printf("conectado a %s\n", host);

    while (1) {
        printf("\nmi_ftp> ");
        fflush(stdout);
        memset(entrada, 0, BUFSIZE);
        if (fgets(entrada, BUFSIZE, stdin) == NULL) break;

        memset(comando, 0, 32);
        memset(argumento, 0, 256);
        sscanf(entrada, "%s %s", comando, argumento);

        if (strcmp(comando, "modo") == 0) {
            if (strcmp(argumento, "port") == 0) {
                modo_activo = 1;
                printf("cambiado a modo ACTIVO (PORT)\n");
            } else {
                modo_activo = 0;
                printf("cambiado a modo PASIVO (PASV)\n");
            }
            continue;
        }

        if (strcmp(comando, "quit") == 0) {
            send(sock_control, "QUIT\r\n", 6, 0);
            leer_respuesta();
            close(sock_control);
            break;
        }
        else if (strcmp(comando, "user") == 0) {
            sprintf(buffer, "USER %s\r\n", argumento);
            send(sock_control, buffer, strlen(buffer), 0);
            leer_respuesta();
        }
        else if (strcmp(comando, "pass") == 0) {
            sprintf(buffer, "PASS %s\r\n", argumento);
            send(sock_control, buffer, strlen(buffer), 0);
            leer_respuesta();
        }
        else if (strcmp(comando, "get") == 0 || strcmp(comando, "put") == 0) {
            int data_sock = -1;
            int server_sock_activo = -1; 

            if (modo_activo) {
                if (preparar_modo_activo(&server_sock_activo) < 0) {
                    printf("fallo configurando modo PORT\n");
                    continue;
                }
            } else {
                data_sock = conectar_modo_pasivo();
                if (data_sock < 0) {
                    printf("fallo en modo PASV\n");
                    continue;
                }
            }

            if (strcmp(comando, "get") == 0) sprintf(buffer, "RETR %s\r\n", argumento);
            else sprintf(buffer, "STOR %s\r\n", argumento);
            
            send(sock_control, buffer, strlen(buffer), 0);
            
            // Leemos respuesta de inicio de transferencia (150)
            memset(buffer, 0, BUFSIZE);
            recv(sock_control, buffer, BUFSIZE - 1, 0);
            printf("%s", buffer);
            int code = atoi(buffer);

            // Si  leimos el 226 anterior aqui, leemos de nuevo
            if (code == 226) {
                 printf("[sync] mensaje 226 ignorado...\n");
                 memset(buffer, 0, BUFSIZE);
                 recv(sock_control, buffer, BUFSIZE - 1, 0);
                 printf("%s", buffer);
                 code = atoi(buffer);
            }

            if (modo_activo && (code == 150 || code == 125)) {
                data_sock = accept(server_sock_activo, NULL, NULL);
                close(server_sock_activo); 
                if (data_sock < 0) { perror("error accept"); continue; }
            }

            if (code != 150 && code != 125) {
                if (data_sock >= 0) close(data_sock);
                continue;
            }

            int pid = fork();
            if (pid == 0) { // HIJO
                if (strcmp(comando, "get") == 0) {
                    FILE *f = fopen(argumento, "wb");
                    if (!f) exit(1);
                    char dbuf[BUFSIZE];
                    int n;
                    while ((n = recv(data_sock, dbuf, BUFSIZE, 0)) > 0) fwrite(dbuf, 1, n, f);
                    fclose(f);
                } else {
                    FILE *f = fopen(argumento, "rb");
                    if (!f) { printf("no existe archivo local\n"); exit(1); }
                    char dbuf[BUFSIZE];
                    int n;
                    while ((n = fread(dbuf, 1, BUFSIZE, f)) > 0) send(data_sock, dbuf, n, 0);
                    fclose(f);
                }
                close(data_sock);
                printf("\n[hijo %d] termine tarea '%s'\nmi_ftp> ", getpid(), argumento);
                exit(0);
            } else { // PADRE
                close(data_sock);
                printf("[padre] transferencia en background (pid: %d)\n", pid);
            }
        }
        else {
            if (strcmp(comando, "cd") == 0) strcpy(comando, "CWD");
            sprintf(buffer, "%s %s\r\n", comando, argumento);
            send(sock_control, buffer, strlen(buffer), 0);
            leer_respuesta();
        }
        while(waitpid(-1, NULL, WNOHANG) > 0); 
    }
    return 0;
}