#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include "button.h"
#include "client.h"

#define PAGESIZE 23
#define COUNTOFPAGE 256
#define LINESIZE 50

struct page_t{
	char line[PAGESIZE][LINESIZE];
	int optline[PAGESIZE];
	int fontline[PAGESIZE];
};

class help{
private:
	int read_text(int);
public:
	SDL_Surface* screen;
	SDL_Surface* back;
	button* but[256];
	TTF_Font* font;
	TTF_Font* TextFont[3];
	int show();
	help(SDL_Surface*, client_t*, const char*);
	~help();
	int action(SDL_Event* event);

	struct page_t text[COUNTOFPAGE];

	int curent_page;
	int count_of_page;

	client_t* my;
};

struct help_get_info_from_server_argument_t{
	client_t* my;
	pthread_mutex_t* mut_exit;
};

struct help_scanf_argument_t{
	client_t* my;
	pthread_mutex_t* mut_exit;
};

struct help_scene_argument_t{
	client_t* my;
	pthread_mutex_t* mut_exit;
	class help* scene;
};

void* help_get_info_from_server(void* arguments);
void* help_scanf(void* arguments);
void* help_scene(void* arguments);
