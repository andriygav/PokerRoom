#pragma once
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "play_info.h"
#define EXIT -1
#define FIRST_STATUS 100
#define MENU 1
#define HELP 2
#define GAME 3
#define FROM_TO_TABLE 1025

#define START_CASH 20000.0

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

#define ACTION_CALL 10004
#define ACTION_RAISE 10005
#define ACTION_FOLD 10006

class client {
 public:
  bool argument[256];
  size_t id;
  int status;
  int sock;
  int fd;
  client(int status, int sock, bool* argument, int fd);

  int first_state();
  int change_status(struct recivesock*);

  // Menu object
  int menu();
  // Help object
  char help_text[1024];
  int help();
  // Game object
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
  int tester(int*);
};

static int log(int md, const char* format, ...);