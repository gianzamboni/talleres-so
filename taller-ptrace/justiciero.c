#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	int status;
	pid_t child;

	if (argc <= 1) {
		fprintf(stderr, "Uso: %s commando [argumentos ...]\n", argv[0]);
		exit(1);
	}

	/* Fork en dos procesos */
	child = fork();
	if (child == -1) { perror("ERROR fork"); return 1; }
	if (child == 0) {
		/* S'olo se ejecuta en el Hijo */
		int trace = ptrace(PTRACE_TRACEME, 0, 0, 0);
		execvp(argv[1], argv+1);
	
	} else {
		/* S'olo se ejecuta en el Padre */
		while(1) {
			if (waitpid(child, &status, 0) < 0) { perror("waitpid"); break; }
			if (WIFEXITED(status)) break; /* Proceso terminado */			
			int sys_no = ptrace(PTRACE_PEEKUSER, child, 8*ORIG_RAX, 0);
			if(sys_no == SYS_kill) {
				printf("Se ha hecho justicia\n");
				ptrace(PTRACE_KILL, child,0,0);
				break;
			} else{
				int syscall_enter = ptrace(PTRACE_SYSCALL, child, 0, 0);
			}	
			

		}

		ptrace(PTRACE_DETACH, child, 0,0);
	}
	return 0;
}
