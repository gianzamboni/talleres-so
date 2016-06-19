#include "mt.h"

int main(int argc, char* argv[]) {
    int                 socket_fd;
    struct sockaddr_in  remote;
    char                buf[MAX_MSG_LENGTH];

    /* Crear un socket de tipo UNIX con SOCK_STREAM */
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("creando socket");
        exit(1);
    }

    /* Establecer la dirección a la cual conectarse. */
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = INADDR_ANY;
    remote.sin_port = htons(PORT);

    /* Conectarse. */
    if (connect(socket_fd, (struct sockaddr *)&remote, sizeof(remote)) == -1) {
        perror("conectandose");
        exit(1);
    }

    /* Establecer la dirección a la cual conectarse para escuchar. */
    while(printf("> "), fgets(buf, MAX_MSG_LENGTH, stdin), !feof(stdin)) {
        if (send(socket_fd, buf, strlen(buf), 0) == -1) {
            perror("enviando");
            exit(1);
        }

        if(!strcmp(buf, END_STRING)) break;
    }

    /* Cerrar el socket. */
    close(socket_fd);

    return 0;

    return 0;
}
