#include "header.h"

int main(void)
{
    int                 socket_fd;
    int                 len;
    struct sockaddr_un  remote;
    char                buf[MENSAJE_MAXIMO];

    /* Crear un socket de tipo UNIX con SOCK_STREAM */
    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("creando socket");
        exit(1);
    }

    /* Establecer la dirección a la cual conectarse. */
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);

    /* Conectarse. */
    if (connect(socket_fd, (struct sockaddr *)&remote, len) == -1) {
        perror("conectandose");
        exit(1);
    }

    /* Establecer la dirección a la cual conectarse para escuchar. */
    while(printf("> "), fgets(buf, MENSAJE_MAXIMO, stdin), !feof(stdin)) {
        if (send(socket_fd, buf, strlen(buf), 0) == -1) {
            perror("enviando");
            exit(1);
        }
    }

    /* Cerrar el socket. */
    close(socket_fd);

    return 0;
}
