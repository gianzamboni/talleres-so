#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>  /* pid_t */
#include <sys/wait.h>   /* waitpid */
#include <unistd.h>     /* exit, fork */

#define FORN(c,n) for(int c = 0; c <= n; c++ )
#define CHCKFORERROR(func, str) if(func < 0) {perror(str); exit(1);}

int run(char *program_name[], char **program_argv[], unsigned int count) {

	int pipes[count][2];
	int pid;

	FORN(i, count) {
		CHCKFORERROR(pipe(pipes[i]), "creando pipes")
	}

	FORN(i, count) {
		CHCKFORERROR((pid = fork()), "creando hijos")
		
		if(pid == 0) {
			FORN(j, count){
				// Solo voy a leer de mi pipe
				if(j != i) CHCKFORERROR(close(pipes[j][0]), "closing reading pipes")
				// SOlo voy a escribir en el pipe del siguiente proceso
				if(j != i + 1) CHCKFORERROR(close(pipes[j][1]), "closin writing pipes")
			}

			if(i == 0){
				// Soy el primer hijo, no leo nisiquiera de mi pipe
				CHCKFORERROR(close(pipes[i][0]), "closing my read-end pipe")
				dup2(pipes[i+1][1], 1); //redirigo mi stdout
			} else if(i == count - 1) {
				// Soy el último hijo, no mando mis resultados a nadie
				dup2(pipes[i][0], 0); //redirigo mi stdin
			} else {
				dup2(pipes[i][0], 0); // redirigo mi stdin
				dup2(pipes[i+1][1], 1); //redirigo mi stdout
			}

			execvp(program_name[i], program_argv[i]);
		}
	}

	// Soy el padre, cierro todos los pipes, yo no los voy a usar
	if(pid != 0) {
		FORN(i, count){
			CHCKFORERROR(close(pipes[i][0]), "closing pipes")
			CHCKFORERROR(close(pipes[i][1]), "closing pipes")
		}
	}

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
