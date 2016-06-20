#include "mt.h"

int main(int argc, char* argv[]) {
    int                 socket_fd;
    struct sockaddr_in  remote;
    char                buf[MAX_MSG_LENGTH];

    if(argc != 2) {
        printf("\n Usage: %s 127.0.0.1\n",argv[0]);
        exit(1);
    } 

    char* the_ip = argv[1];

    /* Crear un socket de tipo UNIX con SOCK_STREAM */
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("creando socket");
        exit(1);
    }


    /* Establecer la dirección a la cual conectarse. */
    remote.sin_family = AF_INET;
    remote.sin_port = htons(PORT);

    if(inet_aton(the_ip, &remote.sin_addr) < 0){
    	perror("inet aton error");
    	exit(1);
    }

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
        if(!strncmp(buf, END_STRING, MAX_MSG_LENGTH)) {
        	break;
        } else {
        	memset(buf, 0, MAX_MSG_LENGTH);
        	read(socket_fd, buf, MAX_MSG_LENGTH);
        	printf("Recibido: %s\n", buf);
        }
    }

    /* Cerrar el socket. */
    close(socket_fd);

    return 0;
}
