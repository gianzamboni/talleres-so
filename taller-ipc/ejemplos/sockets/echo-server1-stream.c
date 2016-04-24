#include "header.h"

int main(void)
{
    int s, s1, t, len;
    struct sockaddr_un local, remote;
    char str[MENSAJE_MAXIMO];
    int n;

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

	/* Aceptar una conexión entrante. */
    t = sizeof(remote);
    if ((s1 = accept(s, (struct sockaddr*) &remote, (socklen_t*) &t)) == -1) {
        perror("aceptando la conexión entrante");
        exit(1);
    }

	/* Mostrar en pantalla todo lo que recibimos. */
    for(;;) {
        n = recv(s1, str, MENSAJE_MAXIMO, 0);
        if (n == 0) 
            break;
        if (n < 0) { 
        	perror("recibiendo");
        	exit(1);
        }
        str[n] = '\0'; /* Agregar caracter de fin de cadena a lo recibido. */
        printf("%s", str);
    }

	/* Cerrar la conexión entrante. */
    close(s1);

	/* Cerrar la conexión que escucha. */
	close(s);
	
    return 0;
}

