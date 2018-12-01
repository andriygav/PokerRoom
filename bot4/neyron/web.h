#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "neyron.h"

class web{
public:
	neyr* in;

	neyr* meLayer;
	neyr** playerLayer;
	neyr* tableLayer;

	neyr* meNextLayer;
	neyr** playerNextLayer;
	neyr* tableNextLayer;

	neyr* meSumLayer;
	neyr* playerSumLayer;
	neyr* tableSumLayer;

	neyr* hiden;
	neyr* out;

	size_t inSize;

	size_t meInSize;
	size_t playerInSize;
	size_t tableInSize;

	size_t playerCount;

	size_t meSize;
	size_t playerSize;
	size_t tableSize;

	size_t meNextSize;
	size_t playerNextSize;
	size_t tableNextSize;

	size_t meSumSize;
	size_t playerSumSize;
	size_t tableSumSize;

	size_t hidSize;
	size_t outSize;

	web(size_t size[16]);

	void show_web();

	void init(double* arr);

	double sigmoid(double x);
	double fsigmoid(double x);

	void fit(double* arr, double ans);
	void epoh(double* arr, double* ans, size_t count);

	void set_all_player(size_t num);

	double result(double* arr);
	void save(const char* file);
	int load(const char* file);

};
