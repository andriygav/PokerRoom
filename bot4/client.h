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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "negotiator.h"
#include "NeyronBot.h"
#define BOT_INPUT_NEYRON 15
#define START_CASH 20000.0

#define EXIT -1
#define FIRST_STATUS 100
#define MENU 1
#define HELP 2
#define GAME 3
#define FROM_TO_TABLE 1025

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
#define ACTION_ALL_IN 10007
#define ACTION_NEW_GAME 10008


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
	int num;
	double my_money;
	int rewrite(struct sendinf*);
	int show_all_inf();
	int cheak_pok_status(int);
	int final_table[6];
	double init_cash[6];

	class bot* boting;
	
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
	for(int i = 0; i < 6; i++){
		this->final_table[i] = newinf->final_table[i];
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
	
	/*this->boting = (class bot*)malloc(sizeof(class bot));
	new(this->boting)(class bot)(BOT_INPUT_NEYRON);*/
	this->boting = new (class bot)(BOT_INPUT_NEYRON);

	this->fd = fildis;
	for(int i = 0; i < 256; i++){
		this->argument[i] = argument[i];
	}
	this->mycard[0] = 0;
	this->mycard[1] = 0;
	this->status = status;
	this->sock = sock;
	this->num = 0;
	this->id = 0;
	this->my_money = 0.0;
	
	for(int i = 0; i < 6; i++){
		init_cash[i] = 0;
	}

	this->table.blind = 0;
	this->table.num = 0;
	this->table.bet = 0;
	this->table.bank = 0;
	
	for(int i= 0; i < 5; i++)
		this->table.card[i] = 0;
	for(int i = 0; i < 6; i++){
		this->final_table[i] = 0;
	}

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

int client::change_status(struct recivesock* rec) {
  if (rec->code == EXIT) {
    this->status = rec->code;
    return EXIT;
  }
  if (rec->code == FIRST_STATUS) {
    this->status = rec->code;
    return FIRST_STATUS;
  }
  if (rec->code == MENU) {
    this->status = rec->code;
    return MENU;
  }
  if (rec->code == HELP) {
    this->status = rec->code;
    return HELP;
  }
  if (rec->code == GAME) {
    this->status = rec->code;
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


int client::first_state() {
  log(this->fd, "first status\n");
  if (this->argument['t']) printf("first status\n");

  int bytes_read = 0;
  char rbuf[256];
  for (size_t i = 0; i < sizeof(rbuf); i++) {
    rbuf[i] = 0;
  }
  struct recivesock rec;
  while (1) {
    bytes_read = recv(sock, &rec, sizeof(rec), MSG_DONTWAIT);
    if (bytes_read == 0) {
      log(this->fd, "Lost conection whith server\n");
      this->status = EXIT;
      return EXIT;
    }
    if (bytes_read != -1) {
    	if(rec.id != 0){
    		this->id = rec.id;
    		if (rec.code < 1024) {
	      	int ret = this->change_status(&rec);
	      	if(ret != 0){
	      		return ret;
	      	}
	      }
    	}
    }
    if (myscanf(rbuf)) {
      if (!strncmp(rbuf, "exit", 4)) {
        this->status = EXIT;
        return EXIT;
      }
    }
  }
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
  int bytes_read = 0;

  while(1){
 whil:
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
      }else{
      	goto whil;
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
	strncpy(rbuf.login, "bot4", 4);
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
	strncpy(rbuf.login, "bot4", 4);
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
  int bytes_read = 0;
  while(1){
whil:
    bytes_read = recv(my->sock, &rec, sizeof(rec), 0);
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
      }else{
      	goto whil;
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

	//struct help_get_info_from_server_argument_t* argv;
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

static double count_action(int num, client* my){
	if(my->player[num].last_action == ACTION_FOLD)
		return -0.45;
	if(my->player[num].last_action == ACTION_CALL || my->player[num].last_action == ACTION_ALL_IN)
		return 0;
	if(my->player[num].last_action == ACTION_RAISE){
		return 0.1 + (my->player[num].bet - my->player[(num+5)%6].bet)/my->table.bank;
	}
	return 0;
}

static int set_arr(double* arr, client* my, int num){

	arr[0] = 0;	
	int cards[7] = {-1,-1,-1,-1,-1,-1,-1};
	for (int i = 0;i < 5; i++)
		cards[i+2] = my->table.card[i];
	cards[0] = my->mycard[0];
	cards[1] = my->mycard[1];
	arr[1] = counter(cards, num);
	arr[2] = my->player[my->num].cash/START_CASH;
	arr[3] = (my->player[my->num].bet+1)/(my->table.bet + 1);

	for(int i = 0; i < 6; i++){
		if(i != my->num){
			arr[4 + i*2] = (my->player[i].bet + 1)/(my->table.bet + 1);
			arr[5 + i*2] = (my->player[i].cash + 1)/START_CASH;
		}
	}

	arr[14] = (my->table.bank + 1)/START_CASH;

	return 0;
}

static double phi(double x){
	return ((x+1.0)/2.0);
}

static double phi_in(double x){
	return (x*2.0 - 1.0);
}

static double tah(double x){
	return ((exp(x) - exp(-x))/(exp(x) + exp(-x)));
}

int OP(int sem, int num, int opt){
	struct sembuf sops;
	sops.sem_num = (unsigned short)num;
	sops.sem_op = (short)opt;
	sops.sem_flg = 0;
	int ret = semop(sem, &sops, 1);
	return ret;
}

#ifdef __linux__ 
    union semun {
	int val;                  /* значение для SETVAL */
	struct semid_ds *buf;     /* буферы для  IPC_STAT, IPC_SET */
	unsigned short *array;    /* массивы для GETALL, SETALL */
		           /* часть, особенная для Linux: */
	struct seminfo *__buf;    /* буфер для IPC_INFO */
};
#endif

FILE* fres = fopen("./Bot4/fres.txt","a+");
FILE* ffit = fopen("./Bot4/fres.txt","r");
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

	int card_1 = -1;
	int card_2 = -1;

	double getMon = 0.0, takeMon = 0.0;
	bool get_money = false, take_money = false;	
	int bytes_read = 0;

	while(1){
whil:
	    bytes_read = recv(my->sock, &rec, sizeof(rec), 0);
	    if(bytes_read == 0){
			log(my->fd, "Lost conection whith server\n");
			my->status = EXIT;
			pthread_mutex_unlock(mut_exit);
			return NULL;
	    }

		if (bytes_read != -1){
      		if(rec.id != 0){
        		my->id = rec.id;
      		}else{
      			goto whil;
      		}
			if(rec.code < 1024){
		        if(my->change_status(&rec) != 0){
			        pthread_mutex_unlock(mut_exit);
			        return NULL;
		        }
			}else if(rec.code == FROM_TO_TABLE){
				my->rewrite(&(rec.inf));
				if(my->table.card[0] < 0){
					for(int i = 0; i < 6; i++)
						my->init_cash[i] = (my->player[i].cash + my->player[i].bet > my->init_cash[i])?(my->player[i].cash + my->player[i].bet):my->init_cash[i];
				}
				my->show_all_inf();
				if(my->cheak_pok_status(STATUS_ACTIVE) && flag != my->id){
					int num_game = 1;
					if(my->table.card[0] >= 0)
						num_game = 2;
					if(my->table.card[3] >= 0)
						num_game = 3;
					if(my->table.card[4] >= 0)
						num_game = 4;

					double arr[BOT_INPUT_NEYRON];
					for (int i = 0; i < BOT_INPUT_NEYRON; i++)
						arr[i] = 0.0;
					set_arr(arr, my, num_game);
					double res = my->boting->action(num_game, arr);
					res = phi(res);
					if(res > 1)
						res = 1;
					else if (res < 0)
						res = 0;
					
					my->boting->log(arr, my->num, res, num_game);
					log(my->fd, "#res = %lf\n", res);

					if(my->table.bet > res * (my->player[my->num].cash) + my->player[my->num].bet){
						sprintf(rbuf.buf, "fold");
						card_1 = my->mycard[0];
						card_2 = my->mycard[1];
					}else if(res * (0.75)*my->player[my->num].cash < my->table.bet - my->player[my->num].bet || (my->player[my->num].cash < my->table.blind)){
						sprintf(rbuf.buf, "call");
					}else{
						sprintf(rbuf.buf, "raise %lf", ((int)((res*(0.9)*(my->player[my->num].cash) - (my->table.bet - my->player[my->num].bet)) / my->table.blind + 1)) * my->table.blind);
					}

					rbuf.id = my->id;
					send(my->sock, &rbuf, sizeof(rbuf), 0);
					flag = my->id;
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
					log(my->fd, "my_money %d -> %lf\n",my->num, my->player[my->num].cash + my->my_money - START_CASH);
				}

				if (((my->cheak_pok_status(STATUS_WINER) || my->cheak_pok_status(STATUS_AFTER_GAME)) && ccount < 1000) && flag1 != my->id && !get_money && !take_money){
					ccount++;
					
					for(size_t i = 0; i < my->boting->count; i++){
						if(my->final_table[my->num] == 1){
							my->boting->ans[i] = pow(my->boting->ans[i], 9.0/10.0);
						}else if(my->final_table[my->num] == 2){
							my->boting->ans[i] = tah(my->boting->ans[i]);
						}else if(my->final_table[my->num] == -1 && i == my->boting->count -1){	
							my->boting->ans[i] = my->boting->ans[i]*(1.0 + tan((1.0/10.0)*(my->boting->arr[i][1] - 0.2)));
												
						}else{
							my->boting->ans[i] = log(1 + my->boting->ans[i]);
						}
						my->boting->ans[i] = phi_in(my->boting->ans[i]);
					}
					int cou = 0;
					for(int ti = 0; ti < 6; ti++){
						if(my->final_table[ti] > 0)
							cou++;
					}


					/*int fd = open("Bot4/semFit", O_CREAT|O_EXCL, 0666);

					if(fd >= 0){
						close(fd);
					}

					int sem = semget(ftok("Bot4/semFit", 0), 1, IPC_CREAT|IPC_EXCL|0666);

					if(sem >= 0){
						union semun arg;
						arg.val = 1;
						semctl(sem, 0, SETVAL, arg);
					}else{
						sem = semget(ftok("Bot4/semFit", 0), 1, 0);
					}

					if(sem < 0){
						log(my->fd, "semaphore: %s\n", strerror(errno));
					}else{
						OP(sem, 0, -1);

						if(cou > 1 || my->final_table[my->num] == -1){
							for (int i = 0; i < my->boting->count; i++){
								fprintf(fres, "%zu ", my->boting->act[i]);
								for (int j = 0; j < BOT_INPUT_NEYRON; j++){
									fprintf(fres, "%lf ", my->boting->arr[i][j]);
								}
								fprintf(fres, "%lf %d %lf\n", my->boting->ans[i], my->final_table[my->num], phi_in(my->boting->myans[i]));
							}
						}
						fflush(fres);
						if (my->argument['f']){
							my->boting->load();
							size_t t_num;
							double t_arr[BOT_INPUT_NEYRON];
							double t_ans;
							double t_myans;
							int t_res;
							rewind(ffit);
							while(fscanf(ffit, "%zu ", &t_num) != EOF){
								for (int j = 0; j < BOT_INPUT_NEYRON; j++){
									fscanf(ffit, "%lf ", &t_arr[j]);
								}
								fscanf(ffit, "%lf %d %lf\n", &t_ans, &t_res, &t_myans);
								if (!(t_res == 1 && phi(my->boting->result(t_num, t_arr)) > phi(t_ans)) && !(t_res != 1 && phi(my->boting->result(t_num, t_arr)) < phi(t_ans)))
									my->boting->learn(t_num, t_arr, t_ans);
							}
						}

						my->boting->save();
						OP(sem, 0, 1);
					}*/


					my->boting->reset_log();
					my->boting->LastAns = 0;

					card_1 = -1;
					card_2 = -1;				

					sprintf(rbuf.buf, "new");
					rbuf.id = my->id;
					send(my->sock, &rbuf, sizeof(rbuf), 0);
					flag1 = my->id;
				}
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
