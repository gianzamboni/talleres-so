#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT            5555

#define MAX_MSG_LENGTH  1024
#define END_STRING      "chau\n"

/* Conectarse con : 'nc -u 127.0.0.1 5555' */

int main(int argc, char* argv[]) {
	int 				socket_fd;
	struct sockaddr_in 	name;
	char 				buf[MAX_MSG_LENGTH];

	/* Crear socket sobre el que se lee: dominio INET, protocolo UDP (DGRAM). */
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd < 0) {
		perror("Creating socket.");
		exit(EXIT_FAILURE);
	}

	/* Crear nombre, usamos INADDR_ANY para indicar que cualquiera puede enviar aquí. */
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;
	name.sin_port = htons(PORT);

	if (bind(socket_fd, (void*) &name, sizeof(name)) < 0) {
		perror("Binding datagram socket.");
		exit(EXIT_FAILURE);
	}

	/* Recibimos mensajes hasta que alguno sea el que marca el final. */
	for (;;) {
		read(socket_fd, buf, MAX_MSG_LENGTH);

		if (strncmp(buf, END_STRING, MAX_MSG_LENGTH) == 0)
			break;

		printf("> %s", buf);
	}

	/* Cerrar socket de recepción. */
	close(socket_fd);

	return 0;
}
