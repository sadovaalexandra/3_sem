#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
	char* command = malloc(sysconf(_SC_ARG_MAX)); // memo 4 a command of max possible length
	while(1) {
		fgets(command, sysconf(_SC_ARG_MAX), stdin); // get the command
		if(!strcmp(command, "exit\n")) {
			return 0; // if the input is 'exit' then exit
		}
		int child_pid = fork();
		if(child_pid) { // parent
			int status;
			waitpid(child_pid, &status, 0); // waiting 4 childs execution & getting its termination status
			printf("\nEXIT STATUS: %d\n\n\n", WEXITSTATUS(status));
			continue;
		}
		else { // child (child_pid = 0)
			char** args = malloc(1000000); // memo 4 args
			args[0] = strtok(command, " \n"); // the command
			for(int i = 1; args[i - 1] != NULL; i++) {
				args[i] = strtok(NULL, " \n");
			}
			execvp(args[0], args); // execute the command
			printf("error!\n"); // if nothing is executed
			return 1;
		}
	}
	return 0;
}
