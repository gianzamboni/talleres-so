#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
using namespace std;

int counter = 0;
int status;

pid_t child;

void increaseCounter(int signum){
	counter++;
	fprintf(stdout, "ya va!\n");
	
}

void waitChild(int signum){
		while(1) {
			if (wait4(-1, &status,0,0) < 0) { perror("waitpid"); break; }
			if (WIFEXITED(status) && WEXITSTATUS(status) == 0) break; /* Proceso terminado */
		}
}

int main(int argc, char* argv[]) {

	if (argc <= 1) {
		fprintf(stderr, "Uso: %s commando [argumentos ...]\n", argv[0]);
		exit(1);
	}

	/* Fork en dos procesos */
	child = fork();
	
	if (child == -1) { perror("ERROR fork"); return 1; }

	if (child == 0) { //Comportamiento del hijo
		
		signal(SIGURG, increaseCounter);

		while(counter < 5){

		}

		kill(getppid(), SIGINT);
		execvp(argv[1], argv+1);
		/* Si vuelve de exec() hubo un error */

	} else { // Comportamiento del padre
		/* S'olo se ejecuta en el Padre */

		signal(SIGINT, waitChild);

		sigset_t emptySet;
		sigemptyset(&emptySet);

		sigset_t toBlock;
		sigemptyset(&toBlock);
		sigaddset(&toBlock, SIGCHLD);

		timespec tiempo;
		tiempo.tv_sec = 1;
		tiempo.tv_nsec = 0;

		timespec resto;

		while(counter < 5){
			sigprocmask(SIG_BLOCK, &toBlock, 0);
			signal(SIGCHLD, 0);
			sigprocmask(SIG_SETMASK, &emptySet, 0);
			nanosleep(&tiempo, &resto);
			fprintf(stdout, "sup!\n");
			kill(child, SIGURG);
			counter ++;
		}
	}
	
	return 0;
}