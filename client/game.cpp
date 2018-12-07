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
#include <unistd.h>
#include <fcntl.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "game.h"


#define COUNT_OF_BUTTON 10

#define MAX_PUT_MONEY 20000

struct str_t{
	client_t* my;
	char buf[256];
	slide* slid;
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
		Uint32 colorkey = SDL_MapRGB(optimal->format, 128, 128, 0);
		SDL_SetColorKey(optimal, SDL_SRCCOLORKEY, colorkey);
		return optimal;
	}
	return NULL;
}

static struct str_t* set_str_t(void* my, const char* buf, void* slid){
	str_t* tmp = (str_t*)malloc(sizeof(str_t));
	for(int i = 0; i < 256; i++)
		tmp->buf[i] = 0;
	tmp->my = (client_t*)my;
	tmp->slid = (slide*)slid;
	sprintf(tmp->buf, "%s", buf);
	return tmp;
}

static void game_button_click_func(void* pointer){
	struct str_t* tmp = (str_t*)pointer;
	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	rbuf.id = tmp->my->id;
	sprintf(rbuf.buf, "%s", tmp->buf);
	if(!strncmp(tmp->buf, "raise", 5) || !strncmp(tmp->buf, "getmoney", 8)){
		sprintf(rbuf.buf, "%s %lf", tmp->buf, tmp->slid->value(tmp->my->player[tmp->my->num].cash));
	}
	if(!strncmp(tmp->buf, "putmoney", 8)){
		sprintf(rbuf.buf, "%s %lf", tmp->buf, tmp->slid->value(MAX_PUT_MONEY));
	}
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


static int ret_x(int i){
	int x = 0;
	switch(i){
		case 0:
			x = 240;
		break;
		case 5:
			x = 10;
		break;
		case 1:
			x = 470;
		break;
		case 4:
			x = 10;
		break;
		case 2:
			x = 470;
		break;
		case 3:
			x = 240; 
		break;
		case 6:
			x = 145;
		break;
	}
	return x;
}

static int ret_y(int i){
	int y = 0;
	switch(i){
		case 0:
			y = 60;
		break;
		case 5:
			y = 170;
		break;
		case 1:
			y = 170;
		break;
		case 4:
			y = 375;
		break;
		case 2:
			y = 375;
		break;
		case 3:
			y = 430; 
		break;
		case 6:
			y = 275; 
		break;
	}
	return y;
}

int game::show(){
	apply_surface(0, 0, this->back, this->screen);
	SDL_Color text_color = {0, 0, 255};
	SDL_Surface* text_surf = NULL;
	SDL_Rect offset = {0, 0, 0, 0};

//Вывод номера комнаты
	sprintf(this->buf, "ROOM NUMBER IS %d", this->my->table.num);
	text_surf = TTF_RenderText_Solid(this->font, this->buf, text_color);
	apply_surface(5, 5, text_surf, this->screen);
	SDL_FreeSurface(text_surf);

//Вывод карт

	for(int i = 0; i < 6; i++){
		text_color.r = 0;
		text_color.g = 0;
		text_color.b = 0;

		apply_surface(ret_x(i), ret_y(i), this->card_texture[53], this->screen);
		apply_surface(ret_x(i) + 62 + 2, ret_y(i), this->card_texture[53], this->screen);

		if(this->my->player[i].card[0] != -1){
			apply_surface(ret_x(i) + 2, ret_y(i) + 2, this->card_texture[this->my->player[i].card[0]], this->screen);
			apply_surface(ret_x(i) + 62 + 2 + 2, ret_y(i) + 2, this->card_texture[this->my->player[i].card[1]], this->screen);
		}
//Вывод информации об игроке
		if(this->my->player[i].status == STATUS_ACTIVE){
			text_color.r = 255;
			text_color.g = 0;
			text_color.b = 0;
		}
		if(this->my->final_table[i] == 1){
			text_color.r = 0;
			text_color.g = 255;
			text_color.b = 0;
		}


		text_surf = TTF_RenderText_Solid(this->font, this->my->player[i].login, text_color);
		SDL_GetClipRect(text_surf, &offset);
		apply_surface(ret_x(i) + (130 - offset.w)/2, ret_y(i) - 30, text_surf, this->screen);
		SDL_FreeSurface(text_surf);

		sprintf(this->buf, "%.1lf", this->my->player[i].cash);
		text_surf = TTF_RenderText_Solid(this->font, this->buf, text_color);
		SDL_GetClipRect(text_surf, &offset);
		apply_surface(ret_x(i) + (130 - offset.w)/2, ret_y(i) + 95, text_surf, this->screen);
		SDL_FreeSurface(text_surf);

		sprintf(this->buf, "(%.1lf)", this->my->player[i].bet);
		text_surf = TTF_RenderText_Solid(this->font, this->buf, text_color);
		SDL_GetClipRect(text_surf, &offset);
		apply_surface(ret_x(i) + (130 - offset.w)/2, ret_y(i) + 95 + 25, text_surf, this->screen);
		SDL_FreeSurface(text_surf);


	}
	text_color.r = 0;
	text_color.g = 0;
	text_color.b = 0;
	int x = ret_x(6);
	int y = ret_y(6);

	for(int i = 0; i < 5; i++){
		apply_surface(x + i* 64, y, this->card_texture[53], this->screen);
		if(this->my->table.card[i] != 0)
			apply_surface(x + 2 + i*64, y + 2, this->card_texture[this->my->table.card[i]], this->screen);
	}
	sprintf(this->buf, "%.1lf", this->my->table.bank);
	text_surf = TTF_RenderText_Solid(this->font, this->buf, text_color);
	SDL_GetClipRect(text_surf, &offset);
	apply_surface(x + (318 - offset.w)/2, y + 95, text_surf, this->screen);
	SDL_FreeSurface(text_surf);

	sprintf(this->buf, "%.1lf", this->my->table.bet);
	text_surf = TTF_RenderText_Solid(this->font, this->buf, text_color);
	SDL_GetClipRect(text_surf, &offset);
	apply_surface(x + (318 - offset.w)/2, y - 30, text_surf, this->screen);
	SDL_FreeSurface(text_surf);


//Текстовое меню
	for(int i = 0; i < COUNT_OF_BUTTON; i++){
		if(this->but[i])
			this->but[i]->drow(this->screen);
	}
	if(this->slide_raise){
		this->slide_raise->drow(this->screen, this->my->player[this->my->num].cash);
	}
	if(this->slide_getmoney){
		this->slide_getmoney->drow(this->screen, this->my->player[this->my->num].cash);
	}
	if(this->slide_putmoney){
		this->slide_putmoney->drow(this->screen, MAX_PUT_MONEY);
	}

	SDL_Flip(this->screen);
	return 0;
}

game::game(SDL_Surface* screen, client_t* my){
	this->screen = NULL;

	if(!my->argument['T']){
		for(int i = 0; i < 256; i++)
			this->buf[i] = 0;
		this->back = load_image("../client/VisSource/background.bmp");
		for(int i = 0; i < 256; i++)
			this->but[i] = NULL;

		this->slide_raise = NULL;
		this->slide_getmoney = NULL;

		char name[256];
		for(int i = 0; i < 256; i++){
			name[i] = 0;
		}
		for(int i = 0; i < 54; i++){
			sprintf(name, "../client/colum/%d.bmp", i);
			this->card_texture[i] = load_image(name);
		}

		this->my = my;
		this->screen = screen;
		struct str_t* tmp = NULL;
		this->font = TTF_OpenFont("../client/VisSource/font.ttf", 24);

		this->slide_raise = new slide(600, 450, 200, 50, "../client/VisSource/slide.bmp", "../client/VisSource/slideback.bmp", "../client/VisSource/line.bmp", this->font);
		this->slide_raise->status = false;
		this->slide_getmoney = new slide(600, 210, 200, 50, "../client/VisSource/slide.bmp", "../client/VisSource/slideback.bmp", "../client/VisSource/line.bmp", this->font);
		this->slide_getmoney->status = false;
		this->slide_putmoney = new slide(600, 300, 200, 50, "../client/VisSource/slide.bmp", "../client/VisSource/slideback.bmp", "../client/VisSource/line.bmp", this->font);
		this->slide_putmoney->status = false;


		tmp = set_str_t(my, "exit", NULL);
		this->but[0] = new button(600, 0, 200, 30, "../client/VisSource/button.bmp", "exit", this->font, (void *)game_button_click_func, tmp);

	 	tmp = set_str_t(my, "menu", NULL);
		this->but[1] = new button(600, 35, 200, 30, "../client/VisSource/button.bmp", "menu", this->font, (void *)game_button_click_func, tmp);

	 	tmp = set_str_t(my, "help", NULL);
		this->but[2] = new button(600, 70, 200, 30, "../client/VisSource/button.bmp", "help", this->font, (void *)game_button_click_func, tmp);

	 	tmp = set_str_t(my, "disconnect", NULL);
		this->but[3] = new button(600, 105, 200, 30, "../client/VisSource/button.bmp", "disconnect", this->font, (void *)game_button_click_func, tmp);

		tmp = set_str_t(my, "new", NULL);
		this->but[4] = new button(600, 140, 200, 30, "../client/VisSource/button.bmp", "new game", this->font, (void *)game_button_click_func, tmp);
		this->but[4]->status = false;

		tmp = set_str_t(my, "call", NULL);
		this->but[5] = new button(600, 500, 200, 30, "../client/VisSource/button.bmp", "call", this->font, (void *)game_button_click_func, tmp);
		this->but[5]->status = false;

	 	tmp = set_str_t(my, "raise", this->slide_raise);
		this->but[6] = new button(600, 535, 200, 30, "../client/VisSource/button.bmp", "raise", this->font, (void *)game_button_click_func, tmp);
		this->but[6]->status = false;	
		
		tmp = set_str_t(my, "fold", NULL);
		this->but[7] = new button(600, 570, 200, 30, "../client/VisSource/button.bmp", "fold", this->font, (void *)game_button_click_func, tmp);
		this->but[7]->status = false;

		tmp = set_str_t(my, "getmoney", this->slide_getmoney);
		this->but[8] = new button(600, 175, 200, 30, "../client/VisSource/button.bmp", "get money", this->font, (void *)game_button_click_func, tmp);
		this->but[8]->status = false;

		tmp = set_str_t(my, "putmoney", this->slide_putmoney);
		this->but[9] = new button(600, 265, 200, 30, "../client/VisSource/button.bmp", "put money", this->font, (void *)game_button_click_func, tmp);
		this->but[9]->status = false;
	}
}

int game::action(SDL_Event* event){
	
	if(this->slide_raise != NULL){
		if(this->slide_raise->clicking(event)){
			this->slide_raise->drow(this->screen, this->my->player[this->my->num].cash);
			SDL_Flip(this->screen);
		}
	}

	if(this->slide_getmoney != NULL){
		if(this->slide_getmoney->clicking(event)){
			this->slide_getmoney->drow(this->screen, this->my->player[this->my->num].cash);
			SDL_Flip(this->screen);
		}
	}

	if(this->slide_putmoney != NULL){
		if(this->slide_putmoney->clicking(event)){
			this->slide_putmoney->drow(this->screen, MAX_PUT_MONEY);
			SDL_Flip(this->screen);
		}
	}
	
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


game::~game(){
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
		if(this->slide_raise){
			delete(this->slide_raise);
			this->slide_raise = NULL;
		}
		if(this->slide_getmoney){
			delete(this->slide_getmoney);
			this->slide_getmoney = NULL;
		}
		if(this->slide_putmoney){
			delete(this->slide_putmoney);
			this->slide_getmoney = NULL;
		}
		for(int i = 0; i < 54; i++){
			SDL_FreeSurface(this->card_texture[i]);
		}
	}
}


void* game_get_info_from_server(void* arguments){
	client_t* my = ((struct game_get_info_from_server_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct game_get_info_from_server_argument_t*)arguments)->mut_exit;
	pthread_mutex_t* mut_sdl = ((struct game_get_info_from_server_argument_t*)arguments)->mut_sdl;
	class game* scene = ((struct game_get_info_from_server_argument_t*)arguments)->scene;
	free((struct game_get_info_from_server_argument_t*)arguments);


	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}

	int bytes_read;

	struct recivesock rec;
	while(1){
whil:

		bytes_read = recv(my->sock, &rec, sizeof(rec), MSG_WAITALL);
		log(my->fd, "bytes_read: %d\n", bytes_read);
		if(bytes_read == 0){
			log(my->fd, "Lost conection whith server\n");
			my->status = EXIT;
			pthread_mutex_unlock(mut_exit);
			return NULL;
		}

		if (bytes_read != -1){
			log(my->fd, "recive rec_code: %d %zu\n", rec.code, rec.id);
			if(rec.id != 0){
				my->id = rec.id;
			}
			if(rec.code < 1024){
				if (my->change_status(&rec) != 0){
					pthread_mutex_unlock(mut_exit);
					return NULL;
				}
			}else if(rec.code == FROM_TO_TABLE){
				my->rewrite(&(rec.inf));
				my->show_all_inf();

				if(my->argument['t']){
					my->show_all_inf_to_console();
				}

				if(!my->argument['T']){
					if(!my->cheak_pok_status(STATUS_ACTIVE)){
						scene->but[5]->status = false;
						scene->but[6]->status = false;
						scene->but[7]->status = false;
						scene->slide_raise->status = false;
					}else{
						scene->but[5]->status = true;
						scene->but[6]->status = true;
						scene->but[7]->status = true;
						scene->slide_raise->status = true;	
					}

					if(my->cheak_pok_status(STATUS_AFTER_GAME) || my->cheak_pok_status(STATUS_WINER)){
						scene->but[4]->status = true;
						scene->but[8]->status = true;
						scene->but[9]->status = true;
						scene->slide_getmoney->status = true;
						scene->slide_putmoney->status = true;
					}else{
						scene->but[4]->status = false;
						scene->but[8]->status = false;
						scene->but[9]->status = false;
						scene->slide_getmoney->status = false;
						scene->slide_putmoney->status = false;
					}
					pthread_mutex_lock(mut_sdl);
					scene->show();
					pthread_mutex_unlock(mut_sdl);
				}

			}else if(rec.code == SERVER_DISCONNECT_YOU){
				sprintf(rbuf.buf, "disconnect");
				rbuf.id = my->id;
				send(my->sock, &rbuf, sizeof(rbuf), 0);
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
	"refresh",
	"disconnect",
	"new",
	"call",
	"raise",
	"getmoney",
	"putmoney",
	"fold",
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


void* game_scanf(void* arguments){
	client_t* my = ((struct game_scanf_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct game_scanf_argument_t*)arguments)->mut_exit;
	free((struct game_scanf_argument_t*)arguments);

	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}

	char * buf = NULL;

	while(1){
		if(buf != NULL){
			free(buf);
			buf = NULL;
		}
		rl_attempted_completion_function = completion;
		buf = readline("->game->");
		if(buf == NULL){
			goto out;
		}
		add_history(buf);
		optind = 1;

		snprintf(rbuf.buf, 256, "%s", buf);
		log(my->fd, "send id: %zu\n", my->id);
		if(!strncmp(rbuf.buf, "exit", 4)){
			my->status = EXIT;
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
			goto out;
		} else if(!strncmp(rbuf.buf, "help", 4)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
			goto out;
		} else if(!strncmp(rbuf.buf, "menu", 4)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
			goto out;
		} else if(!strncmp(rbuf.buf, "refresh", 7)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		} else if(!strncmp(rbuf.buf, "disconnect", 11)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
			goto out;
		} else if(!strncmp(rbuf.buf, "new", 3)){
			if(my->cheak_pok_status(STATUS_AFTER_GAME) || my->cheak_pok_status(STATUS_WINER)){
				rbuf.id = my->id;
				send(my->sock, &rbuf, sizeof(rbuf), 0);
			}
		} else if(!strncmp(rbuf.buf, "call", 4)){
			if(my->cheak_pok_status(STATUS_ACTIVE)){
				rbuf.id = my->id;
				send(my->sock, &rbuf, sizeof(rbuf), 0);
			}
		} else if(!strncmp(rbuf.buf, "raise", 5)){
			double x = 0;
			sscanf(rbuf.buf + 5, "%lf", &x);
			if(x > 0){
				if(my->cheak_pok_status(STATUS_ACTIVE)){
					rbuf.id = my->id;
					send(my->sock, &rbuf, sizeof(rbuf), 0);
				}
			}
		} else if(!strncmp(rbuf.buf, "fold", 4)){
			if(my->cheak_pok_status(STATUS_ACTIVE)){
				rbuf.id = my->id;
				send(my->sock, &rbuf, sizeof(rbuf), 0);
			}
		} else if(!strncmp(rbuf.buf, "getmoney", 8)){
			double x = 0;
			sscanf(rbuf.buf + 8, "%lf", &x);
			if(x > 0){
				if(my->cheak_pok_status(STATUS_AFTER_GAME) || my->cheak_pok_status(STATUS_WINER)){
					rbuf.id = my->id;
					send(my->sock, &rbuf, sizeof(rbuf), 0);
				}
			}
		} else if(!strncmp(rbuf.buf, "putmoney", 8)){
			double x = 0;
			sscanf(rbuf.buf + 8, "%lf", &x);
			if(x > 0){
				if(my->cheak_pok_status(STATUS_AFTER_GAME) || my->cheak_pok_status(STATUS_WINER)){
					rbuf.id = my->id;
					send(my->sock, &rbuf, sizeof(rbuf), 0);
				}
			}
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


void* game_scene(void* arguments){
	client_t* my = ((struct game_scene_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct game_scene_argument_t*)arguments)->mut_exit;
	pthread_mutex_t* mut_sdl = ((struct game_scene_argument_t*)arguments)->mut_sdl;
	class game* scene = ((struct game_scene_argument_t*)arguments)->scene;
	free((struct game_scene_argument_t*)arguments);

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
		pthread_mutex_lock(mut_sdl);
		scene->action(&event);
		pthread_mutex_unlock(mut_sdl);
	}
	pthread_mutex_unlock(mut_exit);
	return NULL;
}
