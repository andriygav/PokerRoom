#include <stdlib.h>
#include <stdio.h>

//Poker combination
#define COMB_FLASH_ROYAL 117
#define COMB_STRET_FLASH 104
#define COMB_CARE 91
#define COMB_FULL_HOUSE 78
#define COMB_FLASH 65
#define COMB_STRET 52
#define COMB_SET 39
#define COMB_TWO_PAIRS 26
#define COMB_PAIRS 13
#define COMB_HIGTH_CARDS 10
//CARD SUIT
#define SUIT_HEARTS 1
#define SUIT_DIAMONDS 2
#define SUIT_CLUBS 3
#define SUIT_SPADES 4
//CARD VALUE
#define CARD_2 0
#define CARD_3 1
#define CARD_4 2
#define CARD_5 3
#define CARD_6 4
#define CARD_7 5
#define CARD_8 6
#define CARD_9 7
#define CARD_T 8
#define CARD_J 9
#define CARD_Q 10
#define CARD_K 11
#define CARD_A 12

struct card{
  int num;
  int value;
  int suit;
};

int ret_hight(int x){
  if (x != -1)
    return (x%13);
  return -1;
}
int ret_mast(int x){
  if (x != -1)
    return (x/13);
  return -1;
}

int is_Kicker(struct card a[]){
  return a[0].value;
}

int is_OnePair(struct card a[], int n){
  for (int i=0; i < n-1; i++){
    if (a[i].value == a[i+1].value && (a[i].num != -1))
      return a[i].value;
  }
  return -1;
}

int is_TwoPairs(struct card a[], int n){

  int fir_pair = 0;
  for (int i=0; i < n-1; i++){
    if (a[i].value == a[i+1].value && (a[i].num != -1)){
      fir_pair = a[i].value;
      break;
    }
  }
  for (int i=0; i < n-1; i++){
    if (a[i].value == a[i+1].value && (a[i].num != -1) && (a[i].value != fir_pair))
      return fir_pair;
  }
  return -1;
}

int is_Set(struct card a[], int n){
  for (int i=0; i < n-2; i++){
    if ((a[i].value == a[i+1].value) && (a[i+2].value == a[i+1].value) && (a[i].num != -1))
      return a[i].value;
  }
  return -1;
}

int is_Straight(struct card a[], int n){
  int max = 0, count = 0;
  for (int i=0; i < n - 1; i++){
    if ((a[i].value - a[i+1].value <= 1)){
      count++;
      if (count == 5)
        return a[i-4].value;
    }
    else count = 0;
  }
  return -1;
}

int is_Flush(struct card a[], int n){

  int count[4] = {0,0,0,0};
  for(int i = 0; i < n; i++){
    switch (a[i].suit){
      case 0: count[0]++; break;
      case 1: count[1]++; break;
      case 2: count[2]++; break;
      case 3: count[3]++; break;
    }
  }

  for (int i = 0; i < 4; i++)
    if (count[i] >=5)
      for (int j = 0; j < 3; j++)
        if (a[j].suit == i)
          return a[i].value;

  return -1;
}

int is_FullHouse(struct card a[], int n){
  int set = is_Set(a,n);
  int pair = is_OnePair(a,n);
  if (set && pair && set != pair)
    return set;
  return -1;
}

int is_Quads(struct card a[], int n){
  for (int i=0; i < n-4; i++){
    if ((a[i].value == a[i+1].value) && (a[i+2].value == a[i+1].value) && (a[i+2].value == a[i+3].value) && (a[i].num != -1))
      return a[i].value;
  }
  return -1;
}

int is_StraightFlush(struct card a[], int n){
  int count = 0;
  for (int i=0; i < n - 1; i++){
    if ((a[i].value - a[i+1].value <= 1) && (a[i].suit == a[i+1].suit)){
      count++;
      if (count == 5)
        return a[i-4].value;
    }
    else count = 0;
  }
  return -1;
}

int is_RoyalFlush(struct card a[], int n){

  if(is_Straight(a, n) == CARD_A && is_Flush(a, n) == CARD_A)
    return CARD_A;
  return -1;

}

void sortcard(struct card a[], int n){
  struct card tmp = {0,0,0};
  for (int i = 0; i < n; i++)
    for(int j = n-1; j > i; j--){
      if(a[i].value < a[j].value){
        tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
      }
      if(a[i].value == a[j].value && a[i].num < a[j].num){
        tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
      }
    }
}
char* conv(char* str,int num){
  //char str[2];
  int hei = ret_hight(num);
  switch(hei){
    case CARD_A: sprintf(str, "A"); break;
    case CARD_K: sprintf(str, "K"); break;
    case CARD_Q: sprintf(str, "Q"); break;
    case CARD_J: sprintf(str, "J"); break;
    case CARD_T: sprintf(str, "T"); break;
    case CARD_9: sprintf(str, "9"); break;
    case CARD_8: sprintf(str, "8"); break;
    case CARD_7: sprintf(str, "7"); break;
    case CARD_6: sprintf(str, "6"); break;
    case CARD_5: sprintf(str, "5"); break;
    case CARD_4: sprintf(str, "4"); break;
    case CARD_3: sprintf(str, "3"); break;
    case CARD_2: sprintf(str, "2"); break;
    default: sprintf(str, "-"); break;
  }
  int mast = ret_mast(num);
  switch(mast){
    case 0: sprintf(str+1, "S"); break;
    case 1: sprintf(str+1, "H"); break;
    case 2: sprintf(str+1, "C"); break;
    case 3: sprintf(str+1, "D"); break;
    default: sprintf(str+1, "1"); break;
  }
  //return str;
}

int count_comb(struct card arr[], int n){

  int ot = 0;
  ot = is_RoyalFlush(arr, n);
  if(ot != -1){
    return COMB_FLASH_ROYAL;
  }
  ot = is_StraightFlush(arr, n);
  if(ot != -1){
    return COMB_STRET_FLASH + ot;
  }
  ot = is_Quads(arr, n);
  if(ot != -1){
    return COMB_CARE + ot;
  }
  ot = is_FullHouse(arr, n);
  if(ot != -1){
    return COMB_FULL_HOUSE + ot;
  }
  ot = is_Flush(arr, n);
  if(ot != -1){
    return COMB_FLASH + ot;
  }
  ot = is_Straight(arr, n);
  if(ot != -1){
    return COMB_STRET + ot;
  }
  ot = is_Set(arr, n);
  if(ot != -1){
    return COMB_SET + ot;
  }
  ot = is_TwoPairs(arr, n);
  if(ot != -1){
    return COMB_TWO_PAIRS + ot;
  }
  ot = is_OnePair(arr, n);
  if(ot != -1){
    return COMB_PAIRS + ot;
  }
  return is_Kicker(arr);
}