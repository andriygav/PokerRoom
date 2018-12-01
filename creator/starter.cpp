/*
Запускает указаную вами программу в аргументах командной строки и пишет время работы этой програмы.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char* argv[]){
	int ret = 0;
	if (argc < 3){
		printf("ERROR arguments\n");
		return 0;
	}
	int N = atoi(argv[1]); 
	for(int i = 0; i < N; i++){
		pid_t pid = fork();

		if(pid == 0){
			execvp(argv[2], &argv[2]);
			perror(argv[0]);
			goto out;
		}
	}
	
	for(int i = 0;i < N; i++){
		int status;
		wait(&status);
	}
out:
	return ret;
}
