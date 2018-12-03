struct help_get_info_from_server_argument_t {
  client* my;
  pthread_mutex_t* mut_exit;
};

struct help_scanf_argument_t {
  client* my;
  pthread_mutex_t* mut_exit;
};
void* help_get_info_from_server(void* arguments) {
  client* my = ((struct help_get_info_from_server_argument_t*)arguments)->my;
  pthread_mutex_t* mut_exit =
      ((struct help_get_info_from_server_argument_t*)arguments)->mut_exit;
  free((struct help_get_info_from_server_argument_t*)arguments);

  struct recivesock rec;
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
    }
  }
  pthread_mutex_unlock(mut_exit);
  return NULL;
}

void* help_scanf(void* arguments) {
  client* my = ((struct help_scanf_argument_t*)arguments)->my;
  pthread_mutex_t* mut_exit =
      ((struct help_scanf_argument_t*)arguments)->mut_exit;
  free((struct help_scanf_argument_t*)arguments);

  struct recive_t rbuf;
  for (size_t i = 0; i < 256; i++) {
    rbuf.buf[i] = 0;
  }
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
    } else if (!strncmp(rbuf.buf, "menu", 4)) {
      rbuf.id = my->id;
      send(my->sock, &rbuf, sizeof(rbuf), 0);
    }
  }
  pthread_mutex_unlock(mut_exit);
  return NULL;
}
