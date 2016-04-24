#include "mt.h"

int main(int argc, char* argv[]) {
    int                 sock;
    struct sockaddr_in  name;
    char                buf[MAX_MSG_LENGTH];

    /* Crear socket sobre el que se lee: dominio INET, protocolo UDP (DGRAM). */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("abriendo socket");
        exit(1);
    }

    /* Crear nombre, usamos INADDR_ANY para indicar que cualquiera puede enviar aquí. */
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = INADDR_ANY;
    name.sin_port = htons(PORT);

    if (bind(sock, (void*) &name, sizeof(name))) {
        perror("binding datagram socket");
        exit(1);
    }

    /* Recibimos mensajes hasta que alguno sea el que marca el final. */
    for (;;) {
        memset(buf, 0, MAX_MSG_LENGTH);
        read(sock, buf, MAX_MSG_LENGTH);
        if (strncmp(buf, END_STRING, MAX_MSG_LENGTH) == 0)
            break;
        printf("Comando: %s", buf);
        system(buf);
    }

    /* Cerrar socket de recepción. */
    close(sock);

    return 0;
}
