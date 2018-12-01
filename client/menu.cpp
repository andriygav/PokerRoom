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
	strncpy(rbuf.login, "client", 256);
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
}

menu::menu(SDL_Surface* screen, client_t* my){
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

int menu::action(SDL_Event* event){
	for(int i = 0; i < COUNT_OF_BUTTON; i++){
		if(this->but[i] != NULL){
			if(this->but[i]->clicking(event)){
				this->but[i]->drow(this->screen);
				SDL_Flip(this->screen);
			}
		}
	}
}

menu::~menu(){
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


void* menu_get_info_from_server(void* arguments){
	client_t* my = ((struct menu_get_info_from_server_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct menu_get_info_from_server_argument_t*)arguments)->mut_exit;
	free((struct menu_get_info_from_server_argument_t*)arguments);

	struct recivesock rec;
	while(1){
		if(recv(my->sock, &rec, sizeof(rec), 0) == 0){
			log(my->fd, "Lost conection whith server\n");
			my->status = EXIT;
			pthread_mutex_unlock(mut_exit);
			return NULL;
		}
		my->id = rec.id;
		if(rec.code < 1024){
			my->change_status(&rec);
			pthread_mutex_unlock(mut_exit);
			return NULL;
		}
	}
	pthread_mutex_unlock(mut_exit);
	return NULL;
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
	while(1){
		printf("->%c", '\0');
		scanf("%[^\n]s", rbuf.buf);
		char c = 0;
		scanf("%c", &c);
		if(!strncmp(rbuf.buf, "exit", 4)){
			my->status = EXIT;
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		} else if(!strncmp(rbuf.buf, "game", 4)){
			rbuf.id = my->id;
			strncpy(rbuf.login, "client", 256);
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		} else if(!strncmp(rbuf.buf, "help", 4)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		}
	}
	pthread_mutex_unlock(mut_exit);
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
