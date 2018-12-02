#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include "button.h"
#include "client.h"


class menu{
public:
	SDL_Surface* screen;
	SDL_Surface* back;
	button* but[256];
	TTF_Font* font;
	int show();
	menu(SDL_Surface*, client_t*);
	~menu();
	int action(SDL_Event* event);
	client_t* my;
};

struct menu_scene_argument_t{
	client_t* my;
	pthread_mutex_t* mut_exit;
	class menu* scene;
};

struct menu_scanf_argument_t{
	client_t* my;
	pthread_mutex_t* mut_exit;
};

struct menu_get_info_from_server_argument_t{
	client_t* my;
	pthread_mutex_t* mut_exit;
};

void* menu_get_info_from_server(void* arguments);
void* menu_scanf(void* arguments);
void* menu_scene(void* arguments);

