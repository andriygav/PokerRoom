#include "neyron.h"
#include <time.h>

static double my_rand(){
	double x = 1 + ((double)rand());
	double y = 1 + ((double)rand());
	return x>y?(y/x + 1):(x/y + 1);	
}

neyr::neyr(size_t size, size_t fl){
	this->input = 0;
	this->output = 0;
	this->delta = 0;
	this->weight = new double[size];
	this->dweight = new double[size];
	for(int i = 0; i < size; i++){
		this->weight[i] = 1;
		this->dweight[i] = 0;
	}
}

neyr::neyr(size_t size){
	this->input = 0;
	this->output = 0;
	this->delta = 0;
	this->weight = new double[size];
	this->dweight = new double[size];
	struct timespec tmp;
	clock_gettime(CLOCK_REALTIME, &tmp);
	srand((unsigned int)tmp.tv_nsec);
	for(int i = 0; i < size; i++){
		this->weight[i] = my_rand();
		this->dweight[i] = 0;
	}

	

}
