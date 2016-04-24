#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>  /* pid_t */
#include <sys/wait.h>   /* waitpid */
#include <unistd.h>     /* exit, fork */

int run(char *program_name[], char **program_argv[], unsigned int count) {
    /* TODO: Implemenar */

    return 0;
}

int main(int argc, char* argv[]) {
    /* Parsing de "ls -al | wc | awk '{ print $2 }'" */
    char *program_name[] = {
        "/bin/ls",
        "/usr/bin/wc",
        "/usr/bin/awk",
    };

    char *ls_argv[] = {"ls", "-al", NULL};
    char *wc_argv[] = {"wc", NULL};
    char *awk_argv[] = {"awk", "{ print $2 }", NULL};

    char **program_argv[] = {
        ls_argv,
        wc_argv,
        awk_argv,
    };

    unsigned int count = 3;

    int status = run(program_name, program_argv, count);

    printf("[+] Status : %d\n", status);

    return 0;
}
