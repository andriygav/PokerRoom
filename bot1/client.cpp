#include <zconf.h>
#include <math.h>
#include "client.h"
#include "client_help.h"
#include "client_menu.h"
#include "client_game.h"
#include "negotiator.cpp"

static int log(int md, const char* format, ...) {
  char str[256];
  for (int i = 0; i < 256; i++) str[i] = 0;
  va_list args;
  va_start(args, format);
  vsprintf(str, format, args);
  va_end(args);
  if (write(md, str, strnlen(str, 256)) == -1) {
    perror("write");
  }
  return 0;
}

static size_t myscanf(char* dest) {
  struct pollfd fds;
  fds.fd = 0;
  fds.events = POLLIN;
  if (poll(&fds, 1, 0) != 0) {
    std::cin.getline(dest, 256);
    return 1;
  }
  return 0;
}

int client::cheak_pok_status(int st) {
  if (this->player[this->num].status == st) return 1;
  return 0;
}

int client::show_all_inf() {
  // printf("\n--------------------------------------------------\n");
  log(this->fd, "\n-----------------------TABLE----------------------\n");
  log(this->fd, "bank = %f, curen bet = %f, card = %d %d %d %d %d\n",
      this->table.bank, this->table.bet, this->table.card[0],
      this->table.card[1], this->table.card[2], this->table.card[3],
      this->table.card[4]);

  log(this->fd, "\n-----------------------MYCARDS--------------------\n");
  log(this->fd, "status = %d; %d %d; cash = %lf, bet = %lf \n",
      this->player[this->num].status, this->mycard[0], this->mycard[1],
      this->player[this->num].cash, this->player[this->num].bet);
  // printf("\n--------------------------------------------------\n");
  return 0;
}

void client::print_log() {
  char str[2] = {0, 0};
  conv(str, this->mycard[0]);
  log(this->fd, "%s\n", str);
  conv(str, this->mycard[1]);
  log(this->fd, "%s\n", str);
  for (int i = 0; i < 5; i++) {
    conv(str, this->table.card[i]);
    log(this->fd, "%s ", str);
  }
}

int client::rewrite(struct sendinf* newinf) {
  // copy card
  this->mycard[0] = newinf->card[0] - 1;
  this->mycard[1] = newinf->card[1] - 1;
  this->num = newinf->num;
  // copy table
  this->table.bank = newinf->table.bank;
  this->table.bet = newinf->table.bet;
  this->table.blind = newinf->table.blind;
  this->table.num = newinf->table.num;
  for (int i = 0; i < 5; i++) {
    this->table.card[i] = newinf->table.card[i] - 1;
  }
  // copy all players
  for (int i = 0; i < 6; i++) {
    this->player[i].cash = newinf->player[i].cash;
    this->player[i].last_action = newinf->player[i].last_action;
    this->player[i].bet = newinf->player[i].bet;
    this->player[i].status = newinf->player[i].status;
    this->player[i].blind = newinf->player[i].blind;
    this->player[i].comb = newinf->player[i].comb;
    this->player[i].card[0] = newinf->player[i].card[0] - 1;
    this->player[i].card[1] = newinf->player[i].card[1] - 1;
    strncpy(this->player[i].login, newinf->player[i].login,
            sizeof(newinf->player[i].login));
  }
  return 0;
}

client::client(int status, int sock, bool* argument, int fildis) {
  this->fd = fildis;
  for (int i = 0; i < 256; i++) {
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

  for (int i = 0; i < 5; i++) this->table.card[i] = 0;

  for (int i = 0; i < 4; i++) this->circ[i] = 0;

  for (int i = 0; i < 6; i++) {
    this->player[i].cash = 0;
    this->player[i].last_action = 0;
    this->player[i].bet = 0;
    this->player[i].status = -1;
    this->player[i].blind = 0;
    this->player[i].card[0] = -1;
    this->player[i].card[1] = -1;
    this->player[i].comb = 0;
    for (size_t j = 0; j < sizeof(this->player[i].login); j++) {
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
    bytes_read = recv(sock, &rec, sizeof(rec), MSG_WAITALL);
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

int client::menu() {
  log(this->fd, "menu\n");
  if (this->argument['t']) printf("menu\n");

  int bytes_read = 0;
  struct recive_t rbuf;
  for (size_t i = 0; i < 256; i++) {
    rbuf.buf[i] = 0;
  }
  struct recivesock rec;

  pthread_mutex_t* mut_exit = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
  if (pthread_mutex_init(mut_exit, NULL)) {
    log(this->fd, "menu mut_exit - init", strerror(errno));
    return 0;
  }
  pthread_mutex_lock(mut_exit);

  void* argv;

  pthread_t menu_get_info_from_server_thread = 0;
  argv = (struct menu_get_info_from_server_argument_t*)malloc(
      sizeof(struct menu_get_info_from_server_argument_t));
  ((struct menu_get_info_from_server_argument_t*)argv)->my = this;
  ((struct menu_get_info_from_server_argument_t*)argv)->mut_exit = mut_exit;
  pthread_create(&menu_get_info_from_server_thread, NULL,
                 menu_get_info_from_server, argv);

  pthread_t menu_scanf_thread = 0;
  if (this->argument['t']) {
    argv = (struct menu_scanf_argument_t*)malloc(
        sizeof(struct menu_scanf_argument_t));
    ((struct menu_scanf_argument_t*)argv)->my = this;
    ((struct menu_scanf_argument_t*)argv)->mut_exit = mut_exit;
    pthread_create(&menu_scanf_thread, NULL, menu_scanf, argv);
  }

  pthread_t menu_start_game_thread = 0;
  if (this->argument['g']) {
    argv = (struct menu_scanf_argument_t*)malloc(
        sizeof(struct menu_scanf_argument_t));
    ((struct menu_scanf_argument_t*)argv)->my = this;
    ((struct menu_scanf_argument_t*)argv)->mut_exit = mut_exit;
    pthread_create(&menu_start_game_thread, NULL, menu_start_game, argv);
  }

  pthread_mutex_lock(mut_exit);
  pthread_cancel(menu_get_info_from_server_thread);

  if (this->argument['t']) pthread_cancel(menu_scanf_thread);
  if (this->argument['g']) pthread_cancel(menu_start_game_thread);

  pthread_join(menu_get_info_from_server_thread, NULL);

  if (this->argument['t']) pthread_join(menu_scanf_thread, NULL);
  if (this->argument['g']) pthread_join(menu_start_game_thread, NULL);

  pthread_mutex_destroy(mut_exit);
  free(mut_exit);
  return 0;
}

int client::help() {
  log(this->fd, "help\n");
  if (this->argument['t']) printf("help\n");

  int bytes_read = 0;
  struct recive_t rbuf;
  for (size_t i = 0; i < 256; i++) {
    rbuf.buf[i] = 0;
  }
  struct recivesock rec;

  pthread_mutex_t mut_exit;
  if (pthread_mutex_init(&mut_exit, NULL)) {
    log(this->fd, "menu mut_exit - init: %s", strerror(errno));
    return 0;
  }
  pthread_mutex_lock(&mut_exit);

  void* argv;

  pthread_t help_get_info_from_server_thread = 0;
  argv = (struct help_get_info_from_server_argument_t*)malloc(
      sizeof(struct help_get_info_from_server_argument_t));
  ((struct help_get_info_from_server_argument_t*)argv)->my = this;
  ((struct help_get_info_from_server_argument_t*)argv)->mut_exit = &mut_exit;
  pthread_create(&help_get_info_from_server_thread, NULL,
                 help_get_info_from_server, argv);

  pthread_t help_scanf_thread = 0;
  if (this->argument['t']) {
    argv = (struct help_scanf_argument_t*)malloc(
        sizeof(struct help_scanf_argument_t));
    ((struct help_scanf_argument_t*)argv)->my = this;
    ((struct help_scanf_argument_t*)argv)->mut_exit = &mut_exit;
    pthread_create(&help_scanf_thread, NULL, help_scanf, argv);
  }

  pthread_mutex_lock(&mut_exit);
  pthread_cancel(help_get_info_from_server_thread);
  if (this->argument['t']) pthread_cancel(help_scanf_thread);

  pthread_join(help_get_info_from_server_thread, NULL);
  if (this->argument['t']) pthread_join(help_scanf_thread, NULL);

  pthread_mutex_destroy(&mut_exit);
  return 0;
}

int client::game() {
  log(this->fd, "game\n");
  if (this->argument['t']) printf("game");

  int bytes_read = 0;
  struct recive_t rbuf;
  for (size_t i = 0; i < 256; i++) {
    rbuf.buf[i] = 0;
  }
  struct recivesock rec;

  pthread_mutex_t mut_exit;
  if (pthread_mutex_init(&mut_exit, NULL)) {
    log(this->fd, "game mut_exit - init: %s", strerror(errno));
    return 0;
  }
  pthread_mutex_lock(&mut_exit);

  void* argv;

  pthread_t game_get_info_from_server_thread = 0;
  argv = (struct game_get_info_from_server_argument_t*)malloc(
      sizeof(struct game_get_info_from_server_argument_t));
  ((struct game_get_info_from_server_argument_t*)argv)->my = this;
  ((struct game_get_info_from_server_argument_t*)argv)->mut_exit = &mut_exit;
  pthread_create(&game_get_info_from_server_thread, NULL,
                 game_get_info_from_server, argv);

  pthread_t game_scanf_thread = 0;
  if (this->argument['t']) {
    argv = (struct game_scanf_argument_t*)malloc(
        sizeof(struct game_scanf_argument_t));
    ((struct game_scanf_argument_t*)argv)->my = this;
    ((struct game_scanf_argument_t*)argv)->mut_exit = &mut_exit;
    pthread_create(&game_scanf_thread, NULL, game_scanf, argv);
  }
  pthread_mutex_lock(&mut_exit);

  pthread_cancel(game_get_info_from_server_thread);

  if (this->argument['t']) pthread_cancel(game_scanf_thread);

  pthread_join(game_get_info_from_server_thread, NULL);

  if (this->argument['t']) pthread_join(game_scanf_thread, NULL);

  pthread_mutex_destroy(&mut_exit);
  return 0;
}

int client::negotiator() {
  int allcards[7];
  int preflop = 0;
  allcards[0] = this->mycard[0];
  allcards[1] = this->mycard[1];
  log(this->fd, "%d %d ", allcards[0], allcards[1]);
  for (int i = 2; i < 7; i++) {
    allcards[i] = this->table.card[i - 2];
    log(this->fd, "%d ", allcards[i]);
  }
  if (allcards[2] <= -1) {
    this->circ[0] = 0;
    this->cur_bet = 0;
  } else if (allcards[5] <= -1)
    this->circ[0] = 1;
  else if (allcards[6] <= -1)
    this->circ[0] = 2;
  else
    this->circ[0] = 3;
  log(this->fd, "\n");

  struct timespec tmp;
  clock_gettime(CLOCK_REALTIME, &tmp);
  srand((unsigned int)tmp.tv_nsec);

  return tester(allcards);
}

int client::tester(int* allcards) {
  int act = -1;
  int count_of_test = 500;
  if (this->circ[0] != 0) {
    double wins = test(allcards, this->circ, count_of_test);
    wins = wins / count_of_test;
    if (wins > 0.35 && wins < 0.5) wins *= 2;
    if (wins >= 0.5) wins = 0.9;
    double bet_cur = this->table.bet - this->player[this->num].bet;
    double win = wins * (this->table.bank);
    log(this->fd, "win %d: %f, %f", this->num, win, bet_cur);
    log(this->fd, "\n");

    if (win < (1 - wins) * bet_cur) act = 0;
    if (((1 - wins) * (bet_cur + this->table.blind) > win) &&
        (win >= (1 - wins) * bet_cur))
      act = 1;
    if (win >= (1 - wins) * (bet_cur + this->table.blind)) act = 2;
  } else {
    act = prefl(allcards[0], allcards[1]);
  }

  double bet_cur = this->table.bet + this->cur_bet;

  this->old_bet = this->player[this->num].bet;

  if (this->circ[this->circ[0]] == 0) {
    this->old_bet = 0.0;
  }
  this->circ[this->circ[0]] += 1;

  if (act == 0) return ACTION_FOLD;
  if (act == 1) {
    if (this->table.bet - this->player[this->num].bet <
        this->player[this->num].cash) {
      return ACTION_CALL;
    } else
      ACTION_FOLD;
  }
  if (act == 2) {
    int vseRavnoStavit = this->table.bet - this->player[this->num].bet;
    int esheMozhno = (this->player[this->num].cash) - vseRavnoStavit;
    int limp = 0;
    for (int i = 0; i < 6; i++)
      if (this->player[i].last_action == ACTION_RAISE ||
          this->player[i].last_action == ACTION_CALL)
        limp++;
    // printf("limp %d\n", limp);
    log(this->fd, "old_bet - %lf, table_blind %lf\n", this->old_bet,
        this->table.blind);
    if ((4 + limp) * this->table.blind < esheMozhno)
      this->player[this->num].bet += (4 + limp) * this->table.blind;
    else if (4 * this->table.blind <= esheMozhno)
      this->player[this->num].bet += 4 * this->table.blind;
    else if (this->player[this->num].cash <
             (this->table.bet + 4 * this->table.blind -
              this->player[this->num].bet) ||
             (this->table.bet + 4 * this->table.blind -
              this->player[this->num].bet <
              this->table.blind))
      return ACTION_CALL;
    else
      return ACTION_FOLD;
    if (this->circ[this->circ[0]] == 1) return ACTION_RAISE;
    return ACTION_CALL;
  }
}