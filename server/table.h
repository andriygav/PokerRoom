#pragma once
#include <pthread.h>
#include <time.h>
#include "count_comb.h"
#include "play_info.h"

#define MAX_CLIENT_NUM 2049
#define MAX_ADMIN_NUM 1
#define BIG_BLIND 1.0
#define START_CASH 20000
// ACTION TABLE

// STATUS TABLE
#define STATUS_NON_PLAYER -1
#define STATUS_CONECT 1
#define STATUS_ACTIVE 2
#define STATUS_IN_GAME 3
#define STATUS_WINER 4
#define STATUS_AFTER_GAME 5

// SOME DEFINE
#define BUTTON_PERSON 0
#define LIT_BLIND_PERSON 1
#define BIG_BLIND_PERSON 2

// POKER ROOM TABLE
#define ROOM_WAITING_ALL_PEOPLE 0
#define ROOM_BEGIN_GAME 1
#define ROOM_FIRST_RAUND 2
#define ROOM_NEXT_LVL 3
#define ROOM_END_GAME 10
#define ROOM_WAIT 4

#define ACTION_CALL 10004
#define ACTION_RAISE 10005
#define ACTION_FOLD 10006
#define ACTION_ALL_IN 10007
#define ACTION_NEW_GAME 10008

#define COUNT_OF_ROOM 50
#define SEND_INFO_TO_CLIENT 1
#define DISCONNECT_CLIENT 2

#pragma pack(push, 1)
struct msg_from_room_t {
  long num;
  long comand;
  struct sendinf inf;
};
#pragma pack(pop)

int set_msg_from_room_t(struct msg_from_room_t* msg, int num,
                        struct sendinf* inf) {
  if (msg != NULL) {
    msg->num = num;
    msg->comand = 0;
    if (inf != NULL) {
      copy_sendinf(&(msg->inf), inf);
    } else {
      set_sendinf(&(msg->inf));
    }
  }
  return 0;
}
#pragma pack(push, 1)
struct msg_to_room_msg_t {
  long comand;
  double rs;
  char login[256];
};
#pragma pack(pop)
#pragma pack(push, 1)
struct msg_to_room_t {
  long num;
  struct msg_to_room_msg_t buf;
};
#pragma pack(pop)

int set_msg_to_room_t(struct msg_to_room_t* msg, int num, int command) {
  msg->num = num;
  msg->buf.comand = command;
  msg->buf.rs = 0;
  for (int i = 0; i < sizeof(msg->buf.login); i++) {
    msg->buf.login[i] = 0;
  }
  return 0;
}
#pragma pack(push, 1)
struct client {
  int PokMesDis;
  long count;
  int fd_poker_log;
  long offset;
  long num[6];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct table_t {
  struct client cl[COUNT_OF_ROOM];
  char arr[MAX_CLIENT_NUM];
  char login[MAX_CLIENT_NUM][256];
  char ip[MAX_CLIENT_NUM][20];
  pthread_t thread_poker[COUNT_OF_ROOM];
  pthread_t thread_client[MAX_CLIENT_NUM];
  pthread_mutex_t mut_read_client;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct privatplayinfo {
  int card_1;
  int card_2;
};
#pragma pack(pop)

static int log(int md, const char* format, ...) {
  char str[256];
  va_list args;
  va_start(args, format);
  vsprintf(str, format, args);
  va_end(args);
  if (md >= 0) {
    if (write(md, str, strnlen(str, 256)) == -1) {
      perror("write");
    }
  }
  return 0;
}

class pokroom {
 public:
  struct publplayinfo pupi[6];
  struct publtableinfo puti;
  struct privatplayinfo prpi[6];

  int message_discrip;
  struct client* client;

  pokroom(int md, struct client*, int number, FILE* file);
  int number;

  bool nead_send_info_to_server;

  int conect(int, char* S);
  int disconect(int);
  int prsend(int);
  int send_public_info();
  int sendtoall();
  int game();
  int set_action_play();
  int start_game();
  // Cards in table
  int card[52];
  int set_colum();
  int get_colum();
  int next_card();
  int number_card;
  int chplayer(int);
  int dell_active_player();
  // Game aray
  int status;
  int button;
  int lit_blind;
  int big_blind;
  int active_player;
  double start_bet;

  FILE* ColumFile;
  size_t count_of_game;

  int reload_bet();

  int call(int);
  int rais(int, double);
  int fold(int);
  int getmoney(int, double);
  int putmoney(int, double);

  int all_in(int);
  double all_in_table[6];
  int count_all_in();
  int final_table[6];

  int open_cards();
  int open_comb();
  int count_comb(int);

  int new_game(int);

  int get_winer();
  int put_money();

  int tup;

  bool action_play[6];

  int first_play;
};

int pokroom::reload_bet() {
  this->puti.bet = 0.0;
  for (int i = 0; i < 6; i++) {
    this->pupi[i].bet = 0;
  }
  return 0;
}

int pokroom::new_game(int i) {
  if (this->pupi[i].cash >= this->start_bet) {
    this->pupi[i].status = STATUS_CONECT;
  }
  return 0;
}

int pokroom::getmoney(int i, double money) {
  if (this->pupi[i].cash - money >= START_CASH) {
    log(this->client->fd_poker_log, "#GETMONEY %d %lf\n", i, money);
    this->pupi[i].cash -= money;
    return 1;
  }
  return 0;
}

int pokroom::putmoney(int i, double money) {
  log(this->client->fd_poker_log, "#PUTMONEY %d %lf\n", i, money);
  this->pupi[i].cash += money;
  return 1;
}

static int get_best_comb(int** arr, int num, int comb, int res[6], int md) {
  log(md, "GET_BEST_COMB_START %d\n", comb);
  for (int i = 0; i < 6; i++) {
    log(md, "%d ", res[i]);
  }
  log(md, "\n");
  int temp[6][5];
  for (int i = 0; i < num; i++) {
    if (res[i] != 0) {
      get_comb_card(arr[i], 7, temp[i], comb);
      for (int j = 0; j < 5; j++) {
        temp[i][j] = ret_higth(temp[i][j]);
      }
    } else {
      for (int j = 0; j < 5; j++) {
        temp[i][j] = 0;
      }
    }
  }
  log(md, "\n");
  for (int i = 0; i < num; i++) {
    for (int j = 0; j < 7; j++) {
      log(md, "%d ", arr[i][j]);
    }
    log(md, "\n");
  }
  log(md, "\n");
  for (int i = 0; i < num; i++) {
    for (int j = 0; j < 5; j++) {
      log(md, "%d ", temp[i][j]);
    }
    log(md, "\n");
  }
  log(md, "\n");
  for (int i = 0; i < 5; i++) {
    int mx = 0;
    for (int j = 0; j < num; j++) {
      if (((temp[j][i] > mx && mx != CARD_A) || temp[j][i] == CARD_A) &&
          res[j] == 1) {
        mx = temp[j][i];
      }
    }
    log(md, "%d ", mx);
    for (int j = 0; j < num; j++) {
      if (temp[j][i] != mx) {
        res[j] = 0;
      }
    }
  }
  log(md, "\n");
  log(md, "GET_BEST_COMB_END %d\n", comb);
  return 0;
}

int pokroom::put_money() {
  for (int i = 0; i < 6; i++) {
    log(this->client->fd_poker_log, "player %d, place %d\n", i,
        this->final_table[i]);
  }

  for (int i = 1; i <= 6; i++) {
    int count_of_winer = 0;
    for (int j = 0; j < 6; j++) {
      if (this->final_table[j] == i) {
        if (this->all_in_table[j] != 0) {
          count_of_winer++;
        }
        this->pupi[j].status = STATUS_WINER;
      }
    }
    if (count_of_winer == 0) {
      count_of_winer = 1;
    }
    double cash = this->puti.bank;
    double forone = cash / count_of_winer;
    for (int j = 0; j < 6; j++) {
      if (this->final_table[j] == i) {
        this->pupi[j].cash += (this->all_in_table[j] != -1)
                                  ? (this->all_in_table[j] / count_of_winer)
                                  : (forone);
        this->puti.bank -= (this->all_in_table[j] != -1)
                               ? (this->all_in_table[j] / count_of_winer)
                               : (forone);
      }
    }
    for (int j = 0; j < 6; j++) {
      this->all_in_table[j] -= (cash - this->puti.bank);
      if (this->all_in_table[j] < 0) {
        this->all_in_table[j] = 0;
      }
    }
  }

  return 0;
}

int pokroom::get_winer() {
  log(this->client->fd_poker_log, "GET_WINER_START\n");

  for (int i = 0; i < 6; i++) {
    if (this->pupi[i].status != STATUS_IN_GAME &&
        this->pupi[i].status != STATUS_ACTIVE) {
      this->final_table[i] = -1;
    } else {
      this->final_table[i] = 0;
    }
  }

  for (int i = 0; i < 6; i++) {
    log(this->client->fd_poker_log, "%d ", this->final_table[i]);
  }
  log(this->client->fd_poker_log, "\n");

  for (int i = 0; i < 6; i++) {
    if (this->pupi[i].status != STATUS_NON_PLAYER) {
      this->pupi[i].status = STATUS_AFTER_GAME;
    }
  }
  int count_of_player = 0;
  for (int i = 0; i < 6; i++) {
    if (this->final_table[i] == 0) count_of_player++;
  }
  int number = 1;
  log(this->client->fd_poker_log, "count user %d\n", count_of_player);

  while (count_of_player != 0) {
    for (int i = 1; i <= 10; i++) {
      int count = 0;
      int num = -1;
      for (int j = 0; j < 6; j++) {
        if (this->pupi[j].comb == i &&
            this->pupi[j].status == STATUS_AFTER_GAME &&
            this->final_table[j] == 0) {
          count++;
          num = j;
        }
      }
      if (count == 1) {
        this->final_table[num] = number;
        break;
      } else if (count > 1) {
        int** arr = (int**)malloc(6 * sizeof(int*));
        int res[6] = {0, 0, 0, 0, 0, 0};
        for (int t = 0; t < 6; t++) {
          arr[t] = (int*)malloc(7 * sizeof(int));
          if (this->pupi[t].comb == i && this->final_table[t] == 0) {
            res[t] = 1;
            for (int p = 0; p < 5; p++) {
              arr[t][p] = this->puti.card[p];
            }
            arr[t][5] = this->pupi[t].card[0];
            arr[t][6] = this->pupi[t].card[1];
          } else {
            for (int p = 0; p < 7; p++) {
              arr[t][p] = 0;
            }
          }
        }
        get_best_comb(arr, 6, i, res, this->client->fd_poker_log);

        for (int t = 0; t < 6; t++) {
          if (this->pupi[t].status == STATUS_AFTER_GAME && res[t] == 1) {
            this->final_table[t] = number;
          }
        }

        for (int t = 0; t < 6; t++) {
          free(arr[t]);
        }
        free(arr);
        break;
      }
    }
    count_of_player = 0;
    for (int i = 0; i < 6; i++) {
      if (this->final_table[i] == 0) count_of_player++;
    }
    number++;
  }

  log(this->client->fd_poker_log, "GET_WINER_END\n");
  return 0;
}

int pokroom::dell_active_player() {
  for (int i = 0; i < 6; i++) {
    if (this->pupi[i].status == STATUS_ACTIVE)
      this->pupi[i].status = STATUS_IN_GAME;
  }
  this->active_player = -1;
  return 0;
}

int pokroom::count_comb(int num) {
  int arr[7];
  for (int i = 0; i < 5; i++) {
    arr[i] = this->puti.card[i];
  }
  arr[5] = this->pupi[num].card[0];
  arr[6] = this->pupi[num].card[1];

  if (is_FLASH_ROYAL(arr, 7, NULL)) {
    return COMB_FLASH_ROYAL;
  }
  if (is_STRET_FLASH(arr, 7, NULL)) {
    return COMB_STRET_FLASH;
  }
  if (is_CARE(arr, 7, NULL)) {
    return COMB_CARE;
  }
  if (is_FULL_HOUSE(arr, 7, NULL)) {
    return COMB_FULL_HOUSE;
  }
  if (is_FLASH(arr, 7, NULL)) {
    return COMB_FLASH;
  }
  if (is_STRET(arr, 7, NULL)) {
    return COMB_STRET;
  }
  if (is_SET(arr, 7, NULL)) {
    return COMB_SET;
  }
  if (is_TWO_PAIRS(arr, 7, NULL)) {
    return COMB_TWO_PAIRS;
  }
  if (is_PAIRS(arr, 7, NULL)) {
    return COMB_PAIRS;
  }
  return COMB_HIGTH_CARDS;
}

int pokroom::open_comb() {
  for (int i = 0; i < 6; i++) {
    if (this->pupi[i].status == STATUS_IN_GAME) {
      this->pupi[i].comb = count_comb(i);
    }
  }
  log(this->client->fd_poker_log, "OPEN_COMB_BEGIN\n");
  for (int i = 0; i < 6; i++) {
    log(this->client->fd_poker_log, "%d - %d\n", i, this->pupi[i].comb);
  }
  log(this->client->fd_poker_log, "OPEN_COMB_END\n");
  return 0;
}

int pokroom::open_cards() {
  for (int i = 0; i < 6; i++) {
    if (this->pupi[i].status == STATUS_IN_GAME) {
      this->pupi[i].card[0] = this->prpi[i].card_1;
      this->pupi[i].card[1] = this->prpi[i].card_2;
    }
  }
  return 0;
}

int pokroom::start_game() {
  this->tup = 0;

  for (int i = 0; i < 6; i++) {
    this->all_in_table[i] = -1;
    this->action_play[i] = false;
    this->pupi[i].bet = 0;
    this->pupi[i].status = STATUS_IN_GAME;
    this->pupi[i].comb = 0;
    this->pupi[i].blind = 0;
    this->pupi[i].last_action = ACTION_NEW_GAME;
    this->pupi[i].card[0] = 0;
    this->pupi[i].card[1] = 0;
  }

  this->button++;
  this->button %= 6;
  this->first_play = this->button;
  this->pupi[this->button].blind = BUTTON_PERSON;

  this->lit_blind = this->button + 1;
  this->lit_blind %= 6;
  this->pupi[this->lit_blind].blind = LIT_BLIND_PERSON;

  this->big_blind = this->button + 2;
  this->big_blind %= 6;
  this->pupi[this->big_blind].blind = BIG_BLIND_PERSON;

  this->active_player = this->button + 3;
  this->active_player %= 6;
  this->start_bet = BIG_BLIND;
  this->puti.bet = this->start_bet;
  this->puti.blind = this->start_bet;

  this->pupi[this->lit_blind].bet = this->puti.bet / 2.0;
  this->pupi[this->big_blind].bet = this->puti.bet;
  this->puti.bank =
      this->pupi[this->lit_blind].bet + this->pupi[this->big_blind].bet;

  this->pupi[this->lit_blind].cash -= this->pupi[this->lit_blind].bet;
  this->pupi[this->big_blind].cash -= this->pupi[this->big_blind].bet;
  this->pupi[this->active_player].status = STATUS_ACTIVE;

  for (int i = 0; i < 5; i++) {
    this->puti.card[i] = 0;
  }

  this->set_colum();
  if (this->ColumFile != NULL) {
    this->get_colum();
  }

  for (int i = 0; i < 6; i++) {
    this->prpi[i].card_1 = this->next_card();
    this->prpi[i].card_2 = this->next_card();
  }

  for (int i = 0; i < 6; i++) {
    this->final_table[i] = 0;
  }

  return 0;
}

int pokroom::all_in(int num) {
  log(this->client->fd_poker_log, "all_in %d\n", num);
  if (this->active_player != num) {
    prsend(num);
    return 0;
  }
  this->puti.bank += this->pupi[this->active_player].cash;
  this->pupi[num].bet += this->pupi[active_player].cash;
  this->puti.bet = (this->puti.bet > this->pupi[num].bet)
                       ? (this->puti.bet)
                       : (this->pupi[num].bet);
  this->pupi[num].cash = 0;
  this->all_in_table[num] = -2;
  this->pupi[active_player].last_action = ACTION_ALL_IN;
  this->pupi[num].status = STATUS_IN_GAME;

  this->action_play[num] = true;

  this->active_player++;
  this->active_player %= 6;
  return 1;
}

int pokroom::call(int num) {
  log(this->client->fd_poker_log, "call %d\n", num);
  if (active_player != num) {
    prsend(num);
    return 0;
  }
  if (this->pupi[num].cash <= (this->puti.bet - this->pupi[num].bet)) {
    return this->all_in(num);
  }

  this->pupi[num].cash -= (this->puti.bet - this->pupi[num].bet);
  this->puti.bank += (this->puti.bet - this->pupi[active_player].bet);
  this->pupi[num].bet = this->puti.bet;
  this->pupi[num].last_action = ACTION_CALL;

  this->action_play[num] = true;

  this->pupi[num].status = STATUS_IN_GAME;
  this->active_player++;
  this->active_player %= 6;
  return 1;
}

int pokroom::fold(int num) {
  log(this->client->fd_poker_log, "fold %d\n", num);

  if (this->active_player != num &&
      this->pupi[num].status != STATUS_NON_PLAYER) {
    prsend(num);
    return 0;
  }

  this->pupi[num].last_action = ACTION_FOLD;
  this->pupi[num].card[0] = -1;
  this->pupi[num].card[1] = -1;
  this->prpi[num].card_1 = -1;
  this->prpi[num].card_2 = -1;

  if (this->pupi[num].status != STATUS_NON_PLAYER)
    this->pupi[num].status = STATUS_CONECT;

  if (this->active_player == num) {
    this->active_player++;
    this->active_player %= 6;
  }

  this->action_play[num] = true;
  return 1;
}

int pokroom::rais(int num, double plus) {
  if (active_player != num) {
    prsend(num);
    return 0;
  }
  log(this->client->fd_poker_log, "rais %d %lf %lf %lf %lf\n", num, plus,
      this->pupi[num].cash, this->puti.bet, this->pupi[num].bet);
  if ((this->pupi[num].cash <= (this->puti.bet + plus - this->pupi[num].bet)) ||
      (this->puti.bet + plus < this->start_bet)) {
    return this->all_in(num);
  }
  this->puti.bet += plus;

  int ret = this->call(num);

  this->pupi[num].last_action = ACTION_RAISE;

  this->action_play[num] = true;
  return ret;
}

static int exists_in(int* arr, size_t size, int value) {
  for (size_t i = 0; i < size; i++) {
    if (arr[i] == value) {
      return 1;
    }
  }
  return 0;
}

int pokroom::next_card() {
  ;
  this->number_card++;
  return card[this->number_card - 1];
}

int pokroom::set_colum() {
  struct timespec tmp;
  clock_gettime(CLOCK_REALTIME, &tmp);
  srand((unsigned int)tmp.tv_nsec);

  for (size_t i = 0; i < 52; i++) {
    this->card[i] = 0;
  }

  for (size_t i = 0; i < 52; i++) {
    int c = rand() % 52 + 1;
    while (exists_in(this->card, 52, c)) {
      c = rand() % 52 + 1;
    }
    this->card[i] = c;
  }
  this->number_card = 0;
  return 0;
}

int pokroom::get_colum() {
  if (this->count_of_game > 0) {
    for (size_t i = 0; i < 52; i++) {
      this->card[i] = 0;
    }
    this->count_of_game--;
    for (size_t i = 0; i < 52; i++) {
      fscanf(this->ColumFile, "%d", &(this->card[i]));
    }
    this->number_card = 0;
  } else {
    return 1;
  }
  return 0;
}

int pokroom::chplayer(int call_cheak) {
  int count = 0;

  for (int i = 0; i < 6; i++) {
    if (this->pupi[i].status == STATUS_IN_GAME) count++;
  }
  if (count == 1) {
    this->status = ROOM_END_GAME;
    return 0;
  }

  int j = 0;
  while ((this->pupi[(this->active_player + j) % 6].status != STATUS_IN_GAME) &&
         (j < 6))
    j++;

  if (j >= 5) {
    this->status = ROOM_END_GAME;
    return 0;
  }

  this->active_player = (this->active_player + j) % 6;

  log(this->client->fd_poker_log, "%d %lf\n", this->active_player,
      this->all_in_table[this->active_player]);

  if ((this->action_play[this->active_player]) &&
      ((this->pupi[this->active_player].bet == this->puti.bet)) &&
      (call_cheak)) {
    this->status = ROOM_NEXT_LVL;
    return 0;
  }

  this->pupi[this->active_player].status = STATUS_ACTIVE;
  if (this->all_in_table[this->active_player] != -1) {
    this->pupi[this->active_player].status = STATUS_IN_GAME;
    this->active_player++;
    this->active_player %= 6;
  }
  return 0;
}

int pokroom::count_all_in() {
  for (int i = 0; i < 6; i++) {
    if (this->all_in_table[i] == -2) {
      this->all_in_table[i] = this->puti.bank;
      for (int j = 0; j < 6; j++) {
        this->all_in_table[i] += ((this->pupi[i].bet - this->pupi[j].bet) < 0)
                                     ? (this->pupi[i].bet - this->pupi[j].bet)
                                     : 0;
      }
    }
  }
  return 0;
}

int pokroom::set_action_play() {
  for (int i = 0; i < 6; i++) {
    this->action_play[i] = false;
    if (this->all_in_table[i] != -1) {
      this->action_play[i] = true;
    }
  }
  return 0;
}

int pokroom::game() {
  int count = 0;
  int count_conect = 0;
  int count_not_player = 0;
  switch (this->status) {
    case ROOM_WAITING_ALL_PEOPLE:
      count = 0;
      for (int i = 0; i < 6; i++) {
        if (this->pupi[i].status == STATUS_CONECT) {
          count++;
        }
      }
      if (count == 6) {
        this->start_game();
        this->status = ROOM_FIRST_RAUND;
        this->sendtoall();
      }
      break;
    case ROOM_FIRST_RAUND:
      while (this->pupi[this->active_player].status != STATUS_ACTIVE &&
             this->status == ROOM_FIRST_RAUND) {
        this->chplayer(1);
        this->sendtoall();
      }
      break;
    case ROOM_NEXT_LVL:
      log(this->client->fd_poker_log, "room next lvl\n");
      this->count_all_in();
      this->reload_bet();
      this->set_action_play();
      for (int i = 0; i < 6; i++) {
        if (this->pupi[i].status == STATUS_ACTIVE)
          this->pupi[i].status = STATUS_IN_GAME;
      }
      switch (tup) {
        case 0:
          this->pupi[this->active_player].status = STATUS_IN_GAME;
          for (int i = 0; i < 3; i++) {
            this->puti.card[i] = this->next_card();
          }
          this->first_play++;
          this->first_play %= 6;
          this->active_player = this->first_play;
          tup++;
          this->status = ROOM_FIRST_RAUND;
          break;
        case 1:
          this->puti.card[3] = this->next_card();
          this->pupi[this->active_player].status = STATUS_IN_GAME;
          this->first_play++;
          this->first_play %= 6;
          this->active_player = this->first_play;
          tup++;
          this->status = ROOM_FIRST_RAUND;
          break;
        case 2:
          this->puti.card[4] = this->next_card();
          this->pupi[this->active_player].status = STATUS_IN_GAME;
          this->first_play++;
          this->first_play %= 6;
          this->active_player = this->first_play;
          tup++;
          this->status = ROOM_FIRST_RAUND;
          break;
        case 3:
          this->status = ROOM_END_GAME;
          break;
      }
      this->sendtoall();
      break;
    case ROOM_END_GAME:
      log(this->client->fd_poker_log, "END_GAME_ROOM\n");
      this->dell_active_player();
      this->open_cards();
      this->open_comb();
      this->get_winer();
      this->put_money();
      this->sendtoall();
      log(this->client->fd_poker_log, "#sRESULT\n");
      for (int i = 0; i < 6; i++) {
        log(this->client->fd_poker_log, "#RES %d %s %d %d %lf\n", i,
            this->pupi[i].login, this->final_table[i], this->pupi[i].comb,
            this->pupi[i].cash);
      }
      log(this->client->fd_poker_log, "#qRESULT\n");
      this->status = ROOM_WAIT;
      break;
    case ROOM_WAIT:
      count_conect = 0;
      count_not_player = 0;
      for (int i = 0; i < 6; i++) {
        if (this->pupi[i].status == STATUS_CONECT) count_conect++;
        if (this->pupi[i].status == STATUS_NON_PLAYER) count_not_player++;
      }
      if (count_not_player + count_conect == 6) {
        this->status = ROOM_WAITING_ALL_PEOPLE;
      }
      break;
    default:
      break;
  }
  return 0;
}

int pokroom::sendtoall() {
  for (int i = 0; i < 6; i++) {
    if (this->client->num[i]) {
      this->prsend(i);
    }
  }
  if (nead_send_info_to_server) {
    this->send_public_info();
  }
  return 0;
}

int pokroom::prsend(int i) {
  if (this->pupi[i].status == STATUS_NON_PLAYER) return 0;
  struct msg_from_room_t msg_send;
  set_msg_from_room_t(&msg_send, 0, NULL);
  struct sendinf* dest = &(msg_send.inf);
  // copy card
  dest->card[0] = this->prpi[i].card_1;
  dest->card[1] = this->prpi[i].card_2;
  for (int j = 0; j < 6; j++) {
    dest->final_table[j] = this->final_table[j];
  }
  dest->num = i;
  // copy table
  dest->table.bank = this->puti.bank;
  dest->table.bet = this->puti.bet;
  dest->table.blind = this->puti.blind;
  dest->table.num = this->puti.num;
  for (int j = 0; j < 5; j++) {
    dest->table.card[j] = this->puti.card[j];
  }
  // copy all players
  for (int j = 0; j < 6; j++) {
    dest->player[j].cash = this->pupi[j].cash;
    dest->player[j].last_action = this->pupi[j].last_action;
    dest->player[j].bet = this->pupi[j].bet;
    dest->player[j].status = this->pupi[j].status;
    dest->player[j].blind = this->pupi[j].blind;
    dest->player[j].card[0] = this->pupi[j].card[0];
    dest->player[j].card[1] = this->pupi[j].card[1];
    dest->player[j].comb = this->pupi[j].comb;
    strncpy(dest->player[j].login, this->pupi[j].login,
            sizeof(this->pupi[j].login));
  }
  msg_send.comand = SEND_INFO_TO_CLIENT;
  msg_send.num = this->client->num[i];
  msgsnd(this->message_discrip, (void*)(&msg_send),
         sizeof(msg_send) - sizeof(long), 0);
  return 0;
}

int pokroom::send_public_info() {
  struct msg_from_room_t msg_send;
  set_msg_from_room_t(&msg_send, 0, NULL);
  struct sendinf* dest = &(msg_send.inf);
  // copy card
  dest->card[0] = -1;
  dest->card[1] = -1;
  for (int j = 0; j < 6; j++) {
    dest->final_table[j] = this->final_table[j];
  }
  dest->num = 1;
  // copy table
  dest->table.bank = this->puti.bank;
  dest->table.bet = this->puti.bet;
  dest->table.blind = this->puti.blind;
  dest->table.num = this->puti.num;
  for (int j = 0; j < 5; j++) {
    dest->table.card[j] = this->puti.card[j];
  }
  // copy all players
  for (int j = 0; j < 6; j++) {
    dest->player[j].cash = this->pupi[j].cash;
    dest->player[j].last_action = this->pupi[j].last_action;
    dest->player[j].bet = this->pupi[j].bet;
    dest->player[j].status = this->pupi[j].status;
    dest->player[j].blind = this->pupi[j].blind;
    dest->player[j].card[0] = this->pupi[j].card[0];
    dest->player[j].card[1] = this->pupi[j].card[1];
    dest->player[j].comb = this->pupi[j].comb;
    strncpy(dest->player[j].login, this->pupi[j].login,
            sizeof(this->pupi[j].login));
  }
  msg_send.comand = SEND_INFO_TO_CLIENT;
  msg_send.num = 7 + MAX_CLIENT_NUM;
  msgsnd(this->message_discrip, (void*)(&msg_send),
         sizeof(msg_send) - sizeof(long), 0);
  return 0;
}

int pokroom::disconect(int i) {
  log(this->client->fd_poker_log, "disconnect\n");
  struct msg_from_room_t msg_send;
  set_msg_from_room_t(&msg_send, 0, NULL);
  msg_send.comand = DISCONNECT_CLIENT;
  msg_send.num = this->client->num[i];
  msgsnd(this->message_discrip, (void*)(&msg_send),
         sizeof(msg_send) - sizeof(long), 0);
  log(this->client->fd_poker_log, "#disconnect %d %s %lf\n", i,
      this->pupi[i].login, this->pupi[i].cash);
  this->client->num[i] = 0;
  this->pupi[i].cash = 0;
  this->pupi[i].last_action = 0;
  this->pupi[i].bet = 0;
  this->pupi[i].status = STATUS_NON_PLAYER;
  this->pupi[i].blind = 0;
  this->pupi[i].card[0] = -1;
  this->pupi[i].card[1] = -1;
  this->pupi[i].comb = 0;
  sprintf(this->pupi[i].login, "UNKNOW");
  this->client->count--;
  return 0;
}

int pokroom::conect(int i, char* login) {
  int num = this->client->num[i];
  this->pupi[i].cash = START_CASH;
  this->pupi[i].last_action = 0;
  this->pupi[i].bet = 0;
  this->pupi[i].status = STATUS_CONECT;
  this->pupi[i].blind = 0;
  this->pupi[i].card[0] = -1;
  this->pupi[i].card[1] = -1;
  this->pupi[i].comb = 0;
  sprintf(this->pupi[i].login, "%s_%d", login, i);
  for (size_t t = 128; t < 256; t++) login[t] = 0;
  log(this->client->fd_poker_log, "#connect %d %s %lf\n", i,
      this->pupi[i].login, this->pupi[i].cash);
  return 0;
}

pokroom::pokroom(int md, struct client* cl, int numer, FILE* file) {
  this->ColumFile = file;
  this->count_of_game = -1;
  if (this->ColumFile != NULL) {
    fscanf(this->ColumFile, "%zu", &(this->count_of_game));
  }
  this->nead_send_info_to_server = false;
  this->number = numer;
  this->message_discrip = md;
  this->client = cl;
  for (int i = 0; i < 6; i++) {
    this->client->num[i] = 0;
  }
  for (int i = 0; i < 6; i++) {
    this->final_table[i] = 0;
  }
  this->client->count = 0;
  // refresh public table info
  this->puti.bank = 0;
  this->puti.bet = 0;
  this->puti.blind = 0;
  this->puti.num = numer;
  for (int i = 0; i < 5; i++) {
    this->puti.card[i] = 0;
  }
  // refresh public play info
  for (int i = 0; i < 6; i++) {
    this->pupi[i].cash = 0;
    this->pupi[i].last_action = 0;
    this->pupi[i].bet = 0;
    this->pupi[i].status = STATUS_NON_PLAYER;
    this->pupi[i].blind = 0;
    this->pupi[i].card[0] = -1;
    this->pupi[i].card[1] = -1;
    this->pupi[i].comb = 0;
    for (size_t j = 0; j < sizeof(this->pupi[i].login); j++) {
      this->pupi[i].login[j] = 0;
    }
    sprintf(this->pupi[i].login, "UNKNOW");
  }
  // refresh privat play info
  for (int i = 0; i < 6; i++) {
    this->prpi[i].card_1 = -1;
    this->prpi[i].card_2 = -1;
  }

  // Values for game
  this->status = ROOM_WAITING_ALL_PEOPLE;
  this->button = 0;
  this->active_player = 0;
  this->start_bet = BIG_BLIND;
}
