##Ejercicio 7 

int main(){
	share_mem(&tiki);
	share_mem(&taka);

	pid_t taka_child
	pid_t tiki_child = fork();


	if(tiki_child == 0){
		tiki_taka();
	} else {
		taka_child == fork();

		if(taka_child == 0){
			taka_runner();
		} 
	}

	return 0;
}

##Ejercicio 8.a

int main(){
	int counter = 0;

	pid_t parent_id = get_current_pid();

	pid_t child = fork();
	

	if(child == 0){

		while(1){
			counter = breceive(parent_id);
			counter++;
			bsend(parent_id, counter);
		}

	} else {

		while(1){
			bsend(child, counter);
			counter = breceive(child);
			counter++;
		}
	}
	
	return 0;
}

##Ejercicio 8.b

int main(){

	int counter = 0;

	pid_t parent_id = get_current_pid();

	pid_t child_1 = fork();

	if(child_1){
			pid_t brother = breceive(parent_id);
		while(1){
			counter = breceive(parent_id);
			counter++;
			bsend(brother, counter);
		}
	} else {

		pid_t child_2 = fork()

		if(child_2 == 0) {
				brother = breceive(parent_id);
			while(1){
				counter = breceive(brother);
				counter++;
				bsend(parent_id, counter);
			}
		} else {

			bsend(child_1, child_2);
			bsend(child_2, child_1);

			while(1){

				bsend(child_1, counter);
				counter = breceive(child_2);
				counter++;
			}

		}
	}

	return 0;
}

##Ejercicio 8.c
int main(){

	int counter = 0;

	pid_t parent_id = get_current_pid();

	pid_t child_2;
	pid_t child_1 = fork();

	if(child_1){
		while(1){
			counter = breceive(parent_id);
			counter++;
			bsend(chidl_2, counter);
		}
	} else {

		pid_t child_2 = fork()

		if(child_2 == 0) {
			while(1){
				counter = breceive(child_1);
				counter++;
				bsend(parent_id, counter);
			}
		} else {

			share_mem(child_2);
			share_mem(child_1);

			while(1){

				bsend(child_1, counter);
				counter = breceive(child_2);
				counter++;
			}

		}
	}

	return 0;
}


##Ejercicio 15

int main(int argc, char* argv[]){

	int msg;
	int suma1 = 0;
	int suma2 = 1;
	sh_mem(&msg);

	if( argv[1]==0){
		printf("%d\n", suma1);
	} else if(argv[1]==1){
		printf("%d\n", suma2);
	} else {
		pid_t child = fork();

		if(child == 0){
			while(!msg){
				if(argv[1]==suma1){
					exit(1);
				} else {
					int temp= suma1;
					suma1 = suma1+suma2;
					suma2 =temp;
					msg =1;
				}
			}
		} else {
			while(1){
				if (wait(&status) < 0) { perror("waitpid"); break; }
				if (WIFEXITED(status) && argv[1]== suma1) {
					printf("%d\n", suma1);
					break;
				} /* Proceso terminado */

			
				if(argv[1]== suma1){
					printf("%d\n", suma1);
				} else if(msg) {
					int temp= suma1;
					suma1 = suma1+suma2;
					suma2 =temp;
					msg =1;
				}
			}
		}	
	}

	return 0;
}