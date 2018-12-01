#pragma once
#include <stdio.h>
#include <stdlib.h>

class neyr{
public:
	double input;
	double* weight;
	double output;
	double* dweight;
	double delta;
	neyr(size_t, size_t);
	neyr(size_t);
};

