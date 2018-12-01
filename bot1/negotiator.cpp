#include <pthread.h>
#include <sys/time.h>


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

struct input_train{
	int* card;
	int* circ;
	int N;
};

struct out_train{
	double wins;
};

static int exists_in(int* arr, size_t size, int value){
	for(size_t i = 0; i < size; i++){
		if(arr[i] == value){
			return 1;
		}
	}
	return 0;
}

int set_colum_with(int* card, int* old_card, size_t count, size_t max){
	for(size_t i = 0; i < 52; i++){
		card[i] = 0;	
	}
	for(size_t i = 0; i < count; i++){
		card[i] = old_card[i];
	}
	for(size_t i = count; i < max; i++){
		int c = rand() % 52 + 1;
		while(exists_in(card, 52, c)){
			c = rand() % 52 + 1;
		}
		card[i] = c;
	}
	return 0;
}

void* train_loop(void* arg){

	int* card = ((struct input_train*)arg)->card;
	int* circ = ((struct input_train*)arg)->circ;
	struct card colum[7];
	int Count = ((struct input_train*)arg)->N;
	free((struct input_train*)arg);

	struct timeval time;
	gettimeofday(&time, NULL);
	srand((size_t)time.tv_usec);

	int sums[6] = {0,0,0,0,0,0};
	int equ = 0, flag = 0;
	struct out_train* Wins = (struct out_train*)malloc(sizeof(struct out_train));
	Wins->wins = 0;

	int testcards[7] = {-1, -1, -1, -1, -1, -1, -1};
	for (int i = 2; i < 7; i++){
		testcards[i] = card[i];
	}
	int cr = 0;
	size_t num = 2;

	if (circ[0] == 1){
		num = num + 3;
	}
	if (circ[0] == 2){
		num = num + 4;
	}
	if (circ[0] == 3){
		num = num + 5;
	}

	for (int i = 0; i < Count; i++){
		int cards[52];
		cr = num;
		set_colum_with(cards, testcards, num, 17);

		if (circ[0] == 1){
			testcards[2] = cards[cr++];
			testcards[3] = cards[cr++];
			testcards[4] = cards[cr++];
		}
		if (circ[0] == 2){
			testcards[5] = cards[cr++];
		}
		if (circ[0] == 3){
			testcards[6] = cards[cr++];
		}

		for (int j = 0; j < 5; j++){
			testcards[0] = cards[cr++];
			testcards[1] = cards[cr++];
			create_c(testcards, colum);
			sums[j] = count_comb(colum);
		}
		testcards[0] = card[0];
		testcards[1] = card[1];

		create_c(testcards, colum);
		sums[5] = count_comb(colum);
		for (int j = 0 ;j < 5; j++){
			if (sums[5] > sums[j])
				flag++;
			if (sums[5] == sums[j])
				equ++;
		}
		if (flag == 5)
			Wins->wins += 1/(equ+1);
		flag = 0;
		equ = 0;
	}
	//printf("\n\nwins - %lf\n\n", Wins->wins);
	return Wins;
}



double test(int* card, int* circ, int Count){

	//int testcards[7];
	//printf("start cards:\n");

	//for (int i = 2; i < 7; i++){
	//	testcards[i] = card[i];
		//  printf("%d ", testcards[i]);
	//}
	//printf("\n");
	int i = 0;
	int S_win = 0;
	pthread_t threads[CN];

	for (int k = 0; k < CN; k++){
		struct input_train* input = (struct input_train*)malloc(sizeof(struct input_train));
		if(input != NULL){
			input->card = card;
			input->circ = circ;
			input->N = Count;
			pthread_create(&(threads[k]),NULL, train_loop, (void*)input);
		}else{
				perror("malloc input test");
		}
	}
	//struct out_train* Out;
	void** Out = (void**)malloc(sizeof(void*));
	for(int k = 0; k < CN; k++){
		pthread_join(threads[k], Out);
		if(*Out != NULL){
			//printf("\n\nwins - %lf\n\n", ((struct out_train*)(*Out))->wins);
			S_win += ((struct out_train*)(*Out))->wins;
			free((struct out_train*)(*Out));
		}
	}
	free(Out);
	//printf("i = %d\n",i);
	return S_win/CN;
}

void nul_colum(struct card colum[]){
	for (int j = 0; j < 7; j++){
		colum[j].num = -1;
		colum[j].value = -1;
		colum[j].suit = -1;
	}
}

void create_c(int* testcards, struct card* a){
	for(int i = 0; i < 7; i++){
		a[i].num = testcards[i];
		a[i].suit = ret_mast(a[i].num);
		a[i].value = ret_hight(a[i].num);	
	}
	sortcard(a);
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
	return 0;
}

