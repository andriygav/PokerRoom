#include "NeyronBot.h"
#include <iostream>
#include <unistd.h>

#define BOT_INPUT_NEYRON 15

#define COUNT 1000

int main(){
	FILE* fres = fopen("vector.learn", "r");
	FILE* fansb1 = fopen("fans1_before.txt", "w");
	FILE* fansb2 = fopen("fans2_before.txt", "w");
	FILE* fansb3 = fopen("fans3_before.txt", "w");
	FILE* fansb4 = fopen("fans4_before.txt", "w");
	FILE* fansa1 = fopen("fans1_after.txt", "w");
	FILE* fansa2 = fopen("fans2_after.txt", "w");
	FILE* fansa3 = fopen("fans3_after.txt", "w");
	FILE* fansa4 = fopen("fans4_after.txt", "w");

	class bot* boting = new bot(BOT_INPUT_NEYRON);

	size_t tnum;
	double tarr[BOT_INPUT_NEYRON];
	double tans;

	size_t num[COUNT];
	double arr[COUNT][BOT_INPUT_NEYRON];
	double ans[COUNT];
	int place[COUNT];
	double t_ans;

	//ANS_BEFORE_LEARN
	for (int j = 0; j < COUNT; j++){
		fscanf(fres, "%zu ", &num[j]);
		for (int i = 0; i < BOT_INPUT_NEYRON; i++)
			fscanf(fres, "%lf ", &arr[j][i]);
		fscanf(fres, "%lf %d %lf", &ans[j], &place[j], &t_ans);

		if(num[j] == 1){
			fprintf(fansb1,"%lf %lf\n", boting->result(num[j], arr[j]), ans[j]);
		}
		if(num[j] == 2){
			fprintf(fansb2,"%lf %lf\n", boting->result(num[j], arr[j]), ans[j]);
		}
		if(num[j] == 3){
			fprintf(fansb3,"%lf %lf\n", boting->result(num[j], arr[j]), ans[j]);
		}
		if(num[j] == 4){
			fprintf(fansb4,"%lf %lf\n", boting->result(num[j], arr[j]), ans[j]);
		}
		//printf("\n");
	}
	fclose(fansb1);
	fclose(fansb2);
	fclose(fansb3);
	fclose(fansb4);

	for (int h = 0; h < 1000; h++){
		for (int j = 0; j < COUNT; j++){
			double an = boting->result(num[j], arr[j]);
			//if((place[j] == 1 && an < ans[j]) || (place[j] != 1 && an > ans[j])){
				boting->learn(num[j], arr[j], ans[j]);
			//}
		}
	}
	boting->save();

	//ANS_AFTER_LEARN
	for (int j = 0; j < COUNT; j++){

		if(num[j] == 1){
			fprintf(fansa1,"%lf %lf\n", boting->result(num[j], arr[j]), ans[j]);
		}
		if(num[j] == 2){
			fprintf(fansa2,"%lf %lf\n", boting->result(num[j], arr[j]), ans[j]);
		}
		if(num[j] == 3){
			fprintf(fansa3,"%lf %lf\n", boting->result(num[j], arr[j]), ans[j]);
		}
		if(num[j] == 4){
			fprintf(fansa4,"%lf %lf\n", boting->result(num[j], arr[j]), ans[j]);
		}
	}
	return 0;
}
