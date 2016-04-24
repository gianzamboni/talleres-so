#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define DATOS "sistemas es lo más"

void perror_exit(char *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main() {
    int     pipes[2];
    char    buf[1024];

    /* Crear un pipe. */
    if (pipe(pipes) < 0)
        perror_exit("pipe");

    /* Escribimos los datos en el pipe. */
    if (write(pipes[1], DATOS, sizeof(DATOS)) < 0)
        perror("escribiendo datos");

    /* Leemos los datos del pipe. */
    if (read(pipes[0], buf, 1024) < 0)
        perror_exit("leyendo datos");

    /* Escribimos en pantalla la cantidad de bytes leídos. */
    printf("[+] Caracteres de la frase \"%s\": %d\n", buf, (int) strlen(buf));

    /* Cerramos el pipe de lectura. */
    close(pipes[0]);

    /* Cerramos el pipe de escritura. */
    close(pipes[1]);

    return 0;
}
