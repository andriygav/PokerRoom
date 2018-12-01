#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "client.h"
#include <getopt.h>

int set_arguments(bool* argument, int argc, char** argv){
	int ret = 0;
	int val = 0;

	struct option long_options[1] = {	
		{"terminal", 0, NULL,  't'},
        };

	int a = 0;

	while((val = getopt_long(argc, argv, "tg", long_options, &a)) != -1){
		if(val == '?'){
			printf("Try '%s --help' for more information.\n", argv[0]);
			goto out;
		}
		argument[val] = true;
	}
out:
	return ret;
}

int main(int argc, char **argv){
	std::cin.clear();
//Conect(begin)
	int sock = 0;

	bool argument[256];
	
	for(int i = 0; i < 256; i++){
		argument[i] = false;
	}
	set_arguments(argument, argc, argv);

	char str[256];
	for(int i = 0; i < 256; i++){
		str[i] = 0;
	}
	sprintf(str, "log/SecondBotLog%d.log", getpid());
	int fd = open(str, O_CREAT|O_RDWR|O_APPEND, 0666);
	if(fd < 0){
		perror(str);
		return 1;
	}


	struct sockaddr_in addr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		log(fd, "socket", strerror(errno));
		exit(1);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3425/*atoi(argv[1])*/);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"/*argv[2]*/);
	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		log(fd, "Conect", strerror(errno));
		return 1;
   	}
//Conect(end)
	client obj(0, sock, argument, fd);
	while(1){
		switch(obj.status){
			case FIRST_STATUS:
				obj.first_state();
				break;
			case MENU:
				obj.menu();
				break;
			case HELP:
				obj.help();
				break;
			case GAME:
				obj.game();
				break;
			default:
				goto out;
		}
	}
out:
	return 0;
}
