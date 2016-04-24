#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FILENAME    "shm-test"
#define LEN         4
#define MSG         "hola"

void perror_exit(char *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int fd;
    void *addr;
    pid_t child;

    child = fork();
    if (child == -1)
        perror_exit("fork");

    if (child == 0) {
        sleep(1);

        fd = shm_open(FILENAME, O_RDWR, 0);
        if (fd == -1)
            perror_exit("shm_open");

        addr = mmap(NULL, LEN, PROT_READ, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED)
            perror_exit("mmap child");

        if (close(fd) == -1)
            perror_exit("close");

        write(STDOUT_FILENO, addr, LEN);
        printf("\n");

        exit(EXIT_SUCCESS);

    } else {
        fd = shm_open(FILENAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd == -1)
            perror_exit("shm_open");

        if (ftruncate(fd, LEN))
            perror_exit("ftruncate");

        addr = mmap(NULL, LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED)
            perror_exit("mmap parent");

        if (close(fd) == -1)
            perror_exit("close");

        memcpy(addr, MSG, LEN);

        int status;
        while (1) {
            if (wait(&status) < 0)
                perror_exit("wait");
            if (WIFEXITED(status))
                break;
        }

        exit(EXIT_SUCCESS);
    }
}
