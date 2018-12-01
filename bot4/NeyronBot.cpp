#include "NeyronBot.h"
#include <iostream>
#include <unistd.h>

bot::bot(size_t inSize){
	this->PreFlop = (class web*)malloc(sizeof(class web));
	this->Flop = (class web*)malloc(sizeof(class web));
	this->Turn = (class web*)malloc(sizeof(class web));
	this->River = (class web*)malloc(sizeof(class web));
	this->LastAns = 0;

	this->inSize = inSize;
	size_t size[16];

	size[0] = this->inSize;//Length of vector
	size[1] = 5;//Count of player
	size[2] = 4;//Count of line with my information
	size[3] = 2;//Count of line with one player information
	size[4] = 1;//Count of line with server information
	
	size[5] = 4;//Count of neyron in first layer for my
	size[6] = 2;//COunt of neyron in first layer for one player
	size[7] = 2;//Count of neyron in first layer for table
	
	size[8] = 3;//Count of neyron in second layer for my
	size[9] = 2;//Count of neyron in second layer for one player
	size[10] = 2;//Count of neyron in second layer for table

	size[11] = 2;//Count of neyron in third(sum) layer for my
	size[12] = 1;//Count of neyron in third(sum) layer for one player
	size[13] = 1;//Count of neyron in third(sum) layer for table

	size[14] = 3;//Count of neyron in hiden layer
	size[15] = 1;//Count of neyron in out layer
	
	new(this->PreFlop)(web)(size);
	new(this->Flop)(web)(size);
	new(this->Turn)(web)(size);
	new(this->River)(web)(size);

	this->load();

	reset_log();
}

double bot::action(size_t num, double* arr){
	switch(num){
		case 1:
			arr[0] = this->LastAns;
			this->LastAns = this->PreFlop->result(arr);
			break;
		case 2:
			arr[0] = this->LastAns;
			this->LastAns = this->Flop->result(arr);
			break;
		case 3:
			arr[0] = this->LastAns;
			this->LastAns = this->Turn->result(arr);
			break;
		case 4:
			arr[0] = this->LastAns;
			this->LastAns = this->River->result(arr);
			break;
		default:
			return -1;
			break;
	}
	return this->LastAns;
}

int bot::log(double* arr, size_t num, double ans, size_t act){
	if(this->count > 1023)
		return 0;

	for(int i = 0; i < this->inSize; i++){
		this->arr[this->count][i] = arr[i];
	}
	this->num[this->count] = num;
	this->ans[this->count] = ans;
	this->myans[this->count] = ans;
	this->act[this->count] = act;

	this->count++;
	return 0;
}

int bot::reset_log(){
	this->count = 0;
	return 0;
}

int bot::learn(size_t num, double* arr, double ans){

	switch(num){
		case 1:
			this->PreFlop->fit(arr, ans);
			break;
		case 2:
			this->Flop->fit(arr, ans);
			break;
		case 3:
			this->Turn->fit(arr, ans);
			break;
		case 4:
			this->River->fit(arr, ans);
			break;
		default:
			return -1;
			break;
	}
	return 0;
}

double bot::result(size_t num, double* arr){

	switch(num){
		case 1:
			return this->PreFlop->result(arr);
			break;
		case 2:
			return this->Flop->result(arr);
			break;
		case 3:
			return this->Turn->result(arr);
			break;
		case 4:
			return this->River->result(arr);
			break;
		default:
			return -1;
			break;
	}
	return 0;
}

int bot::save(){
	this->PreFlop->save((const char*)"./Bot4/PreFlop_second.bot");
	this->Flop->save((const char*)"./Bot4/Flop_second.bot");
	this->Turn->save((const char*)"./Bot4/Turn_second.bot");
	this->River->save((const char*)"./Bot4/River_second.bot");
	return 0;
}

int bot::load(){
	this->PreFlop->load((const char*)"./Bot4/PreFlop_second.bot");
	this->Flop->load((const char*)"./Bot4/Flop_second.bot");
	this->Turn->load((const char*)"./Bot4/Turn_second.bot");
	this->River->load((const char*)"./Bot4/River_second.bot");
	return 0;
}

int bot::fit(size_t num){

	this->LastAns = 0;
	for(size_t i = 0; i < this->count; i++){
		if(this->num[i] == num){
			learn(this->act[i], this->arr[i], this->ans[i]);
		}
	}

	return 0;
}



