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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "menu.h"

#include <readline/readline.h>
#include <readline/history.h>

#define COUNT_OF_BUTTON 3

struct str_t{
	client_t* my;
	char buf[256];
};

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

static SDL_Surface* load_image(const char * filename){
	SDL_Surface* load = SDL_LoadBMP(filename);
	if(load != NULL){
		SDL_Surface* optimal = SDL_DisplayFormat(load);
		SDL_FreeSurface(load);
		Uint32 colorkey = SDL_MapRGB(optimal->format, 200, 200, 200);
		SDL_SetColorKey(optimal, SDL_SRCCOLORKEY, colorkey );
		return optimal;
	}
	return NULL;
}

static struct str_t* set_str_t(void* my, const char* buf){
	str_t* tmp = (str_t*)malloc(sizeof(str_t));
	for(int i = 0; i < 256; i++)
		tmp->buf[i] = 0;
	tmp->my = (client_t*)my;
	sprintf(tmp->buf, "%s", buf);
	return tmp;
}

static void menu_button_click_func(void* pointer){
	struct str_t* tmp = (str_t*)pointer;
	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	rbuf.id = tmp->my->id;
	sprintf(rbuf.buf, "%s", tmp->buf);
	strncpy(rbuf.login, tmp->my->login, 256);
	send(tmp->my->sock, &rbuf, sizeof(rbuf), 0);
	return;
}

static void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* dest){
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;
	SDL_BlitSurface(source, NULL, dest, &offset);
	return;
}

int menu::show(){
	apply_surface(0, 0, this->back, this->screen);
	for(int i = 0; i < COUNT_OF_BUTTON; i++){
		if(this->but[i])
			this->but[i]->drow(this->screen);
	};
	SDL_Flip(this->screen);
	return 0;
}

menu::menu(SDL_Surface* screen, client_t* my){
	this->screen = NULL;
	if(!my->argument['T']){
		this->back = load_image("../client/VisSource/background.bmp");
		for(int i = 0; i < 256; i++)
			this->but[i] = NULL;
		this->my = my;
		this->screen = screen;
		struct str_t* tmp = NULL;
		this->font = TTF_OpenFont("../client/VisSource/font.ttf", 24);
	 	tmp = set_str_t(this->my, "exit");
		this->but[0] = new button(600, 0, 200, 30, "../client/VisSource/button.bmp", "exit", this->font, (void *)menu_button_click_func, tmp);
	 	tmp = set_str_t(this->my, "help");
		this->but[1] = new button(600, 35, 200, 30, "../client/VisSource/button.bmp", "help", this->font, (void *)menu_button_click_func, tmp);
	 	tmp = set_str_t(this->my, "game");
		this->but[2] = new button(600, 70, 200, 30, "../client/VisSource/button.bmp", "game", this->font, (void *)menu_button_click_func, tmp);
	}
}

int menu::action(SDL_Event* event){
	for(int i = 0; i < COUNT_OF_BUTTON; i++){
		if(this->but[i] != NULL){
			if(this->but[i]->clicking(event)){
				this->but[i]->drow(this->screen);
				SDL_Flip(this->screen);
			}
		}
	}
	return 0;
}

menu::~menu(){
	if(this->screen != NULL){
		this->screen = NULL;
		TTF_CloseFont(this->font);
		SDL_FreeSurface(this->back);
		this->font = NULL;
		for(int i = 0; i < 256; i++){
			if(this->but[i] != NULL){
				if(this->but[i]->action_data)
					free(this->but[i]->action_data);
				delete(this->but[i]);
			}
			this->but[i] = NULL;
		}
	}
}


void* menu_get_info_from_server(void* arguments){
	client_t* my = ((struct menu_get_info_from_server_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct menu_get_info_from_server_argument_t*)arguments)->mut_exit;
	free((struct menu_get_info_from_server_argument_t*)arguments);

	struct recivesock rec;
	int bytes_read = 0;

	while(1){
		bytes_read = recv(my->sock, &rec, sizeof(rec), 0);
		log(my->fd, "bytes_read: %d\n", bytes_read);
		if(bytes_read == 0){
			log(my->fd, "Lost conection with server\n");
			my->status = EXIT;
			pthread_mutex_unlock(mut_exit);
			return NULL;
		}
		if (bytes_read != -1){
			log(my->fd, "recive rec_code: %d %zu %d\n", rec.code, rec.id, bytes_read);
			if(rec.id != 0){
				my->id = rec.id;
			}
			if(rec.code < 1024){
				if (my->change_status(&rec) != 0){
					pthread_mutex_unlock(mut_exit);
					return NULL;
				}
			}
		}
	}
	pthread_mutex_unlock(mut_exit);
	return NULL;
}
static const char *newEnv[] = {
	"exit",
	"game",
	"help",
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

void* menu_scanf(void* arguments){
	client_t* my = ((struct menu_scanf_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct menu_scanf_argument_t*)arguments)->mut_exit;
	free((struct menu_scanf_argument_t*)arguments);

	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	char c;

	char * buf = NULL;

	while(1){
		if(buf != NULL){
			free(buf);
			buf = NULL;
		}
		rl_attempted_completion_function = completion;
		buf = readline("->menu->");
		add_history(buf);
		optind = 1;
		snprintf(rbuf.buf, 256, "%s", buf);
		if(!strncmp(rbuf.buf, "exit", 4)){
			my->status = EXIT;
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
			goto out;
		} else if(!strncmp(rbuf.buf, "game", 4)){
			rbuf.id = my->id;
			strncpy(rbuf.login, my->login, 256);
			send(my->sock, &rbuf, sizeof(rbuf), 0);
			goto out;
		} else if(!strncmp(rbuf.buf, "help", 4)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
			goto out;
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


void* menu_scene(void* arguments){
	client_t* my = ((struct menu_scene_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct menu_scene_argument_t*)arguments)->mut_exit;
	class menu* scene = ((struct menu_scene_argument_t*)arguments)->scene;
	free((struct menu_scene_argument_t*)arguments);

	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	SDL_Event event;
	while(1){
		SDL_WaitEvent(&event);
		if(event.type == SDL_QUIT){
			my->status = EXIT;
			rbuf.id = my->id;
			sprintf(rbuf.buf, "exit");
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		}
		if(event.type == SDL_USEREVENT){
			return NULL;
		}
		scene->action(&event);
	}
	pthread_mutex_unlock(mut_exit);
	return NULL;
}
