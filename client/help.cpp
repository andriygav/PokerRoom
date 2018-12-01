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
#include "help.h"

#define COUNT_OF_BUTTON 4
#define COUNT_OF_FONT 3
 
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

static void help_button_click_func(void* pointer){
	struct str_t* tmp = (str_t*)pointer;
	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	for(size_t i = 0; i < 256; i++){
		rbuf.login[i] = 0;
	}
	rbuf.id = tmp->my->id;
	sprintf(rbuf.buf, "%s", tmp->buf);
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

int help::show(){
	apply_surface(0, 0, this->back, this->screen);

	char burf[256];
	for(int i = 0; i < sizeof(burf); i++){
		burf[i] = 0;
	}
	sprintf(burf, "%d", this->curent_page);

	SDL_Color text_color = {0, 0, 0};

	SDL_Surface* text_surf = TTF_RenderText_Solid(this->font, burf, text_color);
	SDL_Rect offset;

	SDL_GetClipRect(text_surf, &offset);
	offset.x = (600 - offset.w)/2;
	offset.y = (600 - 570 - offset.h)/2 + 570;

	SDL_BlitSurface(text_surf, NULL, this->screen, &offset);

	SDL_FreeSurface(text_surf);

	for(int j = 0; j < PAGESIZE; j++){
		TTF_Font* font = this->TextFont[this->text[this->curent_page].fontline[j] % COUNT_OF_FONT];

		SDL_Surface* text_surf = TTF_RenderText_Solid(font, this->text[this->curent_page].line[j], text_color);
		SDL_Rect offset;

		SDL_GetClipRect(text_surf, &offset);
		offset.x = 0;
		offset.y = 24 * j;
		if(this->text[this->curent_page].optline[j] == 2){
			offset.x = (600 - offset.w)/2;
		}
		if(this->text[this->curent_page].optline[j] == 1){
			offset.x = 30;
		}

		SDL_BlitSurface(text_surf, NULL, this->screen, &offset);
		SDL_FreeSurface(text_surf);
	}

	for(int i = 0; i < COUNT_OF_BUTTON; i++){
		if(this->but[i])
			this->but[i]->drow(this->screen);
	};

	SDL_Flip(this->screen);
}
struct help_t{
	help* my;
};


static void help_button_next_click_func(void* pointer){
	help_t* tmp = (help_t*)pointer;
	tmp->my->curent_page++;
	tmp->my->curent_page %= tmp->my->count_of_page;
	tmp->my->show();
	return;
}
static void help_button_prev_click_func(void* pointer){
	help_t* tmp = (help_t*)pointer;
	tmp->my->curent_page += (tmp->my->count_of_page - 1);
	tmp->my->curent_page %= tmp->my->count_of_page;
	tmp->my->show();
	return;
}


static help_t* set_help_t(help* my){
	help_t* tmp = (help_t*)malloc(sizeof(help_t));
	tmp->my = my;
	return tmp;	
}

static int option_of_line(const char x){
	if(x == '#'){
		return 1;
	}
	return 0;
}

static int font_of_line(const char x){
	if(x == '&'){
		return 1;
	}
	return 0;
}

static bool IS_LITER(char x){

	if((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9') || x == '"'){
		return true;
	}

	return false;
}

int help::read_text(int fd){
	int ret = 0;
	int num_page = 0;
	int num_line = 0;
	int i = 0;
	char buff[256];
	for(int j = 0; j < 0; j++){
		buff[j] = 0;
	}

	while((ret = read(fd, buff, sizeof(buff) - 1)) > 0){
		if(ret != -1){
			buff[ret] = 0;
			for(int j = 0; j < ret; j++){
				if(option_of_line(buff[j]) == 0 && font_of_line(buff[j]) == 0){
					this->text[num_page].line[num_line][i] = buff[j];
					if(buff[j] == '\n'){
						this->text[num_page].line[num_line][i] = 0;
						i = -1;
						num_line++;
						if(num_line == PAGESIZE){
							num_line = 0;
							num_page++;
							num_page %= COUNTOFPAGE;
						}
					}
					i++;
					if(i == LINESIZE - 1){
						this->text[num_page].line[num_line][i] = 0;
						if(IS_LITER(this->text[num_page].line[num_line][i - 1])){
							for(i; i > 0; i--){
								if(!IS_LITER(this->text[num_page].line[num_line][i - 1])){
									break;
								}
							}
							int old_num_line = num_line;
							int old_num_page = num_page;
							this->text[num_page].line[num_line][i - 1] = 0;
							num_line++;								
							if(num_line == PAGESIZE){
								num_line = 0;
								num_page++;
								num_page %= COUNTOFPAGE;
							}
							for(int j = i; j < LINESIZE; j++){
								this->text[num_page].line[num_line][j - i] = this->text[old_num_page].line[old_num_line][j];
							}
							i = LINESIZE - i - 1;
						}else{
							i = 0;
							num_line++;
							if(num_line == PAGESIZE){
								num_line = 0;
								num_page++;
								num_page %= COUNTOFPAGE;
							}
						}
					}
				}else{
					if(option_of_line(buff[j]) != 0){
						this->text[num_page].optline[num_line] += option_of_line(buff[j]);
					}
					if(font_of_line(buff[j]) != 0){
						this->text[num_page].fontline[num_line] += font_of_line(buff[j]);
					}
				}
			}
		}
	}
	this->count_of_page = num_page + 1;
	return 0;
}

help::help(SDL_Surface* screen, client_t* my, const char* help_text){
	this->back = load_image("../client/VisSource/background.bmp");
	for(int i = 0; i < 256; i++)
		this->but[i] = NULL;

	for(int i = 0; i < COUNTOFPAGE; i++){
		for(int j = 0; j < PAGESIZE; j++){
			for(int t = 0; t < LINESIZE; t++){
				this->text[i].line[j][t] = 0;
				this->text[i].optline[j] = 0;
				this->text[i].fontline[j] = 0;
			}
		}
	}
	this->curent_page = 0;
	this->count_of_page = 1;
	int fd = open(help_text, O_RDONLY);

	if(fd >= 0){
		this->read_text(fd);
		close(fd);
	}

	this->my = my;
	this->screen = screen;
	struct str_t* tmp = NULL;
	this->font = TTF_OpenFont("../client/VisSource/font.ttf", 24);
	this->TextFont[0] = TTF_OpenFont("../client/VisSource/litleFont.ttf", 24);
	this->TextFont[1] = TTF_OpenFont("../client/VisSource/midleFont.ttf", 24);
	this->TextFont[2] = TTF_OpenFont("../client/VisSource/hightFont.ttf", 24);

 	tmp = set_str_t(this->my, "exit");
	this->but[0] = new button(600, 0, 200, 30, "../client/VisSource/button.bmp", "exit", this->font, (void *)help_button_click_func, tmp);
 	tmp = set_str_t(this->my, "menu");
	this->but[1] = new button(600, 35, 200, 30, "../client/VisSource/button.bmp", "menu", this->font, (void *)help_button_click_func, tmp);
	
	struct help_t* tmp2 = NULL;
	tmp2 = set_help_t(this);
	this->but[2] = new button(400, 570, 200, 30, "../client/VisSource/button.bmp", "next", this->font, (void *)help_button_next_click_func, tmp2);
	tmp2 = set_help_t(this);
	this->but[3] = new button(0, 570, 200, 30, "../client/VisSource/button.bmp", "prev", this->font, (void *)help_button_prev_click_func, tmp2);
}

int help::action(SDL_Event* event){
	for(int i = 0; i < COUNT_OF_BUTTON; i++){
		if(this->but[i] != NULL){
			if(this->but[i]->clicking(event)){
				this->but[i]->drow(this->screen);
				SDL_Flip(this->screen);
			}
		}
	}
}

help::~help(){
	this->screen = NULL;
	TTF_CloseFont(this->font);
	for(int i = 0; i < COUNT_OF_FONT; i++){
		TTF_CloseFont(this->TextFont[i]);
	}
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

void* help_get_info_from_server(void* arguments){
	client_t* my = ((struct help_get_info_from_server_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct help_get_info_from_server_argument_t*)arguments)->mut_exit;
	free((struct help_get_info_from_server_argument_t*)arguments);

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

static int print_help(){
	int fd = open("../client/source/help.help", O_RDONLY);
	if(fd >= 0){
		char buff;
		int ret = 0;
		while((ret = read(fd, &buff, sizeof(buff))) > 0){
			if(option_of_line(buff) == 0 && font_of_line(buff) == 0){
				write(1, &buff, sizeof(buff));
			}			
		}
		close(fd);
	}
	return 0;
}

void* help_scanf(void* arguments){
	client_t* my = ((struct help_scanf_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct help_scanf_argument_t*)arguments)->mut_exit;
	free((struct help_scanf_argument_t*)arguments);


	print_help();

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
		} else if(!strncmp(rbuf.buf, "menu", 4)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		}
	}
	pthread_mutex_unlock(mut_exit);
	return NULL;
}



void* help_scene(void* arguments){
	client_t* my = ((struct help_scene_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct help_scene_argument_t*)arguments)->mut_exit;
	class help* scene = ((struct help_scene_argument_t*)arguments)->scene;
	free((struct help_scene_argument_t*)arguments);

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
