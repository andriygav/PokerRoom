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
#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "client.h"
#include "admin.h"

static int log(int md, const char* format, ...){
	char str[256];
	va_list args;
	va_start(args, format);
	vsprintf(str, format, args);
	va_end(args);	
	if(write(md, str, strnlen(str, 256)) == -1){
		perror("write");
	}
	return 0;
}


int set_arguments(bool* argument, int argc, char** argv){
	int ret = 0;
	int val = 0;

	struct option long_options[1] = {	
		{"terminal", 0, NULL,  't'},
        };

	int a = 0;
	optind = 1;
	while((val = getopt_long(argc, argv, "Ttp:a:", long_options, &a)) != -1){
		if(val == '?'){
			printf("Try '%s --help' for more information.\n", argv[0]);
			goto out;
		}
		argument[val] = true;
		if(val == 'T'){
			argument['t'] = true;
		}
	}
out:
	return ret;
}
#undef main

#define ADMIN 0
#define CLIENT 1

#pragma pack(push, 1)
struct authentication_t {
  char login[256];
  char password[256];
  int status;
};
#pragma pack(pop)

int main(int argc, char **argv){
	std::cin.clear();
//Conect(begin)
	int sock = 0;
	bool argument[256];
	for(int i = 0; i < 256; i++){
		argument[i] = false;
	}
	set_arguments(argument, argc, argv);

	int given_port = 3425;
	char* given_addres = (char*)"127.0.0.1";

	optind = 1;
	optarg = NULL;
	int opt = 0;
	while((opt = getopt(argc, argv, "Ttp:a:")) != -1) {
		switch (opt){
			case 'p':
				given_port = atoi(optarg);
				optarg = NULL;
				break;
			case 'a':
				given_addres = optarg;
				break;
			default:
				break;
		}
	}

	char str[256];
	for(int i = 0; i < 256; i++){
		str[i] = 0;
	}
	sprintf(str, "log/ClientLog%d.log", getpid());
	int fd = open(str, O_CREAT|O_RDWR|O_APPEND, 0666);
	if(fd < 0){
		perror(str);
		return 1;
	}

	struct sockaddr_in addr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("socket");
		return 1;
	}

	struct authentication_t authent_rec;
	authent_rec.status = -1;
   	if(argument['t']){
   		printf("%s\n", "Enter your login:");
   		scanf("%250s", authent_rec.login);
   		printf("%s\n", "Enter your password:");
   		scanf("%250s", authent_rec.password);
   	}else{
   		sprintf(authent_rec.login, "client");
   		sprintf(authent_rec.password, "client");
   	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(given_port);
	addr.sin_addr.s_addr = inet_addr(given_addres);
	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		log(fd, "Connect: %s", strerror(errno));
		return 1;
   	}

   	send(sock, &authent_rec, sizeof(authent_rec), 0);
   	int bytes_read = 0;
   	bytes_read = recv(sock, &authent_rec, sizeof(authent_rec), 0);
    if (bytes_read == 0) {
    	log(fd, "Connect: %s", strerror(errno));
    	return 1;
    }
//Conect(end)
    if(authent_rec.status == CLIENT){
		client_t obj(FIRST_STATUS, sock, argument, fd, authent_rec.login);
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
	}else{
		admin_t obj(MENU, sock, argument, fd);
		while(1){
			switch(obj.status){
				case MENU:
					obj.menu();
					break;
				default:
					goto out;
			}
		}
	}
out:
	close(fd);
	return 0;
}
