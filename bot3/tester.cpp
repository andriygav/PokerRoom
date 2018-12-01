#include "NeyronBot.h"
#include <iostream>
#include <unistd.h>

#define BOT_INPUT_NEYRON 22
#define BOT_HIDE_NEYRON 22
#define BOT_HIDE2_NEYRON 11
#define BOT_OUTPUT_NEYRON 1

	double arr[100224][BOT_INPUT_NEYRON];
	double ans[100224];
	size_t num[100224];

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

	FILE* fout = fopen("fout3.txt", "w");


	class bot* boting = new bot(BOT_INPUT_NEYRON, BOT_HIDE_NEYRON, BOT_HIDE2_NEYRON, BOT_OUTPUT_NEYRON);
	//new(boting)(class bot)(BOT_INPUT_NEYRON, BOT_HIDE_NEYRON, BOT_OUTPUT_NEYRON);

	size_t tnum;
	double tarr[22];
	double tans;
	//ANS_BEFORE_LEARN

	fprintf(fansb1,"my real\n");
	fprintf(fansb2,"my real\n");
	fprintf(fansb3,"my real\n");
	fprintf(fansb4,"my real\n");
	fprintf(fansa1,"my real\n");
	fprintf(fansa2,"my real\n");
	fprintf(fansa3,"my real\n");
	fprintf(fansa4,"my real\n");
	for (int j = 0; j < 10000; j++){
		fscanf(fres, "%zu ", &tnum);
		for (int i = 0; i < BOT_INPUT_NEYRON; i++)
			fscanf(fres, "%lf ", &tarr[i]);
		fscanf(fres, "%lf", &tans);
		if(tnum == 1)
			fprintf(fansb1,"%lf %lf\n", boting->result(tnum, tarr), tans);
		if(tnum == 2)
			fprintf(fansb2,"%lf %lf\n", boting->result(tnum, tarr), tans);
		if(tnum == 3)
			fprintf(fansb3,"%lf %lf\n", boting->result(tnum, tarr), tans);
		if(tnum == 4)
			fprintf(fansb4,"%lf %lf\n", boting->result(tnum, tarr), tans);
	}
	
	//rewind(fres);

	//for (int h = 0; h < 10; h++){
		int count = 10000;

		for (int j = 0; j < count; j++){
			fscanf(fres, "%zu ", &num[j]);
			for (int i = 0; i < BOT_INPUT_NEYRON; i++)
				fscanf(fres, "%lf ", &arr[j][i]);
			fscanf(fres, "%lf", &ans[j]);
			if (num[j] == 3){
				for (int i = 0; i < BOT_INPUT_NEYRON; i++)
					fprintf(fout, "%lf ", arr[j][i]);
				fprintf(fout, "%lf\n", ans[j]);
			}
		}
		//FIT
		for (int k = 0; k < 5000; k++){
			for (int i = 0; i < count; i++){
				if (num[i] == 2)
				boting->learn(num[i], arr[i], ans[i]);
			}
		}
		boting->save();
	//	printf("%d - iteration end\n", h);
	//}

	rewind(fres);
	for (int j = 0; j < 100; j++){
		fscanf(fres, "%zu ", &tnum);
		for (int i = 0; i < 22; i++)
			fscanf(fres, "%lf ", &tarr[i]);
		fscanf(fres, "%lf", &tans);
	}

	//ANS_AFTER_LEARN
	for (int j = 0; j < 1000; j++){
		fscanf(fres, "%zu ", &tnum);
		for (int i = 0; i < 22; i++)
			fscanf(fres, "%lf ", &tarr[i]);
		fscanf(fres, "%lf", &tans);

		if(tnum == 1){
			/*for (int i = 0; i < 22; i++)
				fprintf(fansa1, "%lf ", tarr[i]);
			fprintf(fansa1, "===");*/
			fprintf(fansa1,"%lf %lf\n", boting->result(tnum, tarr), tans);
		}
		if(tnum == 2){
			/*for (int i = 0; i < 22; i++)
				fprintf(fansa2, "%lf ", tarr[i]);
			fprintf(fansa2, "===");*/
			fprintf(fansa2,"%lf %lf\n", boting->result(tnum, tarr), tans);
		}
		if(tnum == 3){
			/*for (int i = 0; i < 22; i++)
				fprintf(fansa3, "%lf ", tarr[i]);
			fprintf(fansa3, "===");*/
			fprintf(fansa3,"%lf %lf\n", boting->result(tnum, tarr), tans);
		}
		if(tnum == 4){
			/*for (int i = 0; i < 22; i++)
				fprintf(fansa4, "%lf ", tarr[i]);
			fprintf(fansa4, "===");*/
			fprintf(fansa4,"%lf %lf\n", boting->action(tnum, tarr), tans);
		}
	}
	return 0;
}
