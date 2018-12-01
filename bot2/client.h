#pragma once
#include <sys/poll.h>
#include <errno.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "play_info.h"
#include <math.h>
#define EXIT -1
#define FIRST_STATUS 0
#define MENU 1
#define HELP 2
#define GAME 3
#define FROM_TO_TABLE 1025

#define START_CASH 20000.0

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


#define ACTION_CALL 10004
#define ACTION_RAISE 10005
#define ACTION_FOLD 10006
#define N 1000
#define CN 10
#define SB 1.5
char* ftoa(float n, int afterpoint);
double test(int* card, int* circ);
void create_c(int* testcards, struct card* a);
int prefl(int card1, int card2);
void nul_colum(struct card* colum);
int set_colum(int* card);
int is_col_right(int* a);
#include "count_comb.c"
#include "negotiator.cpp"



//int exists_in(int* arr, size_t size, int value);


class client{
public:
	bool argument[256];
	size_t id;
	int status;
	int sock;
	int fd;
	client(int status, int sock, bool* argument, int fd);

	int first_state();
	int change_status(struct recivesock*);

//Menu object
	int menu();
//Help object
	char help_text[1024];
	int help();
//Game object
	int game();
	struct publplayinfo player[6];
	struct publtableinfo table;
	int mycard[2];
	double cur_bet;
	double old_bet;
	int circ[4];
	int num;
	double my_money;
	int rewrite(struct sendinf*);
	int show_all_inf();
	int cheak_pok_status(int);
	int negotiator();
	void print_log();
	int tester(int* );
	
};

int client::cheak_pok_status(int st){
	if(this->player[this->num].status == st)
		return 1;
	return 0;
}

int client::show_all_inf(){
	//printf("\n--------------------------------------------------\n");
	log(this->fd, "\n-----------------------TABLE----------------------\n");
	log(this->fd, "bank = %f, curen bet = %f, card = %d %d %d %d %d\n", this->table.bank, this->table.bet, this->table.card[0], this->table.card[1], this->table.card[2], this->table.card[3], this->table.card[4]);

	log(this->fd, "\n-----------------------MYCARDS--------------------\n");
	log(this->fd, "status = %d; %d %d; cash = %lf, bet = %lf \n", this->player[this->num].status, this->mycard[0], this->mycard[1], this->player[this->num].cash, this->player[this->num].bet);
	//printf("\n--------------------------------------------------\n");
	return 0;
}



void client::print_log(){
	char str[2] = {0, 0};
	log(this->fd, "%s\n", str);
	conv(str, this->mycard[0]);
	log(this->fd, "%s\n", str);
	conv(str, this->mycard[1]);
	log(this->fd, "%s\n", str);
	for(int i = 0; i < 5;i++){
		conv(str, this->table.card[i]);
		log(this->fd, "%s ", str);
	}
}

int client::rewrite(struct sendinf * newinf){
//copy card
	this->mycard[0] = newinf->card[0]-1;
	this->mycard[1] = newinf->card[1]-1;
	this->num = newinf->num;
//copy table	
	this->table.bank = newinf->table.bank;
	this->table.bet = newinf->table.bet;
	this->table.blind = newinf->table.blind;
	this->table.num = newinf->table.num;
	for(int i = 0; i < 5; i++){
		this->table.card[i] = newinf->table.card[i]-1;
	}
//copy all players
	for(int i = 0 ; i < 6; i++){
		this->player[i].cash = newinf->player[i].cash;
		this->player[i].last_action = newinf->player[i].last_action;
		this->player[i].bet = newinf->player[i].bet;
		this->player[i].status = newinf->player[i].status;
		this->player[i].blind = newinf->player[i].blind;
		this->player[i].comb = newinf->player[i].comb;
		this->player[i].card[0] = newinf->player[i].card[0]-1;
		this->player[i].card[1] = newinf->player[i].card[1]-1;		
		strncpy(this->player[i].login, newinf->player[i].login, sizeof(newinf->player[i].login));
	}
	return 0;
}

client::client(int status, int sock, bool* argument, int fildis){
	this->fd = fildis;
	for(int i = 0; i < 256; i++){
		this->argument[i] = argument[i];
	}
	this->mycard[0] = 0;
	this->mycard[1] = 0;
	this->status = status;
	this->sock = sock;
	this->cur_bet = 0.0;
	this->old_bet = 0.0;
	this->num = 0;
	this->id = 0;
	this->my_money = 0.0;

	this->table.blind = 0;
	this->table.num = 0;
	this->table.bet = 0;
	this->table.bank = 0;
	
	for(int i= 0; i < 5; i++)
		this->table.card[i] = 0;

	for(int i= 0; i < 4; i++)
		this->circ[i] = 0;

	for(int i = 0 ; i < 6; i++){
		this->player[i].cash = 0;
		this->player[i].last_action = 0;
		this->player[i].bet = 0;
		this->player[i].status = -1;
		this->player[i].blind = 0;
		this->player[i].card[0] = -1;
		this->player[i].card[1] = -1;
		this->player[i].comb = 0;
		for(size_t j = 0; j < sizeof(this->player[i].login); j++){
			this->player[i].login[j] = 0;
		}
	}
}

int client::change_status(struct recivesock* rec){
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


int client::first_state(){
	log(this->fd, "first status\n");
	if(this->argument['t'])
		printf("first status\n");

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


//MENU START------------------------------------------------------------------------------------------------------------
struct menu_scanf_argument_t{
	client* my;
	pthread_mutex_t* mut_exit;
};

struct menu_get_info_from_server_argument_t{
	client* my;
	pthread_mutex_t* mut_exit;
};

void* menu_get_info_from_server(void* arguments){
	client* my = ((struct menu_get_info_from_server_argument_t*)arguments)->my;
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
	client* my = ((struct menu_scanf_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct menu_scanf_argument_t*)arguments)->mut_exit;
	free((struct menu_scanf_argument_t*)arguments);

	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	for(size_t i = 0; i < 256; i++){
		rbuf.login[i] = 0;
	}
	strncpy(rbuf.login, "bot2", 256);
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
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		} else if(!strncmp(rbuf.buf, "help", 4)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		}
	}
	pthread_mutex_unlock(mut_exit);
	return NULL;
}

void* menu_start_game(void* arguments){
	client* my = ((struct menu_scanf_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct menu_scanf_argument_t*)arguments)->mut_exit;
	free((struct menu_scanf_argument_t*)arguments);

	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	for(size_t i = 0; i < 256; i++){
		rbuf.login[i] = 0;
	}
	strncpy(rbuf.login, "bot2", 256);
	char c;
	for(int i = 0; i < 10; i++){
		sleep(1);
		sprintf(rbuf.buf,"game");
		rbuf.id = my->id;
		send(my->sock, &rbuf, sizeof(rbuf), 0);
	}
	pthread_mutex_unlock(mut_exit);
	return NULL;
}

int client::menu(){
	log(this->fd, "menu\n");
	if(this->argument['t'])
		printf("menu\n");

	int bytes_read = 0;
	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	struct recivesock rec;

	pthread_mutex_t* mut_exit = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if(pthread_mutex_init(mut_exit, NULL)){
		log(this->fd, "menu mut_exit - init", strerror(errno));
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

	pthread_t menu_start_game_thread = 0;
	if(this->argument['g']){		
		argv = (struct menu_scanf_argument_t*)malloc(sizeof(struct menu_scanf_argument_t));
		((struct menu_scanf_argument_t*)argv)->my = this;
		((struct menu_scanf_argument_t*)argv)->mut_exit = mut_exit;
		pthread_create(&menu_start_game_thread, NULL, menu_start_game, argv);
	}
	
	pthread_mutex_lock(mut_exit);
	pthread_cancel(menu_get_info_from_server_thread);

	if(this->argument['t'])
		pthread_cancel(menu_scanf_thread);
	if (this->argument['g'])
		pthread_cancel(menu_start_game_thread);

	pthread_join(menu_get_info_from_server_thread, NULL);

	if(this->argument['t'])
		pthread_join(menu_scanf_thread, NULL);
	if(this->argument['g'])
		pthread_join(menu_start_game_thread, NULL);
	
	pthread_mutex_destroy(mut_exit);
	free(mut_exit);
	return 0;
}
//MENU END------------------------------------------------------------------------------------------------------------

//HELP start--------------------------------------------------------

struct help_get_info_from_server_argument_t{
	client* my;
	pthread_mutex_t* mut_exit;
};

struct help_scanf_argument_t{
	client* my;
	pthread_mutex_t* mut_exit;
};
void* help_get_info_from_server(void* arguments){
	client* my = ((struct help_get_info_from_server_argument_t*)arguments)->my;
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



void* help_scanf(void* arguments){
	client* my = ((struct help_scanf_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct help_scanf_argument_t*)arguments)->mut_exit;
	free((struct help_scanf_argument_t*)arguments);

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

int client::help(){
	log(this->fd, "help\n");
	if(this->argument['t'])
		printf("help\n");

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
	
	pthread_mutex_lock(&mut_exit);
	pthread_cancel(help_get_info_from_server_thread);
	if(this->argument['t'])
		pthread_cancel(help_scanf_thread);

	pthread_join(help_get_info_from_server_thread, NULL);
	if(this->argument['t'])
		pthread_join(help_scanf_thread, NULL);

	pthread_mutex_destroy(&mut_exit);
	return 0;
}

//HELP end--------------------------------------------------

//Game start-------------------------------------

struct game_scanf_argument_t{
	client* my;
	pthread_mutex_t* mut_exit;
};

struct game_get_info_from_server_argument_t{
	client* my;
	pthread_mutex_t* mut_exit;
};

void* game_get_info_from_server(void* arguments){
	client* my = ((struct game_get_info_from_server_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct game_get_info_from_server_argument_t*)arguments)->mut_exit;
	free((struct game_get_info_from_server_argument_t*)arguments);

	struct recivesock rec;
	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}
	for(size_t i = 0; i < 256; i++){
		rbuf.login[i] = 0;
	}
	rbuf.id = 0;

	int was_decl = 0;
	int flag = 0, flag1 = 0;
	int ccount = 0; 
	double getMon = 0.0, takeMon = 0.0;
	bool get_money = false, take_money = false;	
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
		}else if(rec.code == FROM_TO_TABLE){
			my->rewrite(&(rec.inf));
			my->show_all_inf();

			if (my->cheak_pok_status(STATUS_ACTIVE) && flag != my->id){
				char str[2];

				int act = my->negotiator();
				if (act == ACTION_CALL){
					log(my->fd, "%d call\n", my->num);
					sprintf(rbuf.buf, "call");
				}
				if (act == ACTION_FOLD){
					log(my->fd, "%d fold\n", my->num);
					sprintf(rbuf.buf, "fold");
				}
				if (act == ACTION_RAISE){
					char str[256];
					log(my->fd, "%d raise %lf\n", my->num, my->player[my->num].bet - my->old_bet);
					sprintf(rbuf.buf, "raise %f", my->player[my->num].bet - my->old_bet);
				}
				my->cur_bet += my->player[my->num].bet - my->old_bet;

				rbuf.id = my->id;
				send(my->sock, &rbuf, sizeof(rbuf), 0);
				flag = my->id;
				my->print_log();
			}

			if ((my->cheak_pok_status(STATUS_WINER) || my->cheak_pok_status(STATUS_AFTER_GAME)) && get_money && !take_money){
				if (getMon != 0 && abs(getMon - my->player[my->num].cash) > my->table.blind){
					my->my_money += getMon - my->player[my->num].cash;
					log(my->fd, "#MONEY %d -> %lf\n",my->num, my->my_money);
					getMon = 0.0;
					get_money = false;
				}
			}

			if ((my->cheak_pok_status(STATUS_WINER) || my->cheak_pok_status(STATUS_AFTER_GAME)) && !get_money && !take_money){
				//printf("%s %s\n", get_money? "get_true":"get_false",  take_money?"take_true":"take_false");
				if (my->player[my->num].cash > 2*START_CASH){
					getMon = my->player[my->num].cash;
					sprintf(rbuf.buf, "getmoney %lf", my->player[my->num].cash - (START_CASH + 1));
					rbuf.id = my->id;
					send(my->sock, &rbuf, sizeof(rbuf), 0);
					get_money = true;
				}
			}

			if ((my->cheak_pok_status(STATUS_WINER) || my->cheak_pok_status(STATUS_AFTER_GAME)) && take_money && !get_money){
				if (takeMon < 2*my->table.blind && abs(takeMon - my->player[my->num].cash) > my->table.blind){
					my->my_money += takeMon - my->player[my->num].cash;
					log(my->fd, "#MONEY %d -> %lf\n",my->num, my->my_money);
					takeMon = 0.0;
					take_money = false;
				}
			}

			if ((my->cheak_pok_status(STATUS_WINER) || my->cheak_pok_status(STATUS_AFTER_GAME)) && !take_money && !get_money){
				//printf("%s %s\n", get_money? "get_true":"get_false",  take_money?"take_true":"take_false");
				if (my->player[my->num].cash < 2*my->table.blind){
					takeMon = my->player[my->num].cash;
					sprintf(rbuf.buf, "putmoney %lf", START_CASH);
					rbuf.id = my->id;
					send(my->sock, &rbuf, sizeof(rbuf), 0);
					take_money = true;
				}
			}
			if ((my->cheak_pok_status(STATUS_WINER) || my->cheak_pok_status(STATUS_AFTER_GAME)) && (ccount % 100 == 0)){
				log(my->fd, "my_money -> %lf\n", my->player[my->num].cash + my->my_money - START_CASH);
			}

			if (((my->cheak_pok_status(STATUS_WINER) || my->cheak_pok_status(STATUS_AFTER_GAME)) && ccount < 10000) && flag1 != my->id && !get_money && !take_money){
				ccount++;
				sprintf(rbuf.buf, "new");
				rbuf.id = my->id;
				send(my->sock, &rbuf, sizeof(rbuf), 0);
				flag1 = my->id;
				for(int i = 0; i < 4; i++)
					my->circ[i] = 0;
			}

		}


	}
	pthread_mutex_unlock(mut_exit);
	return NULL;
}


void* game_scanf(void* arguments){
	client* my = ((struct game_scanf_argument_t*)arguments)->my;
	pthread_mutex_t* mut_exit = ((struct game_scanf_argument_t*)arguments)->mut_exit;
	free((struct game_scanf_argument_t*)arguments);

	struct recive_t rbuf;
	for(size_t i = 0; i < 256; i++){
		rbuf.buf[i] = 0;
	}

	while(1){
		printf("->%c", '\0');
		scanf("%[^\n]s", rbuf.buf);
		char c = 0;
		scanf("%c", &c);
		if(!strncmp(rbuf.buf, "exit", 4)){
			my->status = EXIT;
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		} else if(!strncmp(rbuf.buf, "help", 4)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		} else if(!strncmp(rbuf.buf, "menu", 4)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		} else if(!strncmp(rbuf.buf, "refresh", 7)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
		} else if(!strncmp(rbuf.buf, "disconnect", 11)){
			rbuf.id = my->id;
			send(my->sock, &rbuf, sizeof(rbuf), 0);
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
		}
	}
	pthread_mutex_unlock(mut_exit);
	return NULL;
}



int client::game(){
	log(this->fd, "game\n");
	if(this->argument['t'])
		printf("game");

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

	void* argv;
	
	pthread_t game_get_info_from_server_thread = 0;
	argv = (struct game_get_info_from_server_argument_t*)malloc(sizeof(struct game_get_info_from_server_argument_t));
	((struct game_get_info_from_server_argument_t*)argv)->my = this;
	((struct game_get_info_from_server_argument_t*)argv)->mut_exit = &mut_exit;
	pthread_create(&game_get_info_from_server_thread, NULL, game_get_info_from_server, argv);

	pthread_t game_scanf_thread = 0;
	if(this->argument['t']){
		argv = (struct game_scanf_argument_t*)malloc(sizeof(struct game_scanf_argument_t));
		((struct game_scanf_argument_t*)argv)->my = this;
		((struct game_scanf_argument_t*)argv)->mut_exit = &mut_exit;
		pthread_create(&game_scanf_thread, NULL, game_scanf, argv);
	}	
	pthread_mutex_lock(&mut_exit);

	pthread_cancel(game_get_info_from_server_thread);
	
	if(this->argument['t'])
		pthread_cancel(game_scanf_thread);

	pthread_join(game_get_info_from_server_thread, NULL);

	if(this->argument['t'])
		pthread_join(game_scanf_thread, NULL);

	pthread_mutex_destroy(&mut_exit);
	return 0;
}

int client::negotiator(){

	int allcards[7];
	int preflop = 0;
	allcards[0] = this->mycard[0];
	allcards[1] = this->mycard[1];
	log(this->fd, "%d %d ", allcards[0], allcards[1]);
	for(int i = 2; i < 7; i++){
		allcards[i] = this->table.card[i - 2];
		log(this->fd, "%d ", allcards[i]);
	}
	if (allcards[2] <= -1){
		this->circ[0] = 0; 
		this->cur_bet = 0;
	}
	else if (allcards[5] <= -1)
		this->circ[0] = 1;
	else if (allcards[6] <= -1)
		this->circ[0] = 2;
	else this->circ[0] = 3;
	log(this->fd, "\n");

	struct timespec tmp;
	clock_gettime(CLOCK_REALTIME, &tmp);
	srand((unsigned int)tmp.tv_nsec);

	return tester(allcards);
}

int is_col_right(int* a){
	for (int i = 0; i < 7; i++)
		for(int j = 6; j > i; j--)
			if (a[i] == a[j])
				return 0;
	return 1;
}

int client::tester(int* allcards){
	int act = -1;
	//printf("her1e\n");
	if (this->circ[0] == 0)
		act = prefl(allcards[0], allcards[1]);
	if (this->circ[0] != 0){
		act = counter(allcards, this->circ);
	}

	double koef = 0;
	
	if (act > 0){
		log(this->fd, "act - %d\n", act);
		koef = (act/50.0)*0.01;
		log(this->fd, "koef - %lf\n", koef);
		act = 2;
	}
	if (act == -2)
		act = 0;
	if (act == -1)
		act = 1;

	//printf("her2e %d\n", act);

	double bet_cur = this->table.bet + this->cur_bet;
	
	this->old_bet = this->player[this->num].bet;

	if (this->circ[this->circ[0]] == 0){
		this->old_bet = 0.0;
	}
	this->circ[this->circ[0]] += 1;

	if (act == 0)
		return ACTION_FOLD;
	if (act == 1){
		if ((this->table.bet - this->player[this->num].bet < this->player[this->num].cash)){
			return ACTION_CALL;
		}
		else ACTION_FOLD;
	}
	if (act == 2){
		int vseRavnoStavit = this->table.bet - this->player[this->num].bet;
		int esheMozhno = (this->player[this->num].cash) - vseRavnoStavit;

		//printf("last_action %d, old_bet - %lf, table_blind %lf\n", this->player[this->num].last_action, this->old_bet, this->table.blind);
		if (this->player[this->num].cash*koef < esheMozhno)
			this->player[this->num].bet += this->player[this->num].cash*koef;
		else if (this->player[this->num].cash < (this->table.bet + this->player[this->num].cash*koef - this->player[this->num].bet) || (this->table.bet + this->player[this->num].cash*koef - this->player[this->num].bet < this->table.blind))
			return ACTION_CALL;
		else 
			return ACTION_FOLD;
		if (this->circ[this->circ[0]] == 1)
			return ACTION_RAISE;
		return ACTION_CALL;
	}
}
