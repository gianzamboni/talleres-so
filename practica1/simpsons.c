#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	int homerStatus;
	int bartStatus;
	int maggieStatus;
	int lisaStatus;

	pid_t homer;
	pid_t bart;
	pid_t lisa;
	pid_t maggie;

	/* Fork en dos procesos */
	homer = fork();


	if (homer == -1) { perror("ERROR fork"); return 1; }
	if (homer == 0) {
		
		printf("Homer\n");
		
		bart = fork();
		
		if(bart == 0 ) {
			
			printf("Bart\n");
		
		} else {
			
			lisa = fork();
			if(lisa == 0){
			
				printf("Lisa\n");
			
			} else {
				
				maggie = fork();
				
				if(maggie == 0){
				
					printf("Maggie\n");
				
				}
			}

			int waitpidB = 1;
			int waitpidL = 1;
			int waitpidM = 1;

			while(1) {

				if(waitpidB){
					waitpidB = waitpid(bart, &bartStatus,0);
				}

				if(waitpidL){
					waitpidL = waitpid(lisa, &bartStatus,0);
				}

				if(waitpidM){
					waitpidM = waitpid(maggie, &maggieStatus,0); 
				}

				if(waitpidB < 0 && waitpidM < 0 && waitpidB < 0){
					perror("waitpid");
					break;	
				}
				if (WIFEXITED(bartStatus) && WEXITSTATUS(bartStatus) == 0 && WIFEXITED(bartStatus) && WEXITSTATUS(bartStatus) == 0 && WIFEXITED(bartStatus) && WEXITSTATUS(bartStatus) == 0) {

					break;
				}

			}

		}



	} else {
		/* S'olo se ejecuta en el Padre */
		printf("Abraham\n");

		while(1) {
			if (waitpid(homer, &homerStatus,0) < 0) { perror("waitpid"); break; }
			if (WIFEXITED(homerStatus) && WEXITSTATUS(homerStatus) == 0) break; /* Proceso terminado */
		}

	}
	return 0;
}
