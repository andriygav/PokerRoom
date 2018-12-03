struct game_scanf_argument_t {
  client* my;
  pthread_mutex_t* mut_exit;
};

struct game_get_info_from_server_argument_t {
  client* my;
  pthread_mutex_t* mut_exit;
};

void* game_get_info_from_server(void* arguments) {
  client* my = ((struct game_get_info_from_server_argument_t*)arguments)->my;
  pthread_mutex_t* mut_exit =
      ((struct game_get_info_from_server_argument_t*)arguments)->mut_exit;
  free((struct game_get_info_from_server_argument_t*)arguments);

  struct recivesock rec;
  struct recive_t rbuf;
  for (size_t i = 0; i < 256; i++) {
    rbuf.buf[i] = 0;
  }
  for (size_t i = 0; i < 256; i++) {
    rbuf.login[i] = 0;
  }
  rbuf.id = 0;

  int was_decl = 0;
  int flag = 0, flag1 = 0;
  int ccount = 0;
  double getMon = 0.0, takeMon = 0.0;
  bool get_money = false, take_money = false;
  while (1) {
    if (recv(my->sock, &rec, sizeof(rec), 0) == 0) {
      log(my->fd, "Lost conection whith server\n");
      my->status = EXIT;
      pthread_mutex_unlock(mut_exit);
      return NULL;
    }

    my->id = rec.id;
    if (rec.code < 1024) {
      my->change_status(&rec);
      pthread_mutex_unlock(mut_exit);
      return NULL;
    } else if (rec.code == FROM_TO_TABLE) {
      my->rewrite(&(rec.inf));
      my->show_all_inf();

      if (my->cheak_pok_status(STATUS_ACTIVE) && flag != my->id) {
        char str[2];

        int act = my->negotiator();
        if (act == ACTION_CALL) {
          log(my->fd, "%d call\n", my->num);
          sprintf(rbuf.buf, "call");
        }
        if (act == ACTION_FOLD) {
          log(my->fd, "%d fold\n", my->num);
          sprintf(rbuf.buf, "fold");
        }
        if (act == ACTION_RAISE) {
          char str[256];
          log(my->fd, "%d raise %lf\n", my->num,
              my->player[my->num].bet - my->old_bet);
          sprintf(rbuf.buf, "raise %f", my->player[my->num].bet - my->old_bet);
        }
        my->cur_bet += my->player[my->num].bet - my->old_bet;

        rbuf.id = my->id;
        send(my->sock, &rbuf, sizeof(rbuf), 0);
        flag = my->id;
        my->print_log();
      }

      if ((my->cheak_pok_status(STATUS_WINER) ||
           my->cheak_pok_status(STATUS_AFTER_GAME)) &&
          get_money && !take_money) {
        if (getMon != 0 &&
            abs(getMon - my->player[my->num].cash) > my->table.blind) {
          my->my_money += getMon - my->player[my->num].cash;
          log(my->fd, "#MONEY %d -> %lf\n", my->num, my->my_money);
          getMon = 0.0;
          get_money = false;
        }
      }

      if ((my->cheak_pok_status(STATUS_WINER) ||
           my->cheak_pok_status(STATUS_AFTER_GAME)) &&
          !get_money && !take_money) {
        // printf("%s %s\n", get_money? "get_true":"get_false",
        // take_money?"take_true":"take_false");
        if (my->player[my->num].cash > 2 * START_CASH) {
          getMon = my->player[my->num].cash;
          sprintf(rbuf.buf, "getmoney %lf",
                  my->player[my->num].cash - (START_CASH + 1));
          rbuf.id = my->id;
          send(my->sock, &rbuf, sizeof(rbuf), 0);
          get_money = true;
        }
      }

      if ((my->cheak_pok_status(STATUS_WINER) ||
           my->cheak_pok_status(STATUS_AFTER_GAME)) &&
          take_money && !get_money) {
        if (takeMon < 2 * my->table.blind &&
            abs(takeMon - my->player[my->num].cash) > my->table.blind) {
          my->my_money += takeMon - my->player[my->num].cash;
          log(my->fd, "#MONEY %d -> %lf\n", my->num, my->my_money);
          takeMon = 0.0;
          take_money = false;
        }
      }

      if ((my->cheak_pok_status(STATUS_WINER) ||
           my->cheak_pok_status(STATUS_AFTER_GAME)) &&
          !take_money && !get_money) {
        // printf("%s %s\n", get_money? "get_true":"get_false",
        // take_money?"take_true":"take_false");
        if (my->player[my->num].cash < 2 * my->table.blind) {
          takeMon = my->player[my->num].cash;
          sprintf(rbuf.buf, "putmoney %lf", START_CASH);
          rbuf.id = my->id;
          send(my->sock, &rbuf, sizeof(rbuf), 0);
          take_money = true;
        }
      }
      if ((my->cheak_pok_status(STATUS_WINER) ||
           my->cheak_pok_status(STATUS_AFTER_GAME)) &&
          (ccount % 100 == 0)) {
        log(my->fd, "my_money -> %lf\n",
            my->player[my->num].cash + my->my_money - START_CASH);
      }

      if (((my->cheak_pok_status(STATUS_WINER) ||
            my->cheak_pok_status(STATUS_AFTER_GAME)) &&
           ccount < 10000) &&
          flag1 != my->id && !get_money && !take_money) {
        ccount++;
        sprintf(rbuf.buf, "new");
        rbuf.id = my->id;
        send(my->sock, &rbuf, sizeof(rbuf), 0);
        flag1 = my->id;
        for (int i = 0; i < 4; i++) my->circ[i] = 0;
      }
    }
  }
  pthread_mutex_unlock(mut_exit);
  return NULL;
}

void* game_scanf(void* arguments) {
  client* my = ((struct game_scanf_argument_t*)arguments)->my;
  pthread_mutex_t* mut_exit =
      ((struct game_scanf_argument_t*)arguments)->mut_exit;
  free((struct game_scanf_argument_t*)arguments);

  struct recive_t rbuf;
  for (size_t i = 0; i < 256; i++) {
    rbuf.buf[i] = 0;
  }

  while (1) {
    printf("->%c", '\0');
    scanf("%[^\n]s", rbuf.buf);
    char c = 0;
    scanf("%c", &c);
    if (!strncmp(rbuf.buf, "exit", 4)) {
      my->status = EXIT;
      rbuf.id = my->id;
      send(my->sock, &rbuf, sizeof(rbuf), 0);
    } else if (!strncmp(rbuf.buf, "help", 4)) {
      rbuf.id = my->id;
      send(my->sock, &rbuf, sizeof(rbuf), 0);
    } else if (!strncmp(rbuf.buf, "menu", 4)) {
      rbuf.id = my->id;
      send(my->sock, &rbuf, sizeof(rbuf), 0);
    } else if (!strncmp(rbuf.buf, "refresh", 7)) {
      rbuf.id = my->id;
      send(my->sock, &rbuf, sizeof(rbuf), 0);
    } else if (!strncmp(rbuf.buf, "disconnect", 11)) {
      rbuf.id = my->id;
      send(my->sock, &rbuf, sizeof(rbuf), 0);
    } else if (!strncmp(rbuf.buf, "new", 3)) {
      if (my->cheak_pok_status(STATUS_AFTER_GAME) ||
          my->cheak_pok_status(STATUS_WINER)) {
        rbuf.id = my->id;
        send(my->sock, &rbuf, sizeof(rbuf), 0);
      }
    } else if (!strncmp(rbuf.buf, "call", 4)) {
      if (my->cheak_pok_status(STATUS_ACTIVE)) {
        rbuf.id = my->id;
        send(my->sock, &rbuf, sizeof(rbuf), 0);
      }
    } else if (!strncmp(rbuf.buf, "raise", 5)) {
      double x = 0;
      sscanf(rbuf.buf + 5, "%lf", &x);
      if (x > 0) {
        if (my->cheak_pok_status(STATUS_ACTIVE)) {
          rbuf.id = my->id;
          send(my->sock, &rbuf, sizeof(rbuf), 0);
        }
      }
    } else if (!strncmp(rbuf.buf, "fold", 4)) {
      if (my->cheak_pok_status(STATUS_ACTIVE)) {
        rbuf.id = my->id;
        send(my->sock, &rbuf, sizeof(rbuf), 0);
      }
    }
  }
  pthread_mutex_unlock(mut_exit);
  return NULL;
}
