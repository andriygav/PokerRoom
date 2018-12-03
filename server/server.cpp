#include <readline/history.h>
#include <readline/readline.h>

#include <signal.h>

#include "display.h"
#include "poker_table.h"

#define EXIT -1
#define FIRST_STATUS 0
#define MENU 1
#define HELP 2
#define GAME 3
#define FROM_TO_TABLE 1025
#define SERVER_DISCONNECT_YOU 1026

static char** get_argv(const char* buf, size_t* count) {
  size_t len = strlen(buf);
  char** argv = (char**)malloc((len + 1) * sizeof(char*));
  size_t j = 0;
  if (argv == NULL) {
    goto out_get_argv;
  }
  for (int i = 0; i < len + 1; i++) {
    argv[i] = NULL;
  }
  *count = 0;

  argv[*count] = (char*)malloc((len + 1) * sizeof(char));

  if (argv[*count] == NULL) {
    goto out_get_argv;
  }

  for (size_t i = 0; i < (len + 1); i++) {
    argv[*count][i] = 0;
  }

  for (size_t i = 0; i < len; i++) {
    if (buf[i] == ' ') {
      while ((buf[i] == ' ' || buf[i] == '\t') && i < len) {
        i++;
      }
      i--;
    }

    argv[*count][j] = buf[i];
    if (buf[i] == ' ') {
      argv[*count][j] = 0;
      (*count)++;
      argv[*count] = (char*)malloc((len + 1) * sizeof(char));
      if (argv[*count] == NULL) {
        goto out_get_argv;
      }
      for (size_t i = 0; i < len + 1; i++) {
        argv[*count][i] = 0;
      }
      j = -1;
    }
    j++;
  }
  argv[*count][j] = 0;
  (*count)++;

  return argv;

out_get_argv:
  if (argv != NULL) {
    for (size_t i = 0; i < len + 1; i++) {
      if (argv[i] != NULL) {
        free(argv[i]);
        argv[i] = NULL;
      }
    }
    free(argv);
    argv = NULL;
  }

  return nullptr;
}

size_t next_id(size_t id) {
  size_t ret = id;
  size_t old_id = id;
  id += rand();
  if (id < 1000) id *= 1000;
  srand(id);
  while (ret == old_id) {
    ret = rand();
  }
  return ret;
}

#pragma pack(push, 1)
struct client_argumets_t {
  int sock;
  int md;
  int num;
  struct client* cl;
  struct table_t* tb;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct client_get_info_from_server_argumets_t {
  int sock;
  int md;
  int num;
  size_t* id;
  struct table_t* tb;
  int fd;
};
#pragma pack(pop)

static struct client* cheak_room(struct table_t* tb, int num) {
  for (int i = 0; i < COUNT_OF_ROOM; i++) {
    if (tb->cl[i].PokMesDis != -1) {
      for (int j = 0; j < 6; j++) {
        if (tb->cl[i].num[j] == num) {
          return &(tb->cl[i]);
        }
      }
    }
  }
  return nullptr;
}

static struct client* find_room(struct table_t* tb) {
  for (int i = 0; i < COUNT_OF_ROOM; i++) {
    if (tb->cl[i].PokMesDis != -1) {
      for (int j = 0; j < 6; j++) {
        if (tb->cl[i].num[j] == 0) {
          return &(tb->cl[i]);
        }
      }
    }
  }

  return nullptr;
}

void* client_get_info_from_server(void* arguments) {
  int sock = ((struct client_get_info_from_server_argumets_t*)arguments)->sock;
  int md = ((struct client_get_info_from_server_argumets_t*)arguments)->md;
  int num = ((struct client_get_info_from_server_argumets_t*)arguments)->num;
  size_t* id = ((struct client_get_info_from_server_argumets_t*)arguments)->id;
  int fd = ((struct client_get_info_from_server_argumets_t*)arguments)->fd;
  struct table_t* tb =
      ((struct client_get_info_from_server_argumets_t*)arguments)->tb;
  free((struct client_get_info_from_server_argumets_t*)arguments);

  struct msg_from_server_t msg_from_serv;
  msg_from_serv.num = 0;
  msg_from_serv.comand = 0;

  struct recivesock rec;
  set_recivesock(&rec, 0, NULL);

  struct client* cl = NULL;
  struct msg_to_room_t msg_to_room;

  for (int i = 0; i < sizeof(msg_to_room.buf.login); i++) {
    msg_to_room.buf.login[i] = 0;
  }

  while (1) {
    if (msgrcv(md, (void*)(&msg_from_serv), sizeof(msg_from_serv), num, 0) ==
        -1) {
      log(fd, "msgrcv on client from server: %s", strerror(errno));
      return nullptr;
    }
    switch (msg_from_serv.comand) {
      case SERVER_COMAND_EXIT:
        pthread_mutex_lock(&(tb->mut_read_client));
        cl = cheak_room(tb, num);
        if (cl != NULL) {
          int k = -1;
          for (int i = 0; i < 6; i++) {
            if (cl->num[i] == num) {
              k = i;
              break;
            }
          }
          if (k != -1) {
            msg_to_room.num = k + MAX_CLIENT_NUM;
            msg_to_room.buf.comand = DISCONECT;
            msg_to_room.buf.rs = 0;
            msgsnd(cl->PokMesDis, (void*)(&msg_to_room),
                   sizeof(msg_to_room) - sizeof(msg_to_room.num), 0);
          }
        }
        tb->arr[num] = 0;
        pthread_mutex_unlock(&(tb->mut_read_client));
        return nullptr;
        break;
      case SERVER_DESTROY_ROOM:
        rec.code = MENU;
        *id = next_id(*id);
        rec.id = *id;
        send(sock, &rec, sizeof(rec), 0);
        break;
      default:
        break;
    }
  }
  return nullptr;
}

#pragma pack(push, 1)
struct client_get_info_from_pokerroom_argumets_t {
  int sock;
  int md;
  int num;
  size_t* id;
  struct client* cl;
  int fd;
};
#pragma pack(pop)

void* client_get_info_from_pokerroom(void* arguments) {
  int sock =
      ((struct client_get_info_from_pokerroom_argumets_t*)arguments)->sock;
  int md = ((struct client_get_info_from_pokerroom_argumets_t*)arguments)->md;
  int num = ((struct client_get_info_from_pokerroom_argumets_t*)arguments)->num;
  size_t* id =
      ((struct client_get_info_from_pokerroom_argumets_t*)arguments)->id;
  int fd = ((struct client_get_info_from_pokerroom_argumets_t*)arguments)->fd;
  struct client* cl =
      ((struct client_get_info_from_pokerroom_argumets_t*)arguments)->cl;
  free((struct client_get_info_from_pokerroom_argumets_t*)arguments);

  struct msg_from_room_t msg_recive;
  set_msg_from_room_t(&msg_recive, 0, NULL);

  struct recivesock rec;
  set_recivesock(&rec, 0, NULL);

  struct msg_from_server_t msg_from_serv;
  msg_from_serv.num = 0;
  msg_from_serv.comand = 0;

  while (1) {
    if (msgrcv(cl->PokMesDis, (void*)(&msg_recive), sizeof(msg_recive), num,
               0) == -1) {
      log(fd, "msgrcv on client from poker: %s", strerror(errno));
      msg_from_serv.comand = SERVER_COMAND_EXIT;
      msg_from_serv.num = num;
      if (md >= 0) {
        msgsnd(md, (void*)(&msg_from_serv),
               sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);
      }
      return nullptr;
    }
    if (msg_recive.comand == SEND_INFO_TO_CLIENT) {
      log(fd, "send info from poker room to %d\n", num);
      rec.code = FROM_TO_TABLE;
      rec.id = *id;
      copy_sendinf(&(rec.inf), &(msg_recive.inf));
      send(sock, &rec, sizeof(rec), 0);
    } else if (msg_recive.comand == DISCONNECT_CLIENT) {
      log(fd, "close read thread for client %d\n", num);
      return nullptr;
    }
  }

  return nullptr;
}

#pragma pack(push, 1)
struct client_get_info_from_client_argumets_t {
  int sock;
  int md;
  int num;
  size_t* id;
  struct table_t* tb;
  int fd;
};
#pragma pack(pop)

static int start_poker_thread(int sock, int md, int num, size_t* id,
                              struct client* cl,
                              pthread_t* client_get_info_from_pokerroom_thread,
                              int fd) {
  struct msg_from_room_t msg_recive;
  set_msg_from_room_t(&msg_recive, 0, NULL);

  if (cl->PokMesDis >= 0) {
    while (msgrcv(cl->PokMesDis, (void*)(&msg_recive), sizeof(msg_recive), num,
                  IPC_NOWAIT) > 0)
      ;
    void* argv;
    *client_get_info_from_pokerroom_thread = 0;
    argv = (struct client_get_info_from_pokerroom_argumets_t*)malloc(
        sizeof(struct client_get_info_from_pokerroom_argumets_t));
    if (argv == NULL) {
      log(fd, "start: cant find memory\n");
      return 1;
    }
    ((struct client_get_info_from_pokerroom_argumets_t*)argv)->sock = sock;
    ((struct client_get_info_from_pokerroom_argumets_t*)argv)->md = md;
    ((struct client_get_info_from_pokerroom_argumets_t*)argv)->num = num;
    ((struct client_get_info_from_pokerroom_argumets_t*)argv)->id = id;
    ((struct client_get_info_from_pokerroom_argumets_t*)argv)->cl = cl;
    ((struct client_get_info_from_pokerroom_argumets_t*)argv)->fd = fd;
    pthread_create(client_get_info_from_pokerroom_thread, NULL,
                   client_get_info_from_pokerroom, argv);
    log(fd, "start poker tread\n");
  }
  return 0;
}

static int end_poker_thread(pthread_t* client_get_info_from_pokerroom_thread,
                            int fd) {
  pthread_cancel(*client_get_info_from_pokerroom_thread);
  pthread_join(*client_get_info_from_pokerroom_thread, NULL);

  log(fd, "end poker tread\n");
  return 0;
}

void* client_get_info_from_client(void* arguments) {
  int sock = ((struct client_get_info_from_client_argumets_t*)arguments)->sock;
  int md = ((struct client_get_info_from_client_argumets_t*)arguments)->md;
  int num = ((struct client_get_info_from_client_argumets_t*)arguments)->num;
  size_t* id = ((struct client_get_info_from_client_argumets_t*)arguments)->id;
  int fd = ((struct client_get_info_from_client_argumets_t*)arguments)->fd;
  struct table_t* tb =
      ((struct client_get_info_from_client_argumets_t*)arguments)->tb;
  struct client* cl = NULL;

  free((struct client_get_info_from_client_argumets_t*)arguments);

  struct recivesock rec;
  set_recivesock(&rec, 0, NULL);

  struct recive_t rec_sock;
  for (size_t i = 0; i < 256; i++) {
    rec_sock.buf[i] = 0;
  }
  for (size_t i = 0; i < 256; i++) {
    rec_sock.login[i] = 0;
  }

  int bytes_read = 0;

  struct msg_from_server_t msg_from_serv;
  msg_from_serv.num = 0;
  msg_from_serv.comand = 0;

  int poknum = 0;

  pthread_t client_get_info_from_pokerroom_thread = 0;

  while (1) {
  whil:
    bytes_read = recv(sock, &rec_sock, sizeof(rec_sock), 0);
    if (bytes_read == 0) {
      log(fd, "Lost conection with %d\n", num);
      msg_from_serv.comand = SERVER_COMAND_EXIT;
      msg_from_serv.num = num;
      msgsnd(md, (void*)(&msg_from_serv),
             sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);
      return nullptr;
    }

    log(fd, "\"%s\" %d %zu %zu\n", rec_sock.buf, num, *id, rec_sock.id);
    if (!strncmp(rec_sock.buf, "exit", 4)) {
      msg_from_serv.comand = SERVER_COMAND_EXIT;
      msg_from_serv.num = num;
      msgsnd(md, (void*)(&msg_from_serv),
             sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);
      return nullptr;
    } else if (!strncmp(rec_sock.buf, "menu", 4) && rec_sock.id == *id) {
      *id = next_id(*id);
      rec.id = *id;
      rec.code = MENU;
      send(sock, &rec, sizeof(rec), 0);
      if (cl != NULL) {
        end_poker_thread(&client_get_info_from_pokerroom_thread, fd);
        cl = NULL;
      }
    } else if (!strncmp(rec_sock.buf, "help", 4) && rec_sock.id == *id) {
      *id = next_id(*id);
      rec.id = *id;
      rec.code = HELP;
      send(sock, &rec, sizeof(rec), 0);
      if (cl != NULL) {
        end_poker_thread(&client_get_info_from_pokerroom_thread, fd);
        cl = NULL;
      }
    } else if (!strncmp(rec_sock.buf, "game", 4) && rec_sock.id == *id) {
      pthread_mutex_lock(&(tb->mut_read_client));
      log(fd, "start game %d - login %s\n", num, rec_sock.login);
      cl = cheak_room(tb, num);
      if (cl != NULL) {
        if (start_poker_thread(sock, md, num, id, cl,
                               &client_get_info_from_pokerroom_thread, fd)) {
          goto whil;
        }
        *id = next_id(*id);
        rec.id = *id;
        rec.code = GAME;
        send(sock, &rec, sizeof(rec), 0);

        for (int i = 0; i < 6; i++) {
          if (cl->num[i] == num) {
            poknum = i;
            break;
          }
        }
        struct msg_to_room_t msg_to_room;
        msg_to_room.num = poknum + MAX_CLIENT_NUM;
        msg_to_room.buf.comand = REFRESH;
        msg_to_room.buf.rs = 0;
        msgsnd(cl->PokMesDis, (void*)(&msg_to_room), sizeof(msg_to_room.buf),
               0);
        pthread_mutex_unlock(&(tb->mut_read_client));
        goto whil;
      }
      cl = find_room(tb);
      if (cl != NULL) {
        cl->count++;
        for (int i = 0; i < 6; i++) {
          if (cl->num[i] == 0) {
            cl->num[i] = num;
            poknum = i;
            break;
          }
        }
        *id = next_id(*id);
        rec.id = *id;
        rec.code = GAME;
        send(sock, &rec, sizeof(rec), 0);
        start_poker_thread(sock, md, num, id, cl,
                           &client_get_info_from_pokerroom_thread, fd);

        struct msg_to_room_t msg_to_room;
        msg_to_room.num = poknum + MAX_CLIENT_NUM;
        msg_to_room.buf.comand = CONECT;
        msg_to_room.buf.rs = 0;
        strncpy(msg_to_room.buf.login, rec_sock.login, 256);
        msgsnd(cl->PokMesDis, (void*)(&msg_to_room), sizeof(msg_to_room.buf),
               0);
      } else {
        *id = next_id(*id);
        rec.id = *id;
        rec.code = MENU;
        send(sock, &rec, sizeof(rec), 0);
      }
      pthread_mutex_unlock(&(tb->mut_read_client));
    } else if (!strncmp(rec_sock.buf, "new", 3) && rec_sock.id == *id) {
      if (cl != NULL) {
        struct msg_to_room_t msg_to_room;
        msg_to_room.num = poknum + MAX_CLIENT_NUM;
        msg_to_room.buf.comand = ACTION_NEW;
        msg_to_room.buf.rs = 0;
        *id = next_id(*id);
        msgsnd(cl->PokMesDis, (void*)(&msg_to_room), sizeof(msg_to_room.buf),
               0);
      }
    } else if (!strncmp(rec_sock.buf, "refresh", 7) && rec_sock.id == *id) {
      if (cl != NULL) {
        struct msg_to_room_t msg_to_room;
        msg_to_room.num = poknum + MAX_CLIENT_NUM;
        msg_to_room.buf.comand = REFRESH;
        msg_to_room.buf.rs = 0;
        msgsnd(cl->PokMesDis, (void*)(&msg_to_room), sizeof(msg_to_room.buf),
               0);
      }
    } else if (!strncmp(rec_sock.buf, "disconnect", 10)) {
      if (cl != NULL) {
        struct msg_to_room_t msg_to_room;
        msg_to_room.num = poknum + MAX_CLIENT_NUM;
        msg_to_room.buf.comand = DISCONECT;
        msg_to_room.buf.rs = 0;
        log(fd, "md is %d poknum %d\n", cl->PokMesDis, poknum);
        msgsnd(cl->PokMesDis, (void*)(&msg_to_room), sizeof(msg_to_room.buf),
               0);
        cl = NULL;
        *id = next_id(*id);
        rec.id = *id;
        rec.code = MENU;
        send(sock, &rec, sizeof(rec), 0);
      }
    } else if (!strncmp(rec_sock.buf, "call", 4) && rec_sock.id == *id) {
      if (cl != NULL) {
        struct msg_to_room_t msg_to_room;
        msg_to_room.num = poknum + MAX_CLIENT_NUM;
        msg_to_room.buf.comand = ACTION_CALL;
        msg_to_room.buf.rs = 0;
        *id = next_id(*id);
        msgsnd(cl->PokMesDis, (void*)(&msg_to_room), sizeof(msg_to_room.buf),
               0);
      }
    } else if (!strncmp(rec_sock.buf, "fold", 4) && rec_sock.id == *id) {
      if (cl != NULL) {
        struct msg_to_room_t msg_to_room;
        msg_to_room.num = poknum + MAX_CLIENT_NUM;
        msg_to_room.buf.comand = ACTION_FOLD;
        msg_to_room.buf.rs = 0;
        *id = next_id(*id);
        msgsnd(cl->PokMesDis, (void*)(&msg_to_room), sizeof(msg_to_room.buf),
               0);
      }
    } else if (!strncmp(rec_sock.buf, "raise", 5) && rec_sock.id == *id) {
      if (cl != NULL) {
        double x = 0;
        sscanf(rec_sock.buf + 5, "%lf", &x);
        if (x > 0) {
          struct msg_to_room_t msg_to_room;
          msg_to_room.num = poknum + MAX_CLIENT_NUM;
          msg_to_room.buf.comand = ACTION_RAISE;
          msg_to_room.buf.rs = x;
          *id = next_id(*id);
          msgsnd(cl->PokMesDis, (void*)(&msg_to_room), sizeof(msg_to_room.buf),
                 0);
        }
      }
    } else if (!strncmp(rec_sock.buf, "getmoney", 8)) {
      if (cl != NULL) {
        double x = 0;
        sscanf(rec_sock.buf + 8, "%lf", &x);
        if (x > 0) {
          struct msg_to_room_t msg_to_room;
          msg_to_room.num = poknum + MAX_CLIENT_NUM;
          msg_to_room.buf.comand = GETMONEY;
          msg_to_room.buf.rs = x;
          *id = next_id(*id);
          msgsnd(cl->PokMesDis, (void*)(&msg_to_room), sizeof(msg_to_room.buf),
                 0);
        }
      }
    } else if (!strncmp(rec_sock.buf, "putmoney", 8)) {
      if (cl != NULL) {
        double x = 0;
        sscanf(rec_sock.buf + 8, "%lf", &x);
        if (x > 0) {
          struct msg_to_room_t msg_to_room;
          msg_to_room.num = poknum + MAX_CLIENT_NUM;
          msg_to_room.buf.comand = PUTMONEY;
          msg_to_room.buf.rs = x;
          *id = next_id(*id);
          msgsnd(cl->PokMesDis, (void*)(&msg_to_room), sizeof(msg_to_room.buf),
                 0);
        }
      }
    }
  }
  msg_from_serv.comand = SERVER_COMAND_EXIT;
  msg_from_serv.num = num;
  msgsnd(md, (void*)(&msg_from_serv),
         sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);
  return nullptr;
}

void* client(void* arguments) {
  int sock = ((struct client_argumets_t*)arguments)->sock;
  int md = ((struct client_argumets_t*)arguments)->md;
  int num = ((struct client_argumets_t*)arguments)->num;
  struct table_t* tb = ((struct client_argumets_t*)arguments)->tb;
  free(((struct client_argumets_t*)arguments));

  char str[256];
  for (int i = 0; i < 256; i++) {
    str[i] = 0;
  }
  sprintf(str, "log/ServerClientLog%d.log", num);
  int fd = open(str, O_CREAT | O_RDWR | O_APPEND, 0666);

  if (fd < 0) {
    perror(str);
    return nullptr;
  }

  int bytes_read = 0;
  size_t* id = (size_t*)malloc(sizeof(size_t));
  *id = next_id(time(NULL));

  struct recive_t rec_sock;
  for (size_t i = 0; i < 256; i++) {
    rec_sock.buf[i] = 0;
  }
  struct msg_from_room_t msg_recive;
  set_msg_from_room_t(&msg_recive, 0, NULL);

  struct msg_from_server_t msg_from_serv;
  msg_from_serv.num = 0;
  msg_from_serv.comand = 0;

  struct recivesock rec;
  set_recivesock(&rec, 0, NULL);

  rec.code = MENU;
  rec.id = *id;
  send(sock, &rec, sizeof(rec), 0);

  void* argv;

  pthread_t client_get_info_from_client_thread = 0;
  argv = (struct client_get_info_from_client_argumets_t*)malloc(
      sizeof(struct client_get_info_from_client_argumets_t));
  ((struct client_get_info_from_client_argumets_t*)argv)->sock = sock;
  ((struct client_get_info_from_client_argumets_t*)argv)->md = md;
  ((struct client_get_info_from_client_argumets_t*)argv)->num = num;
  ((struct client_get_info_from_client_argumets_t*)argv)->id = id;
  ((struct client_get_info_from_client_argumets_t*)argv)->tb = tb;
  ((struct client_get_info_from_client_argumets_t*)argv)->fd = fd;
  pthread_create(&(client_get_info_from_client_thread), NULL,
                 client_get_info_from_client, argv);

  pthread_t client_get_info_from_server_thread = 0;
  argv = (struct client_get_info_from_server_argumets_t*)malloc(
      sizeof(struct client_get_info_from_server_argumets_t));
  ((struct client_get_info_from_server_argumets_t*)argv)->sock = sock;
  ((struct client_get_info_from_server_argumets_t*)argv)->md = md;
  ((struct client_get_info_from_server_argumets_t*)argv)->num = num;
  ((struct client_get_info_from_server_argumets_t*)argv)->tb = tb;
  ((struct client_get_info_from_server_argumets_t*)argv)->id = id;
  ((struct client_get_info_from_server_argumets_t*)argv)->fd = fd;
  pthread_create(&(client_get_info_from_server_thread), NULL,
                 client_get_info_from_server, argv);

  pthread_join(client_get_info_from_server_thread, NULL);
  pthread_cancel(client_get_info_from_client_thread);
  pthread_join(client_get_info_from_client_thread, NULL);
  close(sock);

  free(id);
  pthread_exit(NULL);
}

int create_room(struct table_t* tb, int md, int num, FILE* file) {
  if (num < COUNT_OF_ROOM) {
    pthread_mutex_lock(&(tb->mut_read_client));
    if (tb->cl[num].PokMesDis == -1) {
      printf("create room №%d\n", num);
      tb->cl[num].PokMesDis = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666);
      if (tb->cl[num].PokMesDis == -1) {
        perror("msgget");
        pthread_mutex_unlock(&(tb->mut_read_client));
        return 1;
      }
      struct poker_argumets_t* argv =
          (struct poker_argumets_t*)malloc(sizeof(struct poker_argumets_t));
      argv->mds = md;
      argv->cl = &(tb->cl[num]);
      argv->tb = tb;
      argv->num = num;
      argv->file = file;
      tb->thread_poker[num] = 0;
      pthread_create(&(tb->thread_poker[num]), NULL, poker, (void*)argv);
    }
    pthread_mutex_unlock(&(tb->mut_read_client));
  }
  return 0;
}



#pragma pack(push, 1)
struct admin_argumets_t {
  int sock;
  int md;
  struct table_t* tb;
};
#pragma pack(pop)

void* admin(void* arguments){
	int sock = ((struct admin_argumets_t*)arguments)->sock;
	int md = ((struct admin_argumets_t*)arguments)->md;
	struct table_t* tb = ((struct admin_argumets_t*)arguments)->tb;
	free(((struct client_argumets_t*)arguments));

	char buf[256];
	for(int i = 0; i < 256; i++){
		buf[i] = 0;
	}

	sprintf(buf, "log/ServerAdmin.log");
    int fd = open(buf, O_CREAT | O_RDWR | O_APPEND, 0666);

    int bytes_read = -1;

	char** argv = NULL;
	size_t argc = 0;


	while(1){
whil:		
		if(argv != NULL){
			for(size_t i = 0; i < argc; i++){
				if(argv[i] != NULL){
					free(argv[i]);
					argv[i] = NULL;
				}
			}
			free(argv);
			argv = NULL;
			argc = 0;
		}

	    bytes_read = recv(sock, &buf, sizeof(buf), 0);
	    if (bytes_read == 0) {
			close(sock);
			pthread_mutex_lock(&(tb->mut_read_client));
			tb->admin = 0;
			pthread_mutex_unlock(&(tb->mut_read_client));
			return nullptr;
	    }

	    argc = 0;
		optind = 1;
		argv = get_argv(buf, &argc);

	    log(fd, "admin: \"%s\"\n", buf);
	    if (!strncmp(buf, "exit", 4)) {
			close(sock);
			pthread_mutex_lock(&(tb->mut_read_client));
			tb->admin = 0;
			pthread_mutex_unlock(&(tb->mut_read_client));
			return nullptr;
	    }
	}
}

#define ADMIN 0
#define CLIENT 1

#pragma pack(push, 1)
struct authentication_t {
  char login[256];
  char password[256];
  int status;
};
#pragma pack(pop)

int start_server(int port, const char* adr, int md, struct table_t* tb) {
  int sock = 0;
  struct sockaddr_in addr;
  int count = 0;

  int listener = socket(AF_INET, SOCK_STREAM, 0);
  if (listener < 0) {
    perror("socket");
    return 1;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(adr);

  if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("bind");
    return 1;
  }

  listen(listener, 1);
  int pid = 0;
  int status = 0;

  int bytes_read = 0;

  struct authentication_t authent_rec;

  while (1) {
 whil:
    sock = accept(listener, NULL, NULL);
    if (sock < 0) {
      perror("Прием входящих подключений");
      return 1;
    }

    bytes_read = recv(sock, &authent_rec, sizeof(authent_rec), 0);

    if (bytes_read == 0) {
      goto whil;
    }

    if(!strncmp(authent_rec.login, "admin", 5)){
    	authent_rec.status = ADMIN;
    }else{
    	authent_rec.status = CLIENT;
    }

    send(sock, &authent_rec, sizeof(authent_rec), 0);

    if(authent_rec.status == CLIENT){
	    int k = -1;
	    pthread_mutex_lock(&(tb->mut_read_client));
	    for (int i = 1; i < MAX_CLIENT_NUM; i++) {
	      if (tb->arr[i] == 0) {
	        tb->arr[i] = 1;
          snprintf(tb->login[i], 256, "%s", authent_rec.login);
	        k = i;
	        break;
	      }
	    }
	    pthread_mutex_unlock(&(tb->mut_read_client));
	    if (k > 0) {
	      struct client_argumets_t* argv =
	          (struct client_argumets_t*)malloc(sizeof(struct client_argumets_t));
	      argv->sock = sock;
	      argv->md = md;
	      argv->num = k;
	      argv->tb = tb;
	      tb->thread_client[k] = 0;
	      pthread_create(&(tb->thread_client[k]), NULL, client, (void*)argv);
	      pthread_detach(tb->thread_client[k]);
	    }else{
	      close(sock);
	    }
	}else if(authent_rec.status == ADMIN){

		  int flag = 0;

		  pthread_mutex_lock(&(tb->mut_read_client));
		  if(tb->admin == 0){
		  	flag = 1;
		  	tb->admin = 1;
		  }
		  pthread_mutex_unlock(&(tb->mut_read_client));

		  if(flag == 1){
			  struct admin_argumets_t* argv =
		          (struct admin_argumets_t*)malloc(sizeof(struct admin_argumets_t));
		      argv->sock = sock;
		      argv->md = md;
		      argv->tb = tb;
		      tb->thread_admin = 0;
		      pthread_create(&(tb->thread_admin), NULL, admin, (void*)argv);
		      pthread_detach(tb->thread_admin);
	      }else{
	      	close(sock);
	      }
	}

  }
  return 0;
}

#pragma pack(push, 1)
struct get_comand_server_argumets_t {
  int md;
  struct table_t* tb;
  const char* pwd;
};
#pragma pack(pop)

static int get_room_log_str(int num, struct table_t* tb, char* parslog,
                            size_t len) {
  if (num < COUNT_OF_ROOM) {
    char str[256];
    for (int i = 0; i < 256; i++) str[i] = 0;
    sprintf(str, "log/PokerRoomLog%d.log", num);
    int fd = open(str, O_RDONLY, 0666);
    if (fd < 0) {
      return 1;
    }
    long pos = lseek(fd, tb->cl[num].offset, SEEK_SET);
    char buff[256];
    int ret = 0;
    size_t j = 0;
    while ((ret = read(fd, buff, sizeof(buff) - 1)) > 0) {
      buff[ret] = 0;
      size_t i = 0;
      while (buff[i] != 0) {
        str[j] = buff[i];
        if (str[j] == '\n') {
          str[j] = 0;
          if (j >= len) {
            if (!strncmp(str, parslog, len)) {
              printf("%s\n", str);
            }
          }
          j = -1;
        }
        i++;
        j++;
      }
    }
    close(fd);
  }
  return 0;
}

static int show_room(int num, struct table_t* tb) {
  if (num < COUNT_OF_ROOM) {
    char str[256];
    for (int i = 0; i < 256; i++) str[i] = 0;
    sprintf(str, "log/PokerRoomLog%d.log", num);
    int fd = open(str, O_RDONLY, 0666);
    if (fd < 0) {
      return 1;
    }
    long pos = lseek(fd, tb->cl[num].offset, SEEK_SET);
    char buff[256];
    int ret = 0;
    while ((ret = read(fd, buff, sizeof(buff) - 1)) > 0) {
      buff[ret] = 0;
      printf("%s", buff);
    }
    close(fd);
  }

  return 0;
}

static int show_stat(int num, struct table_t* tb) {
  if (num > COUNT_OF_ROOM) return 1;

  char str[256];
  for (int i = 0; i < 256; i++) str[i] = 0;
  sprintf(str, "log/PokerRoomLog%d.log", num);
  int fd = open(str, O_RDONLY, 0666);
  if (fd < 0) {
    return 1;
  }
  long pos = lseek(fd, tb->cl[num].offset, SEEK_SET);
  char buff[256];
  int ret = 0;
  size_t j = 0;

  // statistic table
  bool exist[6] = {false, false, false, false, false, false};
  char login[6][256];
  double cash[6];
  double max_cash[6] = {0, 0, 0, 0, 0, 0};
  double min_cash[6] = {9999999999.0, 9999999999.0, 9999999999.0,
                        9999999999.0, 9999999999.0, 9999999999.0};
  size_t winer[6] = {0, 0, 0, 0, 0, 0};
  size_t count_of_game[6] = {0, 0, 0, 0, 0, 0};
  double midle_win[6] = {0, 0, 0, 0, 0, 0};
  double midle_lost[6] = {0, 0, 0, 0, 0, 0};
  int rebuy[6] = {0, 0, 0, 0, 0, 0};

  bool end_game = false;
  //---------------

  while ((ret = read(fd, buff, sizeof(buff) - 1)) > 0) {
    buff[ret] = 0;
    size_t i = 0;
    while (buff[i] != 0) {
      str[j] = buff[i];
      if (str[j] == '\n') {
        str[j] = 0;
        if (!strncmp(str, "#connect", 8)) {
          int scanf_i = 0;
          char scanf_login[256];
          double scanf_cash = 0;
          sscanf(str + 8, "%d %s %lf", &scanf_i, scanf_login, &scanf_cash);
          if (scanf_i < 6 && scanf_i >= 0) {
            exist[scanf_i] = true;
            strncpy(login[scanf_i], scanf_login, 256);
            cash[scanf_i] = scanf_cash;
            max_cash[scanf_i] = cash[scanf_i];
            min_cash[scanf_i] = cash[scanf_i];
            midle_lost[scanf_i] = 0;
            midle_win[scanf_i] = 0;
            count_of_game[scanf_i] = 0;
            winer[scanf_i] = 0;
            count_of_game[scanf_i] = 0;
            rebuy[scanf_i] = 0;
          }
        }
        if (!strncmp(str, "#sRESULT", 8)) {
          end_game = true;
        }
        if (!strncmp(str, "#qRESULT", 8)) {
          end_game = false;
        }
        if (!strncmp(str, "#REBUY", 6)) {
          int scanf_num = -1;
          double scanf_cash = -1;
          sscanf(str + 6, "%d %lf", &scanf_num, &scanf_cash);
          if (scanf_num >= 0 && scanf_num <= 6) rebuy[scanf_num]++;
          midle_win[scanf_num] -= scanf_cash;
        }
        if (!strncmp(str, "#RES", 4)) {
          int scanf_i = 0;
          int scanf_place = 0;
          int scanf_comb = 0;
          char scanf_login[256];
          double scanf_cash = 0;
          sscanf(str + 4, "%d %s %d %d %lf", &scanf_i, scanf_login,
                 &scanf_place, &scanf_comb, &scanf_cash);

          if (scanf_cash > cash[scanf_i]) {
            midle_win[scanf_i] += scanf_cash - cash[scanf_i];
          }

          if (scanf_cash < cash[scanf_i]) {
            midle_lost[scanf_i] += cash[scanf_i] - scanf_cash;
          }

          if (scanf_i < 6 && scanf_i >= 0) {
            exist[scanf_i] = true;
            cash[scanf_i] = scanf_cash;
            if (cash[scanf_i] > max_cash[scanf_i])
              max_cash[scanf_i] = cash[scanf_i];
            if (cash[scanf_i] < min_cash[scanf_i])
              min_cash[scanf_i] = cash[scanf_i];
            count_of_game[scanf_i]++;
            if (scanf_place == 1) {
              winer[scanf_i]++;
            }
          }
        }
        j = -1;
      }
      i++;
      j++;
    }
  }
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "login");
  for (int i = 0; i < 6; i++) {
    if (exist[i]) printf("%-10s|", login[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "cash");
  for (int i = 0; i < 6; i++) {
    if (exist[i]) printf("%-10.2lf|", cash[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "rebuy");
  for (int i = 0; i < 6; i++) {
    if (exist[i]) printf("%-10d|", rebuy[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "MAX cash");
  for (int i = 0; i < 6; i++) {
    if (exist[i]) printf("%-10.2lf|", max_cash[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "MIN cash");
  for (int i = 0; i < 6; i++) {
    if (exist[i]) printf("%-10.2lf|", min_cash[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "COUNT all");
  for (int i = 0; i < 6; i++) {
    if (exist[i]) printf("%-10zu|", count_of_game[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "COUNT won");
  for (int i = 0; i < 6; i++) {
    if (exist[i]) printf("%-10zu|", winer[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "won");
  for (int i = 0; i < 6; i++) {
    if (exist[i]) printf("%-10.2lf|", midle_win[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "lost");
  for (int i = 0; i < 6; i++) {
    if (exist[i]) printf("%-10.2lf|", midle_lost[i]);
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "MIDLE won");
  for (int i = 0; i < 6; i++) {
    if (exist[i])
      printf("%-10.2lf|", midle_win[i] / ((double)count_of_game[i]));
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");
  printf("%-10s|", "MIDLE lost");
  for (int i = 0; i < 6; i++) {
    if (exist[i])
      printf("%-10.2lf|", midle_lost[i] / ((double)count_of_game[i]));
  }
  printf("\n");
  printf(
      "---------------------------------------------------------------------"
      "-----------\n");

  close(fd);

  return 0;
}

static const char *newEnv[] = {
	"exit",
	"room",
	"display",
	"kill",
	"delroom",
	"showroomlog",
	"showallclient",
  "showclient",
	"help",
	NULL
};


char* generator(const char* text, int state) {
  static int list_index, len;
  char* name;

  if (!state) {
    list_index = 0;
    len = strlen(text);
  }

  while ((name = (char*)newEnv[list_index++])) {
    if (strncmp(name, text, len) == 0) {
      return strdup(name);
    }
  }

  return nullptr;
}

char** completion(const char* text, int start, int end) {
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, generator);
}

static int print_help() {
  int fd = open("../server/source/help.help", O_RDONLY);
  if (fd >= 0) {
    char buff[256];
    for (int i = 0; i < 256; i++) {
      buff[i] = 0;
    }
    int ret = 0;
    while ((ret = read(fd, buff, sizeof(buff) - 1)) > 0) {
      buff[ret] = 0;
      write(1, buff, ret);
    }

    close(fd);
  }
  return 0;
}


void* get_comand_server(void* arguments){
	int md = ((struct get_comand_server_argumets_t*) arguments)->md;
	struct table_t* tb = ((struct get_comand_server_argumets_t*) arguments)->tb;
	const char* pwd = ((struct get_comand_server_argumets_t*) arguments)->pwd;
	free((struct get_comand_server_argumets_t*) arguments);

	char * buf = NULL;

	struct msg_from_server_t msg_from_serv;
	msg_from_serv.num = 0;
	msg_from_serv.comand = 0;

	char** argv = NULL;
	size_t argc = 0;


	while(1){
whil:		
		if(argv != NULL){
			for(size_t i = 0; i < argc; i++){
				if(argv[i] != NULL){
					free(argv[i]);
					argv[i] = NULL;
				}
			}
			free(argv);
			argv = NULL;
			argc = 0;
		}
		if(buf != NULL){
			free(buf);
			buf = NULL;
		}

		rl_attempted_completion_function = completion;
		buf = readline(pwd);
		add_history(buf);
		argc = 0;
		optind = 1;
		argv = get_argv(buf, &argc);

		if(!strncmp(buf, "exit", 4)){
			for(int i = 0; i < MAX_CLIENT_NUM; i++){
				if(tb->arr[i] != 0){
					msg_from_serv.num = i;
					msg_from_serv.comand = SERVER_COMAND_EXIT;
					msgsnd(md, (void*)(&msg_from_serv), sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);
					tb->arr[i] = 0;
				}
			}
			for(int i = 0; i < COUNT_OF_ROOM; i++){
				if(tb->cl[i].PokMesDis != -1){
					msg_from_serv.num = FROM_SERVER_TO_TABLE + i;
					msg_from_serv.comand = SERVER_COMAND_EXIT;
					msgsnd(md, (void*)(&msg_from_serv), sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);
				}
			}
			for(int i = 0; i < COUNT_OF_ROOM; i++){
				if(tb->cl[i].PokMesDis != -1){
					pthread_join(tb->thread_poker[i], NULL);
				}
			}
			msgctl(md, IPC_RMID, NULL);
			goto out;
		}
		if(argv == NULL){
			perror("get_argv:");
			goto whil;
		}
		if(!strncmp(buf, "display", 7)){
			char* newpwd;
			asprintf(&newpwd, "%sdisplay->", pwd);
			display((void*)tb, md, newpwd);
			free(newpwd);
			newpwd = NULL;
		}
		if(!strncmp(buf, "room", 4)){
			int num = -2;
			FILE* file = NULL;
			int opt = 0;
			optind = 1;
			optarg = NULL;
			while((opt = getopt(argc, argv, "n:f:")) != -1) {
				switch (opt){
					case 'n':
						num = atoi(optarg);
						optarg = NULL;
						break;
					case 'f':
						file = fopen(optarg, "r");
						if(!file)
							perror(optarg);
						optarg = NULL;
						break;
					default:
						break;
				}
			}
			if(num > -1){
				create_room(tb, md, num, file);
			}else if(num == -1){
				for(int num = 0; num < COUNT_OF_ROOM; num++){
					create_room(tb, md, num, NULL);
				}
			}
		}
		if(!strncmp(buf, "kill", 4)){
			int num = -2;
			int opt = 0;
			optind = 1;
			optarg = NULL;
			while((opt = getopt(argc, argv, "n:")) != -1) {
				switch (opt){
					case 'n':
						num = atoi(optarg);
						optarg = NULL;
						break;
					default:
						break;
				}
			}
			if(num >= 0){
				if(tb->arr[num]){
					msg_from_serv.num = num;
					msg_from_serv.comand = SERVER_COMAND_EXIT;
					msgsnd(md, (void*)(&msg_from_serv), sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);
					tb->arr[num] = 0;	
				}
			}else if(num == -1){
				for(int i = 0; i < MAX_CLIENT_NUM; i++){
					if(tb->arr[i]){
						msg_from_serv.num = i;
						msg_from_serv.comand = SERVER_COMAND_EXIT;
						msgsnd(md, (void*)(&msg_from_serv), sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);
						tb->arr[i] = 0;	
					}
				}
			}
		}
		if(!strncmp(buf, "showallclient", 13)){
			printf("\n--------------------SHOW CONNECT PEOPLE--------------------\n");
			for(int i = 0; i < MAX_CLIENT_NUM; i++){
				if(tb->arr[i]){
					printf("%d ", i);
				}
			}
			printf("\n");
		}
    if(!strncmp(buf, "showclient", 10)){
      optind = 1;
      optarg = NULL;
      int opt = 0;
      int num = -1;
      while((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt){
          case 'n':
            num = atoi(optarg);
            optarg = NULL;
            break;
          default:
            break;
        }
      }
      if(num > -1){
        printf("\n------------------------SHOW CLIENT------------------------\n");
        printf("%d:\t%s\n", num, tb->login[num]);
      }
    }
		if(!strncmp(buf, "showroomlog", 11)){
			int num = -1;
			int opt = 0;
			bool flag = true;
			char* str = NULL;
			optind = 1;
			optarg = NULL;
			bool flag_log_stat = false;
			while((opt = getopt(argc, argv, "n:f:sh")) != -1) {
				switch (opt){
					case 'n':
						num = atoi(optarg);
						optarg = NULL;
						break;
					case 'f':
						str = optarg;
						optarg = NULL;
						break;
					case 's':
						flag = false;
						flag_log_stat = true;
						break;
          case 'h':
            printf("showroomlog [-s] [-f str] [-n i] [-h]\n");
            printf("where:\n\t");
            printf("-s : for printing statistic\n\t");
            printf("-f : for finding \'str\' in log\n\t");
            printf("-n : for printing log only for room \'i\'\n");
            goto whil;
            break;
					default:
						break;
				}
			}
			if(flag_log_stat){
				show_stat(num, tb);
			}else if(num >= 0 && flag){
				if(str == NULL){
					show_room(num, tb);
				}else{
					get_room_log_str(num, tb, str, strlen(str));
				}
			}
		}
		if(!strncmp(buf, "delroom", 7)){
			int num = -2;
			int opt = 0;
			optind = 1;
			optarg = NULL;
			while((opt = getopt(argc, argv, "n:")) != -1) {
				switch (opt){
					case 'n':
						num = atoi(optarg);
						optarg = NULL;
						break;
					default:
						break;
				}
			}
			if(num >= 0 ){
				if(num < COUNT_OF_ROOM){
					if(tb->cl[num].PokMesDis != -1){
						msg_from_serv.num = FROM_SERVER_TO_TABLE + num;
						msg_from_serv.comand = SERVER_COMAND_EXIT;
						msgsnd(md, (void*)(&msg_from_serv), sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);
						pthread_join(tb->thread_poker[num], NULL);
					}
				}
				printf("room № %d deleted\n", num);
			}else if(num == -1){
				for(int i = 0; i < COUNT_OF_ROOM; i++){
					if(tb->cl[i].PokMesDis != -1){
						msg_from_serv.num = FROM_SERVER_TO_TABLE + i;
						msg_from_serv.comand = SERVER_COMAND_EXIT;
						msgsnd(md, (void*)(&msg_from_serv), sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);
						pthread_join(tb->thread_poker[i], NULL);
					}
				}
				printf("allroom deleted\n");
			}
		}
		if(!strncmp(buf, "help", 4)){
			print_help();
		}
	}

out:
  if (argv != NULL) {
    for (size_t i = 0; i < argc; i++) {
      if (argv[i]) {
        free(argv[i]);
        argv[i] = NULL;
      }
    }
    free(argv);
    argv = NULL;
  }
  if (buf != NULL) {
    free(buf);
    buf = NULL;
  }

  pthread_exit(NULL);
}

int main(int argc, char** argv) {

  int given_port = 3425;
  char* given_addres = (char*)"127.0.0.1";

  optind = 1;
  optarg = NULL;
  int opt = 0;
  while((opt = getopt(argc, argv, "p:a:")) != -1) {
    switch (opt){
      case 'p':
        given_port = atoi(optarg);
        optarg = NULL;
        break;
      case 'a':
        given_addres = optarg;
        break;
      default:
        break;
    }
  }

  int md = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666);
  if (md == -1) {
    perror("msgget in main");
    return 1;
  }
  int sd = shm_open("my_shared_memory", O_CREAT | O_RDWR, 0666);

  if (sd == -1) {
    perror("shm_open");
    return 1;
  }

  int pid_server = -1;
  if (ftruncate(sd, sizeof(struct table_t)) != 0) {
    perror("ftruncate");
    shm_unlink("my_shared_memory");
    return 1;
  }

  struct table_t* tb = (struct table_t*)mmap(
      NULL, sizeof(struct table_t), PROT_WRITE | PROT_READ, MAP_SHARED, sd, 0);

  if (pthread_mutex_init(&(tb->mut_read_client), NULL)) {
    perror("tb->mut_read_client - init");
    return 1;
  }

  for (int j = 0; j < COUNT_OF_ROOM; j++) {
    tb->cl[j].count = 0;
    tb->cl[j].fd_poker_log = -1;
    tb->cl[j].PokMesDis = -1;
    tb->thread_poker[j] = 0;
    for (int i = 0; i < 6; i++) tb->cl[j].num[i] = 0;
  }

  for (int i = 0; i < MAX_CLIENT_NUM; i++) {
    tb->arr[i] = 0;
    tb->thread_client[i] = 0;
  }

  pid_server = fork();

  if (!pid_server) {
    return start_server(given_port, given_addres, md, tb);
  }

  void* argv_for_send = (struct get_comand_server_argumets_t*)malloc(
      sizeof(struct get_comand_server_argumets_t));
  ((struct get_comand_server_argumets_t*)argv_for_send)->md = md;
  ((struct get_comand_server_argumets_t*)argv_for_send)->tb = tb;
  const char pwd[] = "->";
  ((struct get_comand_server_argumets_t*)argv_for_send)->pwd = pwd;

  pthread_t thread = 0;
  pthread_create(&(thread), NULL, get_comand_server, (void*)argv_for_send);
  pthread_join(thread, NULL);

  kill(pid_server, SIGKILL);
  int status = 0;
  if (pid_server != -1) wait(&status);
  pthread_mutex_destroy(&(tb->mut_read_client));

  shm_unlink("my_shared_memory");
  return 0;
}
