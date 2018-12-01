#pragma once 
#include "neyron/web.h"

class bot{
public:
	class web* PreFlop;
	class web* Flop;
	class web* Turn;
	class web* River;

	double LastAns;

	bot(size_t inSize, size_t hidSize, size_t hid2Size, size_t outSize);

	double action(size_t num, double* arr);
	double result(size_t num, double* arr);

	double arr[100024][22];
	double ans[100024];
	size_t num[100024];
	size_t act[100024];
	size_t count;
	size_t inSize;

	int log(double* arr, size_t num, double  ans, size_t act);
	int reset_log();

	int learn(size_t num, double* arr, double ans);
	int fit(size_t num);
	int save();
	int load();
	//FILE* flog;
};
