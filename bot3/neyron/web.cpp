#include "web.h"
#include <math.h>
#include <iostream>
#include <unistd.h>

#include "web.h"
#include <math.h>

#define E 0.0313
#define A 0.01
#define HAMMA 0.001

web::web(size_t inSize, size_t hidSize, size_t hid2Size, size_t outSize){
	this->inSize = inSize;
	this->hidSize = hidSize;
	this->hid2Size = hid2Size;
	this->outSize = outSize;

	this->in = (neyr*)malloc(inSize*sizeof(neyr));
	for(int i = 0; i < inSize; i++){
		new(this->in + i)(neyr)(1,1);
	}
	this->hiden = (neyr*)malloc(hidSize*sizeof(neyr));
	for(int i = 0; i < hidSize; i++){
		new(this->hiden + i)(neyr)(inSize);
	}
	this->hiden2 = (neyr*)malloc(hid2Size*sizeof(neyr));
	for(int i = 0; i < hid2Size; i++){
		new(this->hiden2 + i)(neyr)(hidSize);
	}
	this->out = (neyr*)malloc(outSize*sizeof(neyr));
	for(int i = 0; i < outSize; i++){
		new(this->out + i)(neyr)(hid2Size);
	}
}

void web::show_web(){
	std::cout<<"\nINPUT LAYER\n"; 
	for(int i = 0; i < this->inSize; i++){
		std::cout<<"input "<<this->in[i].input<<std::endl;
		std::cout<<"output "<<this->in[i].input<<std::endl;
		std::cout<<"delta "<<this->in[i].delta<<std::endl;
		std::cout<<"weight "<<this->in[i].weight[0]<<std::endl;
		std::cout<<"dweight "<<this->in[i].dweight[0]<<std::endl;
		std::cout<<std::endl;
	}
	std::cout<<"\nHIDEN LAYER\n";
	for(int i = 0; i < this->hidSize; i++){
		std::cout<<"input "<<this->hiden[i].input<<std::endl;
		std::cout<<"output "<<this->hiden[i].output<<std::endl;
		std::cout<<"delta "<<this->hiden[i].delta<<std::endl;
		std::cout<<"weight ";
		for(int j = 0; j < inSize; j++){
			std::cout << this->hiden[i].weight[j]<<" ";
		}
		std::cout<<"\ndweight ";
		for(int j = 0; j < inSize; j++){
			std::cout << this->hiden[i].dweight[j]<<" ";
		}
		std::cout<<std::endl<<std::endl;
	}

	std::cout<<"\nOUTPUT LAYER\n";
	for(int i = 0; i < this->outSize; i++){
		std::cout<<"input "<<this->out[i].input<<std::endl;
		std::cout<<"output "<<this->out[i].output<<std::endl;
		std::cout<<"delta "<<this->out[i].delta<<std::endl;
		std::cout<<"weight ";
		for(int j = 0; j < hidSize; j++){
			std::cout << this->out[i].weight[j]<<" ";
		}
		std::cout<<"\ndweight ";
		for(int j = 0; j < hidSize; j++){
			std::cout << this->out[i].dweight[j]<<" ";
		}
		std::cout<<std::endl<<std::endl;
	}	
	std::cout<<std::endl;
}

void web::init(double* arr){
	for(int i = 0; i < this->inSize; i++){
		this->in[i].input = this->sigmoid(arr[i]);
		this->in[i].output = this->in[i].input;
	}
}

double web::sigmoid(double x){
	double ex = exp(-x) + 1;
	return 2/(ex) - 1 + HAMMA*x;
}

static double fsigmoid(double x){
	double ex = exp(x);
	return (2*ex)/((1+ex)*(1+ex)) + HAMMA;
}

void web::fit(double* arr, double ans){

	double my_ans = this->result(arr);

	for(int i = 0; i < this->outSize; i++){
		this->out[i].delta = (ans - this->out[i].output)*fsigmoid(this->out[i].input);
	}

	for(int i = 0; i < this->hid2Size; i++){
		this->hiden2[i].delta = 0;
		for (int j = 0; j < this->outSize; j++){
			this->hiden2[i].delta = this->hiden2[i].delta + (this->out[j].weight[i] * this->out[j].delta);
		}
		this->hiden2[i].delta = this->hiden2[i].delta * fsigmoid(this->hiden2[i].input);
	}

	for(int i = 0; i < this->hidSize; i++){
		this->hiden[i].delta = 0;
		for (int j = 0; j < this->hid2Size; j++){
			this->hiden[i].delta = this->hiden[i].delta + (this->hiden2[j].weight[i] * this->hiden2[j].delta);
		}
		this->hiden[i].delta = this->hiden[i].delta * fsigmoid(this->hiden[i].input);
	}


	for(int i = 0; i < this->hidSize; i++){
		for (int j = 0; j < this-> inSize; j++){
			this->hiden[i].dweight[j] = E*this->hiden[i].delta*this->in[j].output + A*this->hiden[i].dweight[j];
			this->hiden[i].weight[j] += this->hiden[i].dweight[j];
		}
	}

	for(int i = 0; i < this->hid2Size; i++){
		for (int j = 0; j < this-> hidSize; j++){
			this->hiden2[i].dweight[j] = E*this->hiden2[i].delta*this->hiden[j].output + A*this->hiden2[i].dweight[j];
			this->hiden2[i].weight[j] += this->hiden2[i].dweight[j];
		}
	}


	for(int i = 0; i < this->outSize; i++){
		for (int j = 0; j < this-> hid2Size; j++){
			this->out[i].dweight[j] = E*this->out[i].delta*this->hiden2[j].output + A*this->out[i].dweight[j];
			this->out[i].weight[j] += this->out[i].dweight[j];
		}
	}
}

void web::epoh(double* arr, double* ans, size_t count){
	for (int i = 0; i < count; i++){
		fit(arr + (this->inSize)*i, ans[i]);
	}
}

double web::result(double* arr){

	this->init(arr);

	for(int i = 0; i < this->hidSize; i++){
		this->hiden[i].input = 0;
		for(int j = 0; j < this->inSize; j++){
			this->hiden[i].input += (this->in[j].output * this->hiden[i].weight[j]);
		}
		//this->hiden[i].input /= this->inSize;
		this->hiden[i].output = this->sigmoid(this->hiden[i].input);
	}

	for(int i = 0; i < this->hid2Size; i++){
		this->hiden2[i].input = 0;
		for(int j = 0; j < this->hidSize; j++){
			this->hiden2[i].input += (this->hiden[j].output * this->hiden2[i].weight[j]);
		}
		//this->hiden[i].input /= this->inSize;
		this->hiden2[i].output = this->sigmoid(this->hiden2[i].input);
	}

	for(int i = 0; i < this->outSize; i++){
		this->out[i].input = 0;
		for(int j = 0; j < this->hid2Size; j++){
			this->out[i].input += this->hiden2[j].output * this->out[i].weight[j];
		}
		//this->out[i].input /= this->hidSize;
		this->out[i].output = this->sigmoid(this->out[i].input);
	}
	return this->out[0].output;
}

void web::save(const char* file){

	FILE* fin = fopen(file, "w");
	if(fin == NULL)
		return;

	for(int i = 0 ;i < this->inSize; i++)
		fprintf(fin, "%lf ", this->in[i].weight[0]);
	for(int i = 0 ;i < this->hidSize; i++)
		for(int j = 0; j < this->inSize; j++)
			fprintf(fin, "%lf ", this->hiden[i].weight[j]);
	for(int i = 0 ;i < this->hid2Size; i++)
		for(int j = 0; j < this->hidSize; j++)
			fprintf(fin, "%lf ", this->hiden2[i].weight[j]);
	for(int i = 0 ;i < this->outSize; i++)
		for(int j = 0; j < this->hid2Size; j++)
			fprintf(fin, "%lf ", this->out[i].weight[j]);

	fclose(fin);

	return;
}


int web::load(const char* file){

	FILE* fin = fopen(file, "r");
	if (!fin)
		return -1;

	for(int i = 0 ;i < this->inSize; i++)
		fscanf(fin, "%lf ", &(this->in[i].weight[0]));
	for(int i = 0 ;i < this->hidSize; i++)
		for(int j = 0; j < this->inSize; j++)
			fscanf(fin, "%lf ", &(this->hiden[i].weight[j]));
	for(int i = 0 ;i < this->hid2Size; i++)
		for(int j = 0; j < this->hidSize; j++)
			fscanf(fin, "%lf ", &(this->hiden2[i].weight[j]));
	for(int i = 0 ;i < this->outSize; i++)
		for(int j = 0; j < this->hid2Size; j++)
			fscanf(fin, "%lf ", &(this->out[i].weight[j]));
	fclose(fin);
	return 0;
}



















