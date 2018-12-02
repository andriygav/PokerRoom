#include "display.h"
#include "play_info.h"

#define MAX_CLIENT_NUM 2048
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

#define COUNT_OF_ROOM 50
#define SEND_INFO_TO_CLIENT 1
#define DISCONNECT_CLIENT 2

#define SERVER_COMAND_EXIT 100001
#define SERVER_DESTROY_ROOM 100002
#define SERVER_COMAND_SHOW_INFO 100003
#define SERVER_COMAND_DONT_SHOW_INFO 100004
#define SERVER_COMAND_SEND_ALL_INFO 100005

#define FROM_SERVER_TO_TABLE 1000000

// SCREEN OPTION
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_BPP 32

static SDL_Surface* load_image(const char* filename) {
  SDL_Surface* load = SDL_LoadBMP(filename);
  if (load != NULL) {
    SDL_Surface* optimal = SDL_DisplayFormat(load);
    SDL_FreeSurface(load);
    Uint32 colorkey = SDL_MapRGB(optimal->format, 128, 128, 0);
    SDL_SetColorKey(optimal, SDL_SRCCOLORKEY, colorkey);
    return optimal;
  }
  return nullptr;
}

static void apply_surface(int x, int y, SDL_Surface* source,
                          SDL_Surface* dest) {
  SDL_Rect offset;
  offset.x = x;
  offset.y = y;
  SDL_BlitSurface(source, NULL, dest, &offset);
  return;
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
  pthread_t thread_poker[COUNT_OF_ROOM];
  pthread_t thread_client[MAX_CLIENT_NUM];
  pthread_mutex_t mut_read_client;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct msg_from_server_t {
  long num;
  long comand;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct msg_from_room_t {
  long num;
  long comand;
  struct sendinf inf;
};
#pragma pack(pop)

class display {
 public:
  SDL_Surface* screen;
  SDL_Surface* back;
  TTF_Font* font;
  SDL_Surface* card_texture[54];
  int md;
  int show();
  pthread_mutex_t* mut_sdl;
  display(int, int, pthread_mutex_t*);
  ~display();
  struct publplayinfo pupi[6];
  struct publtableinfo puti;

  pthread_t reader_thread;
  int final_table[6];
  char buf[256];
};

static int ret_x(int i) {
  int x = 0;
  switch (i) {
    case 0:
      x = 240;
      break;
    case 5:
      x = 10;
      break;
    case 1:
      x = 470;
      break;
    case 4:
      x = 10;
      break;
    case 2:
      x = 470;
      break;
    case 3:
      x = 240;
      break;
    case 6:
      x = 145;
      break;
  }
  return x;
}

static int ret_y(int i) {
  int y = 0;
  switch (i) {
    case 0:
      y = 60;
      break;
    case 5:
      y = 170;
      break;
    case 1:
      y = 170;
      break;
    case 4:
      y = 375;
      break;
    case 2:
      y = 375;
      break;
    case 3:
      y = 430;
      break;
    case 6:
      y = 275;
      break;
  }
  return y;
}

int display::show() {
  apply_surface(0, 0, this->back, this->screen);
  SDL_Color text_color = {0, 0, 255};
  SDL_Surface* text_surf = NULL;
  SDL_Rect offset = {0, 0, 0, 0};

  //Вывод номера комнаты
  sprintf(this->buf, "ROOM NUMBER IS %d", this->puti.num);
  text_surf = TTF_RenderText_Solid(this->font, this->buf, text_color);
  apply_surface(5, 5, text_surf, this->screen);
  SDL_FreeSurface(text_surf);
  text_color.r = 0;
  text_color.g = 0;
  text_color.b = 0;
  if (this->final_table[0] != 0) {
    for (int i = 0; i < 6; i++) {
      sprintf(this->buf, "%s - %d  %d", this->pupi[i].login, this->pupi[i].comb,
              this->final_table[i]);
      text_surf = TTF_RenderText_Solid(this->font, this->buf, text_color);
      apply_surface(605, 5 + (i + 1) * 25, text_surf, this->screen);
      SDL_FreeSurface(text_surf);
    }
  }

  //Вывод карт

  for (int i = 0; i < 6; i++) {
    text_color.r = 0;
    text_color.g = 0;
    text_color.b = 0;

    apply_surface(ret_x(i), ret_y(i), this->card_texture[53], this->screen);
    apply_surface(ret_x(i) + 62 + 2, ret_y(i), this->card_texture[53],
                  this->screen);

    if (this->pupi[i].card[0] != -1) {
      apply_surface(ret_x(i) + 2, ret_y(i) + 2,
                    this->card_texture[this->pupi[i].card[0]], this->screen);
      apply_surface(ret_x(i) + 62 + 2 + 2, ret_y(i) + 2,
                    this->card_texture[this->pupi[i].card[1]], this->screen);
    }
    //Вывод информации об игроке
    if (this->pupi[i].status == STATUS_ACTIVE) {
      text_color.r = 255;
      text_color.g = 0;
      text_color.b = 0;
    }
    if (this->final_table[i] == 1) {
      text_color.r = 0;
      text_color.g = 255;
      text_color.b = 0;
    }

    text_surf =
        TTF_RenderText_Solid(this->font, this->pupi[i].login, text_color);
    SDL_GetClipRect(text_surf, &offset);
    apply_surface(ret_x(i) + (130 - offset.w) / 2, ret_y(i) - 30, text_surf,
                  this->screen);
    SDL_FreeSurface(text_surf);

    sprintf(this->buf, "%.1lf", this->pupi[i].cash);
    text_surf = TTF_RenderText_Solid(this->font, this->buf, text_color);
    SDL_GetClipRect(text_surf, &offset);
    apply_surface(ret_x(i) + (130 - offset.w) / 2, ret_y(i) + 95, text_surf,
                  this->screen);
    SDL_FreeSurface(text_surf);

    sprintf(this->buf, "(%.1lf)", this->pupi[i].bet);
    text_surf = TTF_RenderText_Solid(this->font, this->buf, text_color);
    SDL_GetClipRect(text_surf, &offset);
    apply_surface(ret_x(i) + (130 - offset.w) / 2, ret_y(i) + 95 + 25,
                  text_surf, this->screen);
    SDL_FreeSurface(text_surf);
  }
  text_color.r = 0;
  text_color.g = 0;
  text_color.b = 0;
  int x = ret_x(6);
  int y = ret_y(6);

  for (int i = 0; i < 5; i++) {
    apply_surface(x + i * 64, y, this->card_texture[53], this->screen);
    if (this->puti.card[i] != 0)
      apply_surface(x + 2 + i * 64, y + 2,
                    this->card_texture[this->puti.card[i]], this->screen);
  }
  sprintf(this->buf, "%.1lf", this->puti.bank);
  text_surf = TTF_RenderText_Solid(this->font, this->buf, text_color);
  SDL_GetClipRect(text_surf, &offset);
  apply_surface(x + (318 - offset.w) / 2, y + 95, text_surf, this->screen);
  SDL_FreeSurface(text_surf);

  sprintf(this->buf, "%.1lf", this->puti.bet);
  text_surf = TTF_RenderText_Solid(this->font, this->buf, text_color);
  SDL_GetClipRect(text_surf, &offset);
  apply_surface(x + (318 - offset.w) / 2, y - 30, text_surf, this->screen);
  SDL_FreeSurface(text_surf);

  SDL_Flip(this->screen);
  return 0;
}

static void* reader(void* arg) {
  class display* scene = (class display*)arg;

  struct msg_from_room_t msg_recive;

  pthread_mutex_lock(scene->mut_sdl);
  scene->show();
  pthread_mutex_unlock(scene->mut_sdl);

  while (1) {
    if (msgrcv(scene->md, (void*)(&msg_recive), sizeof(msg_recive),
               7 + MAX_CLIENT_NUM, 0) == -1) {
      perror("display reader");
      return nullptr;
    }
    scene->puti.bank = msg_recive.inf.table.bank;
    scene->puti.blind = msg_recive.inf.table.blind;
    scene->puti.bet = msg_recive.inf.table.bet;
    scene->puti.num = msg_recive.inf.table.num;
    for (int i = 0; i < 5; i++) {
      scene->puti.card[i] = msg_recive.inf.table.card[i];
    }

    for (int i = 0; i < 6; i++) {
      scene->pupi[i].cash = msg_recive.inf.player[i].cash;
      scene->pupi[i].last_action = msg_recive.inf.player[i].last_action;
      scene->pupi[i].bet = msg_recive.inf.player[i].bet;
      scene->pupi[i].status = msg_recive.inf.player[i].status;
      scene->pupi[i].blind = msg_recive.inf.player[i].blind;
      scene->pupi[i].card[0] = msg_recive.inf.player[i].card[0];
      scene->pupi[i].card[1] = msg_recive.inf.player[i].card[1];
      scene->pupi[i].comb = msg_recive.inf.player[i].comb;
      strncpy(scene->pupi[i].login, msg_recive.inf.player[i].login,
              sizeof(msg_recive.inf.player[i].login));
    }

    for (int i = 0; i < 6; i++) {
      scene->final_table[i] = msg_recive.inf.final_table[i];
    }
    pthread_mutex_lock(scene->mut_sdl);
    if (scene->screen != NULL) {
      scene->show();
      pthread_mutex_unlock(scene->mut_sdl);
    } else {
      pthread_mutex_unlock(scene->mut_sdl);
      return nullptr;
    }
  }
  return nullptr;
}

display::display(int num, int mesd, pthread_mutex_t* mut) {
  this->md = mesd;
  this->mut_sdl = mut;
  if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
    perror("SDL_INIT");
  }

  this->screen =
      SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
  if (this->screen == NULL) {
    perror("create screen");
  }
  if (TTF_Init() == -1) {
    perror("TTF_INIT");
  }

  char name[256];
  for (int i = 0; i < 256; i++) {
    name[i] = 0;
  }
  sprintf(name, "PokerRoom%d", num);
  SDL_WM_SetCaption(name, NULL);

  for (int i = 0; i < 256; i++) this->buf[i] = 0;
  this->back = load_image("../server/VisSource/background.bmp");
  if (this->back == NULL) {
    perror("create screen");
  }
  for (int i = 0; i < 54; i++) {
    sprintf(name, "../server/colum/%d.bmp", i);
    this->card_texture[i] = load_image(name);
    if (this->card_texture[i] == NULL) {
      perror("create screen");
    }
  }

  this->puti.bank = 0;
  this->puti.blind = 0;
  this->puti.bet = 0;
  this->puti.num = 0;
  for (int i = 0; i < 5; i++) {
    this->puti.card[i] = 0;
  }

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

  for (int i = 0; i < 6; i++) {
    this->final_table[i] = 0;
  }

  this->font = TTF_OpenFont("../server/VisSource/font.ttf", 24);
  if (this->font == NULL) perror("fond");
  this->reader_thread = 0;

  pthread_create(&(this->reader_thread), NULL, reader, this);
}

display::~display() {
  pthread_cancel(this->reader_thread);
  pthread_join(this->reader_thread, NULL);
  this->reader_thread = 0;

  for (int i = 0; i < 54; i++) {
    if (this->card_texture[i] != NULL) {
      SDL_FreeSurface(this->card_texture[i]);
    }
  }
  TTF_CloseFont(this->font);
  if (this->screen != NULL) {
    SDL_FreeSurface(this->screen);
  }
  this->screen = NULL;
  SDL_VideoQuit();
  SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
  TTF_Quit();
  SDL_Quit();
}

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

static const char* roomEnv[] = {"exit", NULL};

static const char* displayEnv[] = {"exit", "room", NULL};

static char* room_generator(const char* text, int state) {
  static int list_index, len;
  char* name;

  if (!state) {
    list_index = 0;
    len = strlen(text);
  }

  while ((name = (char*)roomEnv[list_index++])) {
    if (strncmp(name, text, len) == 0) {
      return strdup(name);
    }
  }

  return nullptr;
}

static char** room_completion(const char* text, int start, int end) {
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, room_generator);
}

static void* show_room(int num, struct table_t* tb, int md, const char* pwd) {
  char* buf = NULL;

  pthread_mutex_t mut_sdl;

  pthread_mutex_init(&mut_sdl, NULL);

  class display* scene =
      new class display(num, tb->cl[num].PokMesDis, &mut_sdl);

  struct msg_from_server_t msg_from_serv;
  msg_from_serv.num = num + FROM_SERVER_TO_TABLE;
  msg_from_serv.comand = SERVER_COMAND_SHOW_INFO;
  msgsnd(md, &(msg_from_serv),
         sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);

  while (1) {
    if (buf != NULL) {
      free(buf);
      buf = NULL;
    }
    rl_attempted_completion_function = room_completion;
    buf = readline(pwd);
    add_history(buf);
    if (!strncmp(buf, "exit", 4)) {
      msg_from_serv.num = num + FROM_SERVER_TO_TABLE;
      msg_from_serv.comand = SERVER_COMAND_DONT_SHOW_INFO;
      msgsnd(md, &(msg_from_serv),
             sizeof(msg_from_serv) - sizeof(msg_from_serv.num), 0);
      goto out;
    }
  }

out:
  pthread_mutex_lock(&mut_sdl);
  delete scene;
  pthread_mutex_unlock(&mut_sdl);
  if (buf != NULL) {
    free(buf);
    buf = NULL;
  }
  pthread_mutex_destroy(&mut_sdl);
  return nullptr;
}

static char* dispalay_generator(const char* text, int state) {
  static int list_index, len;
  char* name;

  if (!state) {
    list_index = 0;
    len = strlen(text);
  }

  while ((name = (char*)displayEnv[list_index++])) {
    if (strncmp(name, text, len) == 0) {
      return strdup(name);
    }
  }

  return nullptr;
}

static char** dispalay_completion(const char* text, int start, int end) {
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, dispalay_generator);
}

void* display(void* tableb, int md, const char* pwd) {
  struct table_t* tb = (struct table_t*)tableb;

  char* buf = NULL;

  struct msg_from_server_t msg_from_serv;
  msg_from_serv.num = 0;
  msg_from_serv.comand = 0;

  char** argv = NULL;
  size_t argc = 0;

  while (1) {
  whil:
    if (argv != NULL) {
      for (size_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
          free(argv[i]);
          argv[i] = NULL;
        }
      }
      free(argv);
      argv = NULL;
      argc = 0;
    }
    if (buf != NULL) {
      free(buf);
      buf = NULL;
    }

    rl_attempted_completion_function = dispalay_completion;
    buf = readline(pwd);
    add_history(buf);
    argc = 0;
    argv = get_argv(buf, &argc);

    if (!strncmp(buf, "exit", 4)) {
      goto out;
    }
    if (argv == NULL) {
      perror("get_argv:");
      goto whil;
    }
    if (!strncmp(buf, "room", 4)) {
      int num = -1;
      int opt = 0;
      optind = 1;
      optarg = NULL;
      while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
          case 'n':
            num = atoi(optarg);
            optarg = NULL;
            break;
          default:
            break;
        }
      }
      if (num >= 0) {
        if (tb->cl[num].PokMesDis != -1) {
          char* newpwd;
          asprintf(&newpwd, "%sroom %d->", pwd, num);
          show_room(num, tb, md, newpwd);
          free(newpwd);
          newpwd = NULL;
        } else {
          printf("Recently this room offline\n");
        }
      }
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
    argc = 0;
  }
  if (buf != NULL) {
    free(buf);
    buf = NULL;
  }
  return nullptr;
}
