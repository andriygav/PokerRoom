#pragma once

#pragma pack(push, 1)
struct publplayinfo {
  char login[256];
  double cash;
  int last_action;
  double bet;
  int status;
  int blind;
  int card[2];
  int comb;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct publtableinfo {
  int card[5];
  double bank;
  double bet;
  double blind;
  int num;
};
#pragma pack(pop)

static int copy_publplayinfo(struct publplayinfo* dest,
                             struct publplayinfo* source) {
  if (dest == NULL || source == NULL) return 1;

  dest->cash = source->cash;
  dest->last_action = source->last_action;
  dest->bet = source->bet;
  dest->status = source->status;
  dest->blind = source->blind;
  dest->card[0] = source->card[0];
  dest->card[1] = source->card[1];
  dest->comb = source->comb;
  return 0;
}

static int copy_publtableinfo(struct publtableinfo* dest,
                              struct publtableinfo* source) {
  if (dest == NULL || source == NULL) return 1;

  for (size_t i = 0; i < 5; i++) dest->card[i] = source->card[i];
  dest->bank = source->bank;
  dest->bet = source->bet;
  dest->blind = source->blind;
  dest->num = source->num;
  return 0;
}

#pragma pack(push, 1)
struct sendinf {
  struct publplayinfo player[6];
  struct publtableinfo table;
  int card[2];
  int num;
  int final_table[6];
};
#pragma pack(pop)

static int set_sendinf(struct sendinf* dest) {
  // set card
  dest->card[0] = 0;
  dest->card[1] = 0;
  dest->num = 0;
  for (int i = 0; i < 6; i++) {
    dest->final_table[i] = 0;
  }
  // set table
  dest->table.bank = 0;
  dest->table.bet = 0;
  dest->table.blind = 0;
  dest->table.num = -1;
  for (int i = 0; i < 5; i++) {
    dest->table.card[i] = 0;
  }
  // set players
  for (int i = 0; i < 6; i++) {
    dest->player[i].cash = 0;
    dest->player[i].last_action = 0;
    dest->player[i].bet = 0;
    dest->player[i].status = 0;
    dest->player[i].blind = 0;
    dest->player[i].card[0] = 0;
    dest->player[i].card[1] = 0;
    dest->player[i].comb = 0;
    for (size_t j = 0; j < sizeof(dest->player[i].login); j++) {
      dest->player[i].login[j] = 0;
    }
  }
  return 0;
}
static int copy_sendinf(struct sendinf* dest, struct sendinf* source) {
  // copy card
  dest->card[0] = source->card[0];
  dest->card[1] = source->card[1];
  dest->num = source->num;
  for (int i = 0; i < 6; i++) {
    dest->final_table[i] = source->final_table[i];
  }
  // copy table
  dest->table.bank = source->table.bank;
  dest->table.bet = source->table.bet;
  dest->table.blind = source->table.blind;
  dest->table.num = source->table.num;
  for (int i = 0; i < 5; i++) {
    dest->table.card[i] = source->table.card[i];
  }
  // copy all players
  for (int i = 0; i < 6; i++) {
    dest->player[i].cash = source->player[i].cash;
    dest->player[i].last_action = source->player[i].last_action;
    dest->player[i].bet = source->player[i].bet;
    dest->player[i].status = source->player[i].status;
    dest->player[i].blind = source->player[i].blind;
    dest->player[i].card[0] = source->player[i].card[0];
    dest->player[i].card[1] = source->player[i].card[1];
    dest->player[i].comb = source->player[i].comb;
    strncpy(dest->player[i].login, source->player[i].login,
            sizeof(source->player[i].login));
  }
  return 0;
}

#pragma pack(push, 1)
struct recivesock {
  long code;
  size_t id;
  struct sendinf inf;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct recive_t {
  char buf[256];
  char login[256];
  size_t id;
};
#pragma pack(pop)

static int set_recivesock(struct recivesock* rec, long code,
                          struct sendinf* inf) {
  rec->code = code;
  rec->id = 0;
  if (inf != NULL) {
    copy_sendinf(&(rec->inf), inf);
  } else {
    set_sendinf(&(rec->inf));
  }
  return 0;
}
