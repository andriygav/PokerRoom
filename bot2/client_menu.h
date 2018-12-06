struct menu_scanf_argument_t {
  client* my;
  pthread_mutex_t* mut_exit;
};

struct menu_get_info_from_server_argument_t {
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

void* menu_scanf(void* arguments) {
  client* my = ((struct menu_scanf_argument_t*)arguments)->my;
  pthread_mutex_t* mut_exit =
      ((struct menu_scanf_argument_t*)arguments)->mut_exit;
  free((struct menu_scanf_argument_t*)arguments);

  struct recive_t rbuf;
  for (size_t i = 0; i < 256; i++) {
    rbuf.buf[i] = 0;
  }
  for (size_t i = 0; i < 256; i++) {
    rbuf.login[i] = 0;
  }
  strncpy(rbuf.login, "bot2", 256);
  char c;
  while (1) {
    printf("->%c", '\0');
    scanf("%[^\n]s", rbuf.buf);
    char c = 0;
    scanf("%c", &c);
    if (!strncmp(rbuf.buf, "exit", 4)) {
      my->status = EXIT;
      rbuf.id = my->id;
      send(my->sock, &rbuf, sizeof(rbuf), 0);
    } else if (!strncmp(rbuf.buf, "game", 4)) {
      rbuf.id = my->id;
      send(my->sock, &rbuf, sizeof(rbuf), 0);
    } else if (!strncmp(rbuf.buf, "help", 4)) {
      rbuf.id = my->id;
      send(my->sock, &rbuf, sizeof(rbuf), 0);
    }
  }
  pthread_mutex_unlock(mut_exit);
  return NULL;
}

void* menu_start_game(void* arguments) {
  client* my = ((struct menu_scanf_argument_t*)arguments)->my;
  pthread_mutex_t* mut_exit =
      ((struct menu_scanf_argument_t*)arguments)->mut_exit;
  free((struct menu_scanf_argument_t*)arguments);

  struct recive_t rbuf;
  for (size_t i = 0; i < 256; i++) {
    rbuf.buf[i] = 0;
  }
  for (size_t i = 0; i < 256; i++) {
    rbuf.login[i] = 0;
  }
  strncpy(rbuf.login, "bot2", 256);
  char c;
  for (int i = 0; i < 10; i++) {
    sleep(1);
    sprintf(rbuf.buf, "game");
    rbuf.id = my->id;
    send(my->sock, &rbuf, sizeof(rbuf), 0);
  }
  pthread_mutex_unlock(mut_exit);
  return NULL;
}
