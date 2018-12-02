#pragma once
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "play_info.h"
#include "button.h"

#define EXIT -1
#define FIRST_STATUS 0
#define MENU 1
#define HELP 2
#define GAME 3
#define FROM_TO_TABLE 1025
#define SERVER_DISCONNECT_YOU 1026


//STATUS TABLE
#define STATUS_NON_PLAYER -1
#define STATUS_CONECT 1
#define STATUS_ACTIVE 2
#define STATUS_IN_GAME 3
#define STATUS_WINER 4
#define STATUS_AFTER_GAME 5
//SOME DEFINE
#define BUTTON_PERSON 0
#define LIT_BLIND_PERSON 1
#define BIG_BLIND_PERSON 2

//SCREEN OPTION
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_BPP 32

#define ACTION_CALL 10004
#define ACTION_RAISE 10005
#define ACTION_FOLD 10006


class client_t{
public:
	bool argument[256];
	size_t id;
	int status;
	int sock;
	int fd;
	client_t(int status, int sock, bool* argument, int fd);
	~client_t();

	int first_state();
	int change_status(struct recivesock*);

//Menu object
	int menu();
//Help object
	int help();
//Game object
	int final_table[6];
	int game();
	struct publplayinfo player[6];
	struct publtableinfo table;
	int mycard[2];
	int num;
	int rewrite(struct sendinf*);
	int show_all_inf();
	int show_all_inf_to_console();
	int cheak_pok_status(int);

//Visual object
	bool visual_init();
	SDL_Surface* screen;
};


