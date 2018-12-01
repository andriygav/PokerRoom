
static int log(int md, const char* format, ...){
	char str[256];
	for(int i = 0; i < 256; i++)
		str[i] = 0;
	va_list args;
	va_start(args, format);
	vsprintf(str, format, args);
	va_end(args);	
	if(write(md, str, strnlen(str, 256)) == -1){
		perror("write");
	}
	return 0;
}


int prefl(int card1, int card2){

	//printf("prefl card1 %d, card2 %d\n", card1, card2);

	if ((ret_hight(card1) == ret_hight(card2)) && (ret_hight(card2)>= 3))
		return 2;
	if (ret_hight(card1) >= 10 && ret_hight(card2) >= 10)
		return 2;
	if (((ret_hight(card1) >= 7 && ret_hight(card2) >= 9)|| (ret_hight(card1) >= 9 && ret_hight(card2) >= 7)) && (ret_mast(card1) == ret_mast(card2)))
		return 2;
	if (((ret_hight(card1) >= 5 && ret_hight(card2) >= 6 )|| (ret_hight(card1) >= 6 && ret_hight(card2) >= 5)) \
		&& (ret_mast(card1) == ret_mast(card2)) && (abs(ret_hight(card2) - ret_hight(card1)) == 1 ))
		return 2;

	if ((ret_hight(card1) == ret_hight(card2)) && (ret_hight(card2)>= 0))
		return 1;
	if (ret_hight(card1) == 12 || ret_hight(card2) == 12)
		return 1;
	if ((ret_hight(card1) >= 9 || ret_hight(card2) >= 9)&& (ret_mast(card1) == ret_mast(card2)))
		return 1;
	if (((ret_hight(card1) >= 3 && ret_hight(card2) >= 4) || (ret_hight(card1) >= 4 && ret_hight(card2) >= 3)) && (ret_mast(card1) == ret_mast(card2)))
		return 1;
	if ((ret_hight(card1) >= 1 && ret_hight(card1) <= 4 && ret_hight(card2) >= 2 && ret_hight(card2) >= 4) \
		|| (ret_hight(card2) >= 1 && ret_hight(card2) <= 4 && ret_hight(card1) >= 2 && ret_hight(card1) >= 4)\
		&& (ret_mast(card1) == ret_mast(card2)))
		return 1;
	return 0;

}

void nul_colum(struct card colum[]){
	for (int j = 0; j < 5; j++){
		colum[j].num = -1;
		colum[j].value = -1;
		colum[j].suit = -1;
	}
}

void create_c(int* testcards, struct card* a, int n){
	//struct card* a = (struct card*)malloc(7* sizeof(struct card));
	for(int i = 0; i < n; i++){
		a[i].num = testcards[i];	
	}
		//printf("creted colum\n");
	for (int i = 0; i < n; i++){
		a[i].suit = ret_mast(a[i].num);
		a[i].value = ret_hight(a[i].num); 
		//printf("%d %d %d\n", a[i].value, a[i].suit, a[i].num);
	}
	sortcard(a,n);
}

static int exists_in(int* arr, size_t size, int value){
	for(size_t i = 0; i < size; i++){
		if(arr[i] == value){
			return 1;
		}
	}
	return 0;
}


int set_colum(int* card){
	for(size_t i = 0; i < 52; i++){
		card[i] = 0;	
	}

	for(size_t i = 0; i < 52;i++){
		int c = rand() % 52 + 1;
		while(exists_in(card, 52, c)){
			c = rand() % 52 + 1;
		}
		card[i] = c;
	}
	//this->number_card = 0;
	return 0;
}

int counter(int* col, int* circ){

	struct card cards[7];
	int my_comb = 0, enem_comb = 0, level = 0, n = 0;
	n = 4+circ[0];
	for (int i = 0; i < n; i++)
		cards[i].num = col[i];
	create_c(col, cards, n);
	my_comb = count_comb(cards, n);
	for (int i = 0; i < 52; i++){
		for (int j = 0; j < 52; j++){
			if (!exists_in(col, n, i) && !exists_in(col, n, j)){
				col[0] = i;
				col[1] = j;
				create_c(col, cards, n);

				sortcard(cards, n);
				enem_comb = count_comb(cards, n);

				if (enem_comb > my_comb)
					level -= 1;
				else if (enem_comb < my_comb)
					level += 1;
			}
		}
	}

	//printf("level - %d\n", level);
	if (level > 20)
		return level;
	if (level > 0)
		return -1;
	else 
		return -2;
}

/*
int main(){

	int colum[7];
	for(int i = 0; i < 7; i++){
		scanf("%d",&colum[i]);	
	}
	int num[1] = {0};
	scanf("%d", &num[0]);
	int act = counter(colum, num);
	
	return 0;
}
*/
