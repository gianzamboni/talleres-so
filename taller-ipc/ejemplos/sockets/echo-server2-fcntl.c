#include "header.h"

int leer_de_socket(int s);
int no_bloqueante(int fd);

int main(void)
{
    int s, s1, s2, t, len;
    struct sockaddr_un local, remote;

    /* Crear un socket de tipo UNIX con TCP (SOCK_STREAM). */
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("creando socket");
        exit(1);
    }

    /* Establecer la dirección a la cual conectarse para escuchar. */
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(s, (struct sockaddr *)&local, len) == -1) {
        perror("haciendo bind");
        exit(1);
    }

    /* Escuchar en el socket y permitir 5 conexiones en espera. */
    if (listen(s, 5) == -1) {
        perror("escuchando");
        exit(1);
    }

    /* Aceptar dos conexiónes entrantes. */
    t = sizeof(remote);
    if ((s1 = accept(s, (struct sockaddr*) &remote, (socklen_t*) &t)) == -1) {
        perror("aceptando la primer conexión entrante");
        exit(1);
    }
    if ((s2 = accept(s, (struct sockaddr*) &remote, (socklen_t*) &t)) == -1) {
        perror("aceptando la segunda conexión entrante");
        exit(1);
    }

    /* Setear sockets como no bloqueantes */
    no_bloqueante(s1);
    no_bloqueante(s2);


    /* Mostrar en pantalla todo lo que recibimos. */
    for(;;) {
        if (leer_de_socket(s1) == -1)
            break;
        if (leer_de_socket(s2) == -1)
            break;
    }

    /* Cerrar las conexiones entrantes. */
    close(s1);
    close(s2);

    /* Cerrar la conexión que escucha. */
    close(s);

    return 0;
}

int leer_de_socket(int s) {
    int n;
    char str[MENSAJE_MAXIMO];
    n = recv(s, str, MENSAJE_MAXIMO, 0);

    if (n == 0) {
        return -1;
    }

    /* Me fijo si no hay datos o se produjo un error */
    if (n < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return 0;
        }else{
            /* El error no se produjo por falta de datos */
            perror("recibiendo");
            exit(1);
        }
    }
    str[n] = '\0'; /* Agregar caracter de fin de cadena a lo recibido. */
    printf("%s", str);
    return 0;
}

int no_bloqueante(int fd) {
    int flags;
    /* Toma los flags del fd y agrega O_NONBLOCK */
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1 )
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

