#include <errno.h>
#include <sys/poll.h>
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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include "admin.h"
#include <readline/readline.h>
#include <readline/history.h>

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


static const char *newEnv[] = {
	"exit",
	"restart",
	NULL
};

static char *generator(const char *text, int state){
	static int list_index, len;
	char *name;

	if (!state) {
		list_index = 0;
		len = strlen(text);
	}

	while ((name = (char*)newEnv[list_index++])){
		if (strncmp(name, text, len) == 0){
			return strdup(name);
		}
	}

	return NULL;
}

static char **completion(const char *text, int start, int end){
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, generator);
}

struct admin_scanf_argument_t{
	class admin_t* my;
};

static void* menu_scanf(void* arguments){
	admin_t* my = ((struct admin_scanf_argument_t*)arguments)->my;
	free((struct admin_scanf_argument_t*)arguments);

	char * buf = NULL;

	char sendbuf[256];
	for(int i = 0; i < 256; i++){
		sendbuf[i] = 0;
	}

	while(1){
		if(buf != NULL){
			free(buf);
			buf = NULL;
		}
		rl_attempted_completion_function = completion;
		buf = readline("->");
		if(buf == NULL){
			goto out;
		}
		add_history(buf);
		optind = 1;

		snprintf(sendbuf, 256, "%s", buf);

		if(!strncmp(sendbuf, "exit", 4)){
			my->status = EXIT;
			send(my->sock, &sendbuf, sizeof(sendbuf), 0);
			goto out;
		}
		if(!strncmp(sendbuf, "restart", 7)){
			my->status = EXIT;
			send(my->sock, &sendbuf, sizeof(sendbuf), 0);
			goto out;
		}
	}
out:
	if(buf != NULL){
		free(buf);
		buf = NULL;
	}
	return NULL;
}

int admin_t::menu(){
	void* argv;
	argv = (struct admin_scanf_argument_t*)malloc(sizeof(struct admin_scanf_argument_t));
	((struct admin_scanf_argument_t*)argv)->my = this;

	pthread_t menu_scanf_thread = 0;
	pthread_create(&menu_scanf_thread, NULL, menu_scanf, argv);

	pthread_join(menu_scanf_thread, NULL);

	return 0;
}


admin_t::admin_t(int status, int sock, bool* argument, int fd){
	this->status = status;
	this->fd = fd;
	this->sock = sock;
	for(int i = 0; i < 256; i++){
		this->argument[i] = argument[i];
	}
}

admin_t::~admin_t(){}





