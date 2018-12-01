#pragma once
//Poker combination
#define COMB_FLASH_ROYAL 1
#define COMB_STRET_FLASH 2
#define COMB_CARE 3
#define COMB_FULL_HOUSE 4
#define COMB_FLASH 5
#define COMB_STRET 6
#define COMB_SET 7
#define COMB_TWO_PAIRS 8
#define COMB_PAIRS 9
#define COMB_HIGTH_CARDS 10
//CARD SUIT
#define SUIT_HEARTS 1
#define SUIT_DIAMONDS 2
#define SUIT_CLUBS 3
#define SUIT_SPADES 4
//CARD VALUE
#define CARD_A 1
#define CARD_2 2
#define CARD_3 3
#define CARD_4 4
#define CARD_5 5
#define CARD_6 6
#define CARD_7 7
#define CARD_8 8
#define CARD_9 9
#define CARD_10 10
#define CARD_J 11
#define CARD_Q 12
#define CARD_K 13

static int ret_higth(int x){
	if(x <= 0)
		return x;
	return ((x%13) + 1);
}
static int ret_mast(int x){
	if(x <= 0)
		return x;
	return (x%13!=0?x:(x-1))/13 + 1;
}

static int find_card(int* arr, size_t size, int mast, int rang){
	for(size_t i = 0; i < size; i++){
		if(ret_higth(arr[i]) == rang && ret_mast(arr[i]) == mast)
			return i + 1;
	}
	return 0;
}

static int in_arr(int x, int* arr, size_t size){
	for(size_t i = 0; i < size; i++){
		if(arr[i] == x){
			return 1;
		}
	}
	return 0;
}

static int swap(int* a, int* b){
	int x = *a;
	*a = *b;
	*b = x;
	return 0;
}

static int max_card(int* arr, size_t size, int* a, size_t asize){
	int mx = 0;	
	for(size_t i = 0; i < size; i++){
		if(!in_arr(arr[i], a, asize)){
			if((ret_higth(arr[i]) == CARD_A || ret_higth(arr[i]) > ret_higth(mx)) && ret_higth(mx) != CARD_A){
				mx = arr[i];
			}
		}
	}
	return mx;
}

int is_FLASH_ROYAL(int* arr, size_t size, int* res){
	for(size_t i = 0; i < size; i++){
		if(ret_higth(arr[i]) == CARD_A){
			int mt = ret_mast(arr[i]);
			int count = 1;
			count+=(find_card(arr,size, mt, CARD_10) > 0)?1:0;
			count+=(find_card(arr,size, mt, CARD_J) > 0)?1:0;
			count+=(find_card(arr,size, mt, CARD_Q) > 0)?1:0;
			count+=(find_card(arr,size, mt, CARD_K) > 0)?1:0;
			if(count == 5){
				if(res != NULL){
					res[0] = arr[find_card(arr,size, mt, CARD_A) - 1];
					res[1] = arr[find_card(arr,size, mt, CARD_K) - 1];
					res[2] = arr[find_card(arr,size, mt, CARD_Q) - 1];
					res[3] = arr[find_card(arr,size, mt, CARD_J) - 1];
					res[4] = arr[find_card(arr,size, mt, CARD_10) - 1];
				}
				return CARD_A;
			}
		}
	}
	return 0;
}

int is_STRET_FLASH(int* arr, size_t size, int* res){
	for(size_t i = CARD_K; i >= CARD_5; i--){
		for(size_t j = 1; j < 4; j++){
			if(find_card(arr,size,j,i)){
				int count = 0;
				for(size_t k = i - 4; k <= i; k++){
					count+=(find_card(arr,size, j, k) > 0)?1:0;
				}
				if(count == 5){
					if(res != NULL){
						int t = 0;
						for(size_t k = i - 4; k <= i; k++){
							res[t] = arr[find_card(arr,size, j, k) - 1];
							t++;
						}
					}
					return i;
				}
			}
		}
	}
	return 0;
}



int is_CARE(int* arr, size_t size, int* res){

	int count = 0;
	for(size_t k = 1; k <= 4; k++){
		count+=(find_card(arr,size, k, CARD_A) > 0)?1:0;
	}
	if(count == 4){
		if(res != NULL){
			int t = 0;
			for(size_t k = 1; k <= 4; k++){
				res[t] = arr[find_card(arr,size, k, CARD_A) - 1];
				t++;
			}
			res[4] = max_card(arr, size, res, 4);		
		}
		return CARD_A;
	}


	for(size_t i = CARD_K; i >= CARD_2; i--){
		int count = 0;
		for(size_t k = 1; k <= 4; k++){
			count+=(find_card(arr,size, k, i) > 0)?1:0;
		}
		if(count == 4){
			if(res != NULL){
				int t = 0;
				for(size_t k = 1; k <= 4; k++){
					res[t] = arr[find_card(arr,size, k, i) - 1];
					t++;
				}
				res[4] = max_card(arr, size, res, 4);		
			}
			return i;
		}
	}

	return 0;
}

int is_FULL_HOUSE(int* arr, size_t size, int* res){
	int thri = 0;
	int two = 0;

	int count = 0;
	for(size_t k = 1; k <= 4; k++){
		count+=(find_card(arr,size, k, CARD_A) > 0)?1:0;
	}
	if(count == 3 && thri == 0){
		thri = CARD_A;
	}else if(count > 1){
		two = CARD_A;
	}

	for(size_t i = CARD_K; i >= CARD_2; i--){
		int count = 0;
		for(size_t k = 1; k <= 4; k++){
			count+=(find_card(arr,size, k, i) > 0)?1:0;
		}
		if(count == 3 && thri == 0){
			thri = i;
		}else if(count > 1){
			two = i;
		}
		if(thri && two){
			if(res != NULL){
				int t = 0;
				for(size_t k = 1; k <= 4; k++){
					int tmp = find_card(arr,size, k, thri);
					if(tmp && t < 5){
						res[t] = arr[tmp - 1];
						t++;
					}
				}
				for(size_t k = 1; k <= 4; k++){
					int tmp = find_card(arr,size, k, two);
					if(tmp && t < 5){
						res[t] = arr[tmp - 1];
						t++;
					}
				}	
			}
			return thri;
		}
	}
	return 0;
}

int is_FLASH(int* arr, size_t size, int* res){
	for(size_t i = 1; i <=4; i++){
		int count = 0;
		int max = 0;
		for(size_t j = CARD_A; j <= CARD_K; j++){
			int t = find_card(arr,size, i, j);
			count+=(t > 0)?1:0;
			if(t && max != 1)
				max = j;
		}
		if(count >= 5){
			if(res != NULL){
				int t = 0;
				int tmp = find_card(arr,size, i, CARD_A);
				if(tmp){
					res[t] =  arr[tmp-1];
					t++;
				}
				for(size_t j = CARD_K; j >= CARD_2; j--){
					int tmp = find_card(arr,size, i, j);
					if(tmp && t < 5){
						res[t] =  arr[tmp-1];
						t++;
					}
				}
			}
			return max;
		}
	}
	return 0;
}

int is_STRET(int* arr, size_t size, int* res){
	int count = 0;
	for(size_t k = CARD_10; k <= CARD_K; k++){
		int tcount = 0;
		for(size_t j = 1; j <= 4; j++){
			tcount += (find_card(arr,size, j, k) > 0)?1:0;
		}
		count+=((tcount==0)?0:1);
	}
	if(count == 5){
		if(res != NULL){
			size_t t = 0;
			for(size_t i = 1; i <=4; i++){
				int tmp = find_card(arr, size, i, CARD_A);
				if(tmp && t < 5){
					res[t] = arr[tmp-1];
					t++;
					break;
				}
			}
			for(size_t k = CARD_K; k >= CARD_10; k--){
				for(size_t j = 1; j <= 4; j++){
					int tmp = find_card(arr,size, j, k);
					if(tmp && t < 5){
						res[t] = arr[tmp - 1];
						t++;
						break;
					}				
				}
			}
			
		}
		return CARD_A;
	}

	for(size_t i = CARD_K; i >= CARD_5; i--){
		int count = 0;
		for(size_t k = i - 4; k <= i; k++){
			int tcount = 0;
			for(size_t j = 1; j <= 4; j++){
				tcount += (find_card(arr,size, j, k) > 0)?1:0;
			}
			count+=((tcount==0)?0:1);
		}
		if(count == 5){
			if(res != NULL){
				size_t t = 0;
				for(size_t k = i; k >= i - 4; k--){
					for(size_t j = 1; j <= 4; j++){
						int tmp = find_card(arr,size, j, k);
						if(tmp && t < 5){
							res[t] = arr[tmp - 1];
							t++;
							break;
						}
					}
				}
			}
			return i;
		}
	}
	return 0;
}

int is_SET(int* arr, size_t size, int* res){
	int count = 0;
	for(size_t k = 1; k <= 4; k++){
		count+=(find_card(arr,size, k, CARD_A) > 0)?1:0;
	}
	if(count == 3){
		if(res != NULL){
			int t = 0;
			for(size_t k = 1; k <= 4; k++){
				int tmp = find_card(arr,size, k, CARD_A);
				if(tmp && t < 5){
					res[t] = arr[tmp - 1];
					t++;
				}
			}
			res[3] = max_card(arr, size, res, 3);
			res[4] = max_card(arr, size, res, 4);		
		}
		return CARD_A;
	}

	for(size_t i = CARD_K; i >= CARD_2; i--){
		int count = 0;
		for(size_t k = 1; k <= 4; k++){
			count+=(find_card(arr,size, k, i) > 0)?1:0;
		}
		if(count == 3){
			if(res != NULL){
				int t = 0;
				for(size_t k = 1; k <= 4; k++){
					int tmp = find_card(arr,size, k, i);
					if(tmp && t < 5){
						res[t] = arr[tmp - 1];
						t++;
					}
				}
				res[3] = max_card(arr, size, res, 3);
				res[4] = max_card(arr, size, res, 4);	
			}
			return i;
		}
	}
	return 0;
}

int is_TWO_PAIRS(int* arr, size_t size, int* res){
	int first = 0;
	int second = 0;

	int count = 0;
	for(size_t k = 1; k <= 4; k++){
		count+=(find_card(arr,size, k, CARD_A) > 0)?1:0;
	}
	if(count > 1 && first == 0){
		first = CARD_A;
	}else if(count > 1){
		second = CARD_A;
	}

	for(size_t i = CARD_K; i >= CARD_2; i--){
		int count = 0;
		for(size_t k = 1; k <= 4; k++){
			count+=(find_card(arr,size, k, i) > 0)?1:0;
		}
		if(count > 1 && first == 0){
			first = i;
		}else if(count > 1){
			second = i;
		}
		if(first && second){
			if(second == CARD_A || second > first){
				swap(&first, &second);
			}
			if(res != NULL){
				int t = 0;
				for(size_t k = 1; k <= 4; k++){
					int tmp = find_card(arr,size, k, first);
					if(tmp && t < 5){
						res[t] = arr[tmp - 1];
						t++;
					}
				}
				for(size_t k = 1; k <= 4; k++){
					int tmp = find_card(arr,size, k, second);
					if(tmp && t < 5){
						res[t] = arr[tmp - 1];
						t++;
					}
				}
				res[4] = max_card(arr, size, res, 4);
			}
			return first;
		}
	}
	return 0;
}

int is_PAIRS(int* arr, size_t size, int* res){
	int count = 0;
	for(size_t k = 1; k <= 4; k++){
		count+=(find_card(arr,size, k, CARD_A) > 0)?1:0;
	}
	if(count > 1){
		if(res != NULL){
			int t = 0;
			for(size_t k = 1; k <= 4; k++){
				int tmp = find_card(arr,size, k, CARD_A);
				if(tmp && t < 5){
					res[t] = arr[tmp - 1];
					t++;
				}
			}
			res[2] = max_card(arr, size, res, 2);
			res[3] = max_card(arr, size, res, 3);
			res[4] = max_card(arr, size, res, 4);
		}
		return CARD_A;
	}

	for(size_t i = CARD_K; i >= CARD_2; i--){
		int count = 0;
		for(size_t k = 1; k <= 4; k++){
			count+=(find_card(arr,size, k, i) > 0)?1:0;
		}
		if(count > 1){
			if(res != NULL){
				int t = 0;
				for(size_t k = 1; k <= 4; k++){
					int tmp = find_card(arr,size, k, i);
					if(tmp && t < 5){
						res[t] = arr[tmp - 1];
						t++;
					}
				}
				res[2] = max_card(arr, size, res, 2);
				res[3] = max_card(arr, size, res, 3);
				res[4] = max_card(arr, size, res, 4);
			}
			return i;
		}
	}
	return 0;
}

int is_HIGTH_CARDS(int* arr, size_t size, int* res){
	res[0] = max_card(arr, size, res, 0);
	res[1] = max_card(arr, size, res, 1);
	res[2] = max_card(arr, size, res, 2);
	res[3] = max_card(arr, size, res, 3);
	res[4] = max_card(arr, size, res, 4);
	return res[0];
}

int get_comb_card(int* arr, int size, int* a, int comb){
	if(comb == COMB_FLASH_ROYAL)
		return is_FLASH_ROYAL(arr, size, a);
	if(comb == COMB_STRET_FLASH)
		return is_STRET_FLASH(arr, size, a);
	if(comb == COMB_CARE)
		return is_CARE(arr, size, a);
	if(comb == COMB_FULL_HOUSE)
		return is_FULL_HOUSE(arr, size, a);
	if(comb == COMB_FLASH)
		return is_FLASH(arr, size, a);
	if(comb == COMB_STRET)
		return is_STRET(arr, size, a);
	if(comb == COMB_SET)
		return is_SET(arr, size, a);
	if(comb == COMB_TWO_PAIRS)
		return is_TWO_PAIRS(arr, size, a);
	if(comb == COMB_PAIRS)
		return is_PAIRS(arr, size, a);
	if(comb == COMB_HIGTH_CARDS)
		return is_HIGTH_CARDS(arr, size, a);
	return 0;
}
