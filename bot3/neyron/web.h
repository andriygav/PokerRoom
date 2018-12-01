#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "neyron.h"

class web{
public:
	neyr* in;
	neyr* hiden;
	neyr* hiden2;
	neyr* out;

	size_t inSize;
	size_t hidSize;
	size_t hid2Size;
	size_t outSize;

	web(size_t inSize, size_t hidSize, size_t hid2ize, size_t outSize);
	void show_web();

	void init(double* arr);

	double sigmoid(double x);

	void fit(double* arr, double ans);
	void epoh(double* arr, double* ans, size_t count);

	double result(double* arr);
	void save(const char* file);
	int load(const char* file);

};
