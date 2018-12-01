#include "NeyronBot.h"
#include <iostream>
#include <unistd.h>

bot::bot(size_t inSize, size_t hidSize, size_t hid2Size, size_t outSize){
	/*this->PreFlop = (class web*)malloc(sizeof(class web));
	this->Flop = (class web*)malloc(sizeof(class web));
	this->Turn = (class web*)malloc(sizeof(class web));
	this->River = (class web*)malloc(sizeof(class web));
	this->LastAns = 0;

	this->inSize = inSize;

	new(this->PreFlop)(web)(inSize, hidSize, outSize);
	new(this->Flop)(web)(inSize, hidSize, outSize);
	new(this->Turn)(web)(inSize, hidSize, outSize);
	new(this->River)(web)(inSize, hidSize, outSize);*/

	this->inSize = inSize;
	this->LastAns = 0;
	this->PreFlop = new web(inSize, hidSize, hid2Size, outSize);
	this->Flop = new web(inSize, hidSize, hid2Size, outSize);
	this->Turn = new web(inSize, hidSize, hid2Size, outSize);
	this->River = new web(inSize, hidSize, hid2Size, outSize);

	reset_log();
	this->load();
}

int bot::save(){
	this->PreFlop->save((const char*)"Bot3/PreFlop_first.bot");
	this->Flop->save((const char*)"Bot3/Flop_first.bot");
	this->Turn->save((const char*)"Bot3/Turn_first.bot");
	this->River->save((const char*)"Bot3/River_first.bot");
	return 0;
}

int bot::load(){
	this->PreFlop->load((const char*)"Bot3/PreFlop_first.bot");
	this->Flop->load((const char*)"Bot3/Flop_first.bot");
	this->Turn->load((const char*)"Bot3/Turn_first.bot");
	this->River->load((const char*)"Bot3/River_first.bot");
	return 0;
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
	if(this->count > 102023)
		return 0;

	for(int i = 0; i < this->inSize; i++){
		this->arr[this->count][i] = arr[i];
	}
	this->num[this->count] = num;
	this->ans[this->count] = ans;
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

int bot::fit(size_t num){

	for(size_t i = 0; i < this->count; i++){
		if(this->num[i] == num){
			learn(this->act[i], this->arr[i], this->ans[i]);
		}
	}

	return 0;
}



