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
#include <readline/readline.h>
#include <readline/history.h>
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

struct admin_menu_get_info_from_server_argument_t{
	admin_t* my;
	pthread_mutex_t* mut_exit;
};

struct admin_menu_scanf_argument_t{
	admin_t* my;
	pthread_mutex_t* mut_exit;
};

static int print_stat(struct statistick_table_t* stat){
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "login");
  for (int i = 0; i < 6; i++) {
    if (stat->exist[i]) printf("%-10s|", stat->login[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "cash");
  for (int i = 0; i < 6; i++) {
    if (stat->exist[i]) printf("%-10.2lf|", stat->cash[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "rebuy");
  for (int i = 0; i < 6; i++) {
    if (stat->exist[i]) printf("%-10d|", stat->rebuy[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "MAX cash");
  for (int i = 0; i < 6; i++) {
    if (stat->exist[i]) printf("%-10.2lf|", stat->max_cash[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "MIN cash");
  for (int i = 0; i < 6; i++) {
    if (stat->exist[i]) printf("%-10.2lf|", stat->min_cash[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "COUNT all");
  for (int i = 0; i < 6; i++) {
    if (stat->exist[i]) printf("%-10zu|", stat->count_of_game[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "COUNT won");
  for (int i = 0; i < 6; i++) {
    if (stat->exist[i]) printf("%-10zu|", stat->winer[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "won");
  for (int i = 0; i < 6; i++) {
    if (stat->exist[i]) printf("%-10.2lf|", stat->midle_win[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "lost");
  for (int i = 0; i < 6; i++) {
    if (stat->exist[i]) printf("%-10.2lf|", stat->midle_lost[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "MIDLE won");
  for (int i = 0; i < 6; i++) {
    if (stat->exist[i])
      printf("%-10.2lf|", stat->midle_win[i] / ((double)stat->count_of_game[i]));
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "MIDLE lost");
  for (int i = 0; i < 6; i++) {
    if (stat->exist[i])
      printf("%-10.2lf|", stat->midle_lost[i] / ((double)stat->count_of_game[i]));
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  return 0;
}

void* admin_menu_get_info_from_server(void* arguments){
	admin_t* my = ((struct admin_menu_get_info_from_server_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct admin_menu_get_info_from_server_argument_t*)arguments)->mut_exit;
	free((struct admin_menu_get_info_from_server_argument_t*)arguments);

	struct recvsock_admin rec;

	int bytes_read = 0;

	while(1){
whil:
		bytes_read = recv(my->sock, &rec, sizeof(rec), MSG_WAITALL);
		log(my->fd, "bytes_read: %d\n", bytes_read);
		if(bytes_read == 0){
			log(my->fd, "Lost conection with server\n");
			my->status = EXIT;
			pthread_mutex_unlock(mut_exit);
			return NULL;
		}
		if (bytes_read != -1){
			log(my->fd, "recive rec_id: %zu %d\n", rec.id, bytes_read);
			if(rec.id != 0){
				my->id = rec.id;
				if(rec.comand == ONLY_STR){
					printf("\n%s\n", rec.str);
				}else if(rec.comand == SHOWROOMLOG){
					printf("\n");
					print_stat(&(rec.stat));
				}
				printf("->");
				fflush(stdout);
			}else{
				goto whil;
			}
			
		}
	}
	pthread_mutex_unlock(mut_exit);
	return NULL;
}


static const char *newEnv[] = {
	"exit",
	"room",
	"restart",
	"kill",
	"showclient",
	"showstat",
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

static void* admin_menu_scanf(void* arguments){
	admin_t* my = ((struct admin_menu_scanf_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct admin_menu_scanf_argument_t*)arguments)->mut_exit;
	free((struct admin_menu_scanf_argument_t*)arguments);

	char * buf = NULL;

	struct recvsock_admin rec;

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
		snprintf(rec.str, 256, "%s", buf);
		if(!strncmp(buf, "exit", 4)){
			my->status = EXIT;
			rec.id = my->id;
			send(my->sock, &rec, sizeof(rec), 0);
			goto out;
		}else if(!strncmp(buf, "room", 4)){
			rec.id = my->id;
			send(my->sock, &rec, sizeof(rec), 0);
		}
		else if(!strncmp(buf, "restart", 7)){
			rec.id = my->id;
			send(my->sock, &rec, sizeof(rec), 0);
		}else if(!strncmp(buf, "kill", 4)){
			rec.id = my->id;
			send(my->sock, &rec, sizeof(rec), 0);
		}else if(!strncmp(buf, "showclient", 10)){
			rec.id = my->id;
			send(my->sock, &rec, sizeof(rec), 0);
		}else if(!strncmp(buf, "showstat", 8)){
			rec.id = my->id;
			send(my->sock, &rec, sizeof(rec), 0);
		}
	}
	pthread_mutex_unlock(mut_exit);
out:
	if(buf != NULL){
		free(buf);
		buf = NULL;
	}
	return NULL;
}

int admin_t::menu(){

	pthread_mutex_t* mut_exit = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if(pthread_mutex_init(mut_exit, NULL)){
		log(this->fd, "menu mut_exit - init: %s", strerror(errno));
		return 0;	
	}
	pthread_mutex_lock(mut_exit);

	void* argv;
	pthread_t menu_scanf_thread = 0;
	argv = (struct admin_menu_scanf_argument_t*)malloc(sizeof(struct admin_menu_scanf_argument_t));
	((struct admin_menu_scanf_argument_t*)argv)->my = this;
	((struct admin_menu_scanf_argument_t*)argv)->mut_exit = mut_exit;
	pthread_create(&menu_scanf_thread, NULL, admin_menu_scanf, argv);

	pthread_t menu_get_info_from_server_thread = 0;
	argv = (struct admin_menu_get_info_from_server_argument_t*)malloc(sizeof(struct admin_menu_get_info_from_server_argument_t));
	((struct admin_menu_get_info_from_server_argument_t*)argv)->my = this;
	((struct admin_menu_get_info_from_server_argument_t*)argv)->mut_exit = mut_exit;
	pthread_create(&menu_get_info_from_server_thread, NULL, admin_menu_get_info_from_server, argv);

	pthread_mutex_lock(mut_exit);

	pthread_cancel(menu_get_info_from_server_thread);
	pthread_kill(menu_scanf_thread, SIGQUIT);
	pthread_cancel(menu_scanf_thread);

	pthread_join(menu_scanf_thread, NULL);
	pthread_join(menu_get_info_from_server_thread, NULL);

	pthread_mutex_destroy(mut_exit);
	free(mut_exit);

	return 0;
}


admin_t::admin_t(int status, int sock, bool* argument, int fd){
	this->status = status;
	this->fd = fd;
	this->sock = sock;
	for(int i = 0; i < 256; i++){
		this->argument[i] = argument[i];
	}
	this->id = 0;
}

admin_t::~admin_t(){}
