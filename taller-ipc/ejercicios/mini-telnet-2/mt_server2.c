#include "mt.h"

int main(int argc, char* argv[]) {
    int                 sock, remote_sock, remote_sock_size;
    struct sockaddr_in  local, remote;
    char                buf[MAX_MSG_LENGTH];

    /* Crear socket sobre el que se lee: dominio INET, protocolo UDP (DGRAM). */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("abriendo socket");
        exit(1);
    }

    /* Crear nombre, usamos INADDR_ANY para indicar que cualquiera puede enviar aquí. */
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(PORT);

    if (bind(sock, (void*) &local, sizeof(local))) {
        perror("binding stream socket");
        exit(1);
    }

	/* Escuchar en el socket y permitir 5 conexiones en espera. */
    if(listen(sock, 5) == -1){
    	perror("escuchando");
    	exit(1);
    }


	/* Aceptar una conexión entrante. */
    remote_sock_size = sizeof(remote);
    if ((remote_sock = accept(sock, (struct sockaddr*) &remote, (socklen_t*) &remote_sock_size)) == -1) {
        perror("aceptando la conexión entrante");
        exit(1);
    }

    dup2(remote_sock, 1); //Redirigiendo stdout
    dup2(remote_sock, 2); //Redirigiendo stderr

    /* Recibimos mensajes hasta que alguno sea el que marca el final. */
    for (;;) {
        memset(buf, 0, MAX_MSG_LENGTH);
        read(remote_sock, buf, MAX_MSG_LENGTH);
        if (strncmp(buf, END_STRING, MAX_MSG_LENGTH) == 0)
            break;
        printf("Comando: %s", buf);
        system(buf);
    }

    /* Cerrar socket de recepción. */
    close(remote_sock);
    close(sock);

    return 0;
}


