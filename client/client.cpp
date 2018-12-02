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
#include "menu.h"
#include "help.h"
#include "game.h"

#include "client.h"

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


bool client_t::visual_init(){
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
		return false;
	this->screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	if(this->screen == NULL)
		return false;
	if(TTF_Init() == -1)
		return false;
	SDL_WM_SetCaption("Poker Client", NULL);
	return true;
}

client_t::~client_t(){
	SDL_FreeSurface(this->screen);
	SDL_VideoQuit();
	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	TTF_Quit();
	SDL_Quit();
}

int client_t::cheak_pok_status(int st){
	if(this->player[this->num].status == st)
		return 1;
	return 0;
}

int client_t::show_all_inf(){
	log(this->fd, "\n--------------------------------------------------\n");
	log(this->fd, "\n-----------------------TABLE----------------------\n");
	log(this->fd, "bank = %f, curen bet = %f, card = %d %d %d %d %d\n", this->table.bank, this->table.bet, this->table.card[0], this->table.card[1], this->table.card[2], this->table.card[3], this->table.card[4]);
	log(this->fd, "\n-----------------------PLAYERS--------------------\n");
	for(int i = 0; i<6;i++){
		log(this->fd, "%d", this->player[i].status);	
	}
	for(int i = 0; i<6;i++){
		log(this->fd, "%d", this->final_table[i]);
	}
	log(this->fd, "\n");
	for(int i = 0; i<6;i++){
		log(this->fd, "%d %d %d\n", this->player[i].comb, this->player[i].card[0], this->player[i].card[1]);	
	}
	log(this->fd, "\n");
	log(this->fd, "\n-----------------------MYCARDS--------------------\n");
	log(this->fd, "status = %d; %d %d\n", this->player[this->num].status, this->mycard[0], this->mycard[1]);
	log(this->fd, "\n--------------------------------------------------\n");
	return 0;
}

int client_t::rewrite(struct sendinf * newinf){
//copy card
	this->mycard[0] = newinf->card[0];
	this->mycard[1] = newinf->card[1];
	this->num = newinf->num;
//copy table	
	this->table.bank = newinf->table.bank;
	this->table.bet = newinf->table.bet;
	this->table.blind = newinf->table.blind;
	this->table.num = newinf->table.num;
	for(int i = 0; i < 5; i++){
		this->table.card[i] = newinf->table.card[i];
	}
//copy all players
	for(int i = 0 ; i < 6; i++){
		this->final_table[i] = newinf->final_table[i];
		this->player[i].cash = newinf->player[i].cash;
		this->player[i].last_action = newinf->player[i].last_action;
		this->player[i].bet = newinf->player[i].bet;
		this->player[i].status = newinf->player[i].status;
		this->player[i].blind = newinf->player[i].blind;
		this->player[i].comb = newinf->player[i].comb;
		this->player[i].card[0] = newinf->player[i].card[0];
		this->player[i].card[1] = newinf->player[i].card[1];		
		strncpy(this->player[i].login, newinf->player[i].login, sizeof(newinf->player[i].login));
	}
	this->player[this->num].card[0] = newinf->card[0];
	this->player[this->num].card[1] = newinf->card[1];

	return 0;
}

client_t::client_t(int status, int sock, bool* argument, int fildis){
	this->fd = fildis;
	for(int i = 0; i < 256; i++){
		this->argument[i] = argument[i];
	}
	this->status = status;
	this->sock = sock;
	this->num = 0;
	this->id = 0;
	for(int i = 0; i < 5; i++){
		this->table.card[i] = 0;
	}
	for(int i = 0; i < 6; i++){
		this->final_table[i] = 0;
	}
	this->table.bet = 0;
	this->table.bank = 0;
	this->table.num = -1;
	for(int i = 0 ; i < 6; i++){
		this->player[i].cash = 0;
		this->player[i].last_action = 0;
		this->player[i].bet = 0;
		this->player[i].status = -1;
		this->player[i].blind = 0;
		this->player[i].card[0] = 0;
		this->player[i].card[1] = 0;
		this->player[i].comb = 0;
		for(size_t j = 0; j < sizeof(this->player[i].login); j++){
			this->player[i].login[j] = 0;
		}
	}
	this->screen = NULL;
	if(!this->argument['T']){
		this->visual_init();
	}
	
}

int client_t::change_status(struct recivesock* rec){
	this->status = rec->code;
	if(rec->code == EXIT){
		return EXIT;
	}
	if(rec->code == FIRST_STATUS){
		return FIRST_STATUS;
	}
	if(rec->code == MENU){
		return MENU;
	}
	if(rec->code == HELP){
		return HELP;
	}
	if(rec->code == GAME){
		return GAME;
	}
	return 0;
}

static size_t myscanf(char* dest){
	struct pollfd fds;
	fds.fd = 0;
	fds.events = POLLIN;
	if(poll(&fds, 1, 0) != 0){
		std::cin.getline(dest, 256);
		return 1;
	}
	return 0;
}

int client_t::first_state(){
	log(this->fd, "first status\n");

	if(this->argument['t']){
		printf("first status\n");
	}

	int bytes_read = 0;
	char rbuf[256];
	for(size_t i = 0; i < sizeof(rbuf); i++){
		rbuf[i] = 0;	
	}
	struct recivesock rec;
	while(1){
		bytes_read = recv(sock, &rec, sizeof(rec), MSG_DONTWAIT);
		if(bytes_read == 0){
			log(this->fd, "Lost conection whith server\n");
			this->status = EXIT;
			return EXIT;
		}
		if (bytes_read != -1){
			this->id = rec.id;
			if(rec.code < 1024){
				return this->change_status(&rec);
			}
		}
		if(myscanf(rbuf)){
			if(!strncmp(rbuf, "exit", 4)){
				this->status = EXIT;
				return EXIT;
			}
		}
	}

	return 0;
}



int client_t::menu(){
	log(this->fd, "menu\n");

	if(this->argument['t']){
		printf("menu\n");
	}

	class menu scene(this->screen, this);
	if(!this->argument['T']){
		scene.show();
	}

	int bytes_read = 0;
	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	struct recivesock rec;

	pthread_mutex_t* mut_exit = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if(pthread_mutex_init(mut_exit, NULL)){
		log(this->fd, "menu mut_exit - init: %s", strerror(errno));
		return 0;	
	}
	pthread_mutex_lock(mut_exit);

	void* argv;

	pthread_t menu_get_info_from_server_thread = 0;
	argv = (struct menu_get_info_from_server_argument_t*)malloc(sizeof(struct menu_get_info_from_server_argument_t));
	((struct menu_get_info_from_server_argument_t*)argv)->my = this;
	((struct menu_get_info_from_server_argument_t*)argv)->mut_exit = mut_exit;
	pthread_create(&menu_get_info_from_server_thread, NULL, menu_get_info_from_server, argv);

	pthread_t menu_scanf_thread = 0;
	if(this->argument['t']){		
		argv = (struct menu_scanf_argument_t*)malloc(sizeof(struct menu_scanf_argument_t));
		((struct menu_scanf_argument_t*)argv)->my = this;
		((struct menu_scanf_argument_t*)argv)->mut_exit = mut_exit;
		pthread_create(&menu_scanf_thread, NULL, menu_scanf, argv);
	}

	pthread_t menu_scene_thread = 0;
	if(!this->argument['T']){
		argv = (struct menu_scene_argument_t*)malloc(sizeof(struct menu_scene_argument_t));
		((struct menu_scene_argument_t*)argv)->my = this;
		((struct menu_scene_argument_t*)argv)->mut_exit = mut_exit;
		((struct menu_scene_argument_t*)argv)->scene = &scene;
		pthread_create(&menu_scene_thread, NULL, menu_scene, argv);
	}
		
	pthread_mutex_lock(mut_exit);
	
	pthread_cancel(menu_get_info_from_server_thread);
	
	if(this->argument['t'])
		pthread_cancel(menu_scanf_thread);

	if(!this->argument['T']){
		SDL_Event event;
		SDL_memset(&event, 0, sizeof(event));
	   	event.type = SDL_USEREVENT;
		SDL_PushEvent(&event);
	}

	pthread_join(menu_get_info_from_server_thread, NULL);
	if(this->argument['t'])
		pthread_join(menu_scanf_thread, NULL);

	if(!this->argument['T']){
		pthread_join(menu_scene_thread, NULL);
	}

	pthread_mutex_destroy(mut_exit);
	free(mut_exit);
	return 0;
}


int client_t::help(){
	log(this->fd, "help\n");

	if(this->argument['t']){
		printf("help\n");
	}

	class help scene(this->screen, this, "../client/source/help.help");
	if(!this->argument['T']){
		scene.show();
	}

	int bytes_read = 0;
	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	struct recivesock rec;

	pthread_mutex_t mut_exit;
	if(pthread_mutex_init(&mut_exit, NULL)){
		log(this->fd, "menu mut_exit - init: %s", strerror(errno));
		return 0;	
	}
	pthread_mutex_lock(&mut_exit);

	void* argv;

	pthread_t help_get_info_from_server_thread = 0;
	argv = (struct help_get_info_from_server_argument_t*)malloc(sizeof(struct help_get_info_from_server_argument_t));
	((struct help_get_info_from_server_argument_t*)argv)->my = this;
	((struct help_get_info_from_server_argument_t*)argv)->mut_exit = &mut_exit;
	pthread_create(&help_get_info_from_server_thread, NULL, help_get_info_from_server, argv);

	pthread_t help_scanf_thread = 0;
	if(this->argument['t']){		
		argv = (struct help_scanf_argument_t*)malloc(sizeof(struct help_scanf_argument_t));
		((struct help_scanf_argument_t*)argv)->my = this;
		((struct help_scanf_argument_t*)argv)->mut_exit = &mut_exit;
		pthread_create(&help_scanf_thread, NULL, help_scanf, argv);
	}

	pthread_t help_scene_thread = 0;
	if(!this->argument['T']){
		argv = (struct help_scene_argument_t*)malloc(sizeof(struct help_scene_argument_t));
		((struct help_scene_argument_t*)argv)->my = this;
		((struct help_scene_argument_t*)argv)->mut_exit = &mut_exit;
		((struct help_scene_argument_t*)argv)->scene = &scene;
		pthread_create(&help_scene_thread, NULL, help_scene, argv);
	}

	pthread_mutex_lock(&mut_exit);
	pthread_cancel(help_get_info_from_server_thread);
	
	if(this->argument['t'])
		pthread_cancel(help_scanf_thread);

	if(!this->argument['T']){
		SDL_Event event;
		SDL_memset(&event, 0, sizeof(event));
	   	event.type = SDL_USEREVENT;
		SDL_PushEvent(&event);
	}

	pthread_join(help_get_info_from_server_thread, NULL);
	if(this->argument['t'])
		pthread_join(help_scanf_thread, NULL);
	if(!this->argument['T']){
		pthread_join(help_scene_thread, NULL);
	}

	pthread_mutex_destroy(&mut_exit);
	return 0;
}

int client_t::game(){
	log(this->fd, "game\n");

	if(this->argument['t']){
		printf("game\n");
	}
	class game scene(this->screen, this);
	if(!this->argument['T']){
		scene.show();
	}


	int bytes_read = 0;
	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	struct recivesock rec;

	pthread_mutex_t mut_exit;
	if(pthread_mutex_init(&mut_exit, NULL)){
		log(this->fd, "game mut_exit - init: %s", strerror(errno));
		return 0;	
	}
	pthread_mutex_lock(&mut_exit);
	pthread_mutex_t mut_sdl;
	if(!this->argument['T']){
		if(pthread_mutex_init(&mut_sdl, NULL)){
			log(this->fd, "game mut_sdl - init: %s", strerror(errno));
			return 0;
		}
	}
	void* argv;
	pthread_t game_get_info_from_server_thread = 0;
	argv = (struct game_get_info_from_server_argument_t*)malloc(sizeof(struct game_get_info_from_server_argument_t));
	((struct game_get_info_from_server_argument_t*)argv)->my = this;
	((struct game_get_info_from_server_argument_t*)argv)->mut_exit = &mut_exit;
	((struct game_get_info_from_server_argument_t*)argv)->mut_sdl = &mut_sdl;
	((struct game_get_info_from_server_argument_t*)argv)->scene = &scene;
	pthread_create(&game_get_info_from_server_thread, NULL, game_get_info_from_server, argv);
	pthread_t game_scanf_thread = 0;
	if(this->argument['t']){
		argv = (struct game_scanf_argument_t*)malloc(sizeof(struct game_scanf_argument_t));
		((struct game_scanf_argument_t*)argv)->my = this;
		((struct game_scanf_argument_t*)argv)->mut_exit = &mut_exit;
		pthread_create(&game_scanf_thread, NULL, game_scanf, argv);
	}

	pthread_t game_scene_thread = 0;
	if(!this->argument['T']){
		argv = (struct game_scene_argument_t*)malloc(sizeof(struct game_scene_argument_t));
		((struct game_scene_argument_t*)argv)->my = this;
		((struct game_scene_argument_t*)argv)->mut_exit = &mut_exit;
		((struct game_scene_argument_t*)argv)->mut_sdl = &mut_sdl;
		((struct game_scene_argument_t*)argv)->scene = &scene;
		pthread_create(&game_scene_thread, NULL, game_scene, argv);
	}
	pthread_mutex_lock(&mut_exit);
	
	pthread_cancel(game_get_info_from_server_thread);
	
	if(this->argument['t'])
		pthread_cancel(game_scanf_thread);

	if(!this->argument['T']){
		SDL_Event event;
		SDL_memset(&event, 0, sizeof(event));
	   	event.type = SDL_USEREVENT;
		SDL_PushEvent(&event);
	}
	pthread_join(game_get_info_from_server_thread, NULL);

	if(this->argument['t'])
		pthread_join(game_scanf_thread, NULL);

	if(!this->argument['T']){
		pthread_join(game_scene_thread, NULL);
	}
	pthread_mutex_destroy(&mut_exit);

	if(!this->argument['T']){
		pthread_mutex_destroy(&mut_sdl);
	}
	return 0;
}

