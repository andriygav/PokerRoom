#pragma once
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include "button.h"
#include "slide.h"
#include "client.h"


class game{
public:
	SDL_Surface* screen;
	SDL_Surface* back;
	button* but[256];

	slide* slide_raise;
	slide* slide_getmoney;
	slide* slide_putmoney;


	TTF_Font* font;
	int show();
	game(SDL_Surface*, client_t*);
	~game();
	int action(SDL_Event* event);
	client_t* my;

	SDL_Surface* card_texture[54];
	
	char buf[256];

};


struct game_scene_argument_t{
	client_t* my;
	pthread_mutex_t* mut_exit;
	pthread_mutex_t* mut_sdl;
	class game* scene;
};

struct game_scanf_argument_t{
	client_t* my;
	pthread_mutex_t* mut_exit;
};

struct game_get_info_from_server_argument_t{
	client_t* my;
	pthread_mutex_t* mut_exit;
	pthread_mutex_t* mut_sdl;
	class game* scene;
};

void* game_get_info_from_server(void* arguments);
void* game_scanf(void* arguments);
void* game_scene(void* arguments);

