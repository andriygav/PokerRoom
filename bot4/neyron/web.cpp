#include "web.h"
#include <math.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

#include "web.h"
#include <math.h>

#define E 0.0313
#define A 0.001

web::web(size_t size[16]){

	this->inSize = size[0];
	this->playerCount = size[1];

	this->meInSize = size[2];	
	this->playerInSize = size[3];
	this->tableInSize = size[4];

	this->meSize = size[5];
	this->playerSize = size[6];
	this->tableSize = size[7];

	this->meNextSize = size[8];
	this->playerNextSize = size[9];
	this->tableNextSize = size[10];

	this->meSumSize = size[11];
	this->playerSumSize = size[12];
	this->tableSumSize = size[13];

	this->hidSize = size[14];
	this->outSize = size[15];

//Create input layer
	this->in = (neyr*)malloc(inSize*sizeof(neyr));
	for(int i = 0; i < inSize; i++){
		new(this->in + i)(neyr)(1,1);
	}

//Create "me" at the first layer
	this->meLayer = (neyr*)malloc(this->meSize*sizeof(neyr));
	for(int i = 0; i < this->meSize; i++){
		new(this->meLayer + i)(neyr)(this->meInSize);
	}
//Create "players" at the first layer
	this->playerLayer = (neyr**)malloc(this->playerCount * sizeof(neyr*));
	for(int i = 0; i < playerCount; i++){
		this->playerLayer[i] = (neyr*)malloc(this->playerSize * sizeof(neyr));
		for(int j = 0; j < this->playerSize; j++){
			new(this->playerLayer[i] + j)(neyr)(this->playerInSize);
		}
	}
//Create "table" at the first layer
	this->tableLayer = (neyr*)malloc(this->tableSize * sizeof(neyr));
	for(int i = 0; i < this->tableSize; i++){
		new(this->tableLayer + i)(neyr)(this->tableInSize);
	}

//Create "me" at the second layer
	this->meNextLayer = (neyr*)malloc(this->meNextSize*sizeof(neyr));
	for(int i = 0; i < this->meNextSize; i++){
		new(this->meNextLayer + i)(neyr)(this->meSize);
	}
//Create "players" at the second layer
	this->playerNextLayer = (neyr**)malloc(this->playerCount * sizeof(neyr*));
	for(int i = 0; i < playerCount; i++){
		this->playerNextLayer[i] = (neyr*)malloc(this->playerNextSize * sizeof(neyr));
		for(int j = 0; j < this->playerNextSize; j++){
			new(this->playerNextLayer[i] + j)(neyr)(this->playerSize);
		}
	}
//Create "table" at the second layer
	this->tableNextLayer = (neyr*)malloc(this->tableNextSize * sizeof(neyr));
	for(int i = 0; i < this->tableNextSize; i++){
		new(this->tableNextLayer + i)(neyr)(this->tableSize);
	}

//Create "me" at the third layer
	this->meSumLayer = (neyr*)malloc(this->meSumSize*sizeof(neyr));
	for(int i = 0; i < this->meSumSize; i++){
		new(this->meSumLayer + i)(neyr)(this->meNextSize);
	}
//Create "players" at the third layer
	this->playerSumLayer = (neyr*)malloc(this->playerSumSize * sizeof(neyr));
	for(int j = 0; j < this->playerSumSize; j++){
		new(this->playerSumLayer + j)(neyr)(this->playerNextSize * this->playerCount);
	}
//Create "table" at the third layer
	this->tableSumLayer = (neyr*)malloc(this->tableSumSize * sizeof(neyr));
	for(int i = 0; i < this->tableSumSize; i++){
		new(this->tableSumLayer + i)(neyr)(this->tableNextSize);
	}

//Create hide layer
	this->hiden = (neyr*)malloc(this->hidSize * sizeof(neyr));
	for(int i = 0; i < this->hidSize; i++){
		new(this->hiden + i)(neyr)(this->tableSumSize + this->playerSumSize + this->meSumSize);
	}

//Create out layer
	this->out = (neyr*)malloc(this->outSize * sizeof(neyr));
	for(int i = 0; i < this->outSize; i++){
		new(this->out + i)(neyr)(this->hidSize);
	}

	this->set_all_player(0);

}


void web::set_all_player(size_t num){
	for(size_t k = 0; k < this->playerCount; k++){
//Set first layer
		for(size_t i = 0; i < this->playerSize; i++){
			for(size_t j = 0; j < this->playerInSize; j++){
				this->playerLayer[k][i].weight[j] = this->playerLayer[num][i].weight[j];
			}
		}
//Set second layer
		for(size_t i = 0; i < this->playerNextSize; i++){
			for(size_t j = 0; j < this->playerSize; j++){
				this->playerNextLayer[k][i].weight[j] = this->playerNextLayer[num][i].weight[j];
			}
		}
//Set third layer
		for(size_t i = 0; i < this->playerSumSize; i++){
			for(size_t j = 0; j < this->playerNextSize; j++){
				this->playerSumLayer[i].weight[j + k*this->playerNextSize] = this->playerSumLayer[i].weight[j + num*this->playerNextSize];
			}
		}
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
		this->in[i].input = sigmoid(arr[i]);
		this->in[i].output = this->in[i].input;
	}
}

static double tah(double x){
	return ((exp(x) - exp(-x))/(exp(x) + exp(-x) + 2));
}

static double sign(double x){
	return (x>=0)?1:-1;
}

double web::sigmoid(double x){
	return tah(x) + 0.0001*x;
}

double web::fsigmoid(double x){
	return (2*exp(x))/((1 + exp(x))*(1 + exp(x))) + 0.0001;
}

void web::fit(double* arr, double ans){

	this->init(arr);

	double anns = result(arr);
	//printf("%lf %lf\n", anns, ans);
//out layer
	for(size_t i = 0; i < this->outSize; i++){
		this->out[i].delta = 2*(ans - this->out[i].output)*fsigmoid(this->out[i].input);
		//printf("out%zu delta %.8lf sigm %.8lf inp %.8lf\n", i, this->out[i].delta, fsigmoid(this->out[i].input), this->out[i].input);
	}	

//hiden layer back
	for(size_t i = 0; i < this->hidSize; i++){
		this->hiden[i].delta = 0;
		for(size_t j = 0; j < this->outSize; j++){
			this->hiden[i].delta += (this->out[j].weight[i] * this->out[j].delta);
		}
		this->hiden[i].delta = this->hiden[i].delta * fsigmoid(this->hiden[i].input);
		//printf("hiden%zu delta %.8lf sigm %lf inp %.8lf out %.8lf\n", i, this->hiden[i].delta, fsigmoid(this->hiden[i].input), this->hiden[i].input, this->hiden[i].output);
	}
//meSum layer back
	for(size_t i = 0; i < this->meSumSize; i++){
		this->meSumLayer[i].delta = 0;
		for(size_t j = 0; j < this->hidSize; j++){
			this->meSumLayer[i].delta += (this->hiden[j].weight[i] * this->hiden[j].delta);
		}
		this->meSumLayer[i].delta = this->meSumLayer[i].delta * fsigmoid(this->meSumLayer[i].input);
		//printf("sum%zu delta %.8lf sigm %.8lf inp %.8lf\n", i, this->meSumLayer[i].delta, fsigmoid(this->meSumLayer[i].input), this->meSumLayer[i].input);
	}
//playerSum layer back
	for(size_t i = 0; i < this->playerSumSize; i++){
		this->playerSumLayer[i].delta = 0;
		for(size_t j = 0; j < this->hidSize; j++){
			this->playerSumLayer[i].delta += (this->hiden[j].weight[this->meSumSize + i] * this->hiden[j].delta);
		}
		this->playerSumLayer[i].delta = this->playerSumLayer[i].delta * fsigmoid(this->playerSumLayer[i].input);
	}
//tableSum layer back
	for(size_t i = 0; i < this->tableSumSize; i++){
		this->tableSumLayer[i].delta = 0;
		for(size_t j = 0; j < this->hidSize; j++){
			this->tableSumLayer[i].delta += (this->hiden[j].weight[this->meSumSize + this->playerSumSize + i] * this->hiden[j].delta);
		}
		this->tableSumLayer[i].delta = this->tableSumLayer[i].delta * fsigmoid(this->tableSumLayer[i].input);
	}

//meNext layer back
	for(size_t i = 0; i < this->meNextSize; i++){
		this->meNextLayer[i].delta = 0;
		for(size_t j = 0; j < this->meSumSize; j++){
			this->meNextLayer[i].delta += (this->meSumLayer[j].weight[i] * this->meSumLayer[j].delta);
		}
		this->meNextLayer[i].delta = this->meNextLayer[i].delta * fsigmoid(this->meNextLayer[i].input);
	}
//playerNext layer back
	for(size_t k = 0; k < this->playerCount; k++){
		for(size_t i = 0; i < this->playerNextSize; i++){
			this->playerNextLayer[k][i].delta = 0;
			for(size_t j = 0; j < this->playerSumSize; j++){
				this->playerNextLayer[k][i].delta += (this->playerSumLayer[j].weight[i + k*this->playerNextSize] * this->meSumLayer[j].delta);
			}
			this->playerNextLayer[k][i].delta = this->playerNextLayer[k][i].delta * fsigmoid(this->playerNextLayer[k][i].input);
		}
	}
//tableNext layer back
	for(size_t i = 0; i < this->tableNextSize; i++){
		this->tableNextLayer[i].delta = 0;
		for(size_t j = 0; j < this->tableSumSize; j++){
			this->tableNextLayer[i].delta += (this->tableSumLayer[j].weight[i] * this->tableSumLayer[j].delta);
		}
		this->tableNextLayer[i].delta = this->tableNextLayer[i].delta * fsigmoid(this->tableNextLayer[i].input);
	}

//me layer back
	for(size_t i = 0; i < this->meSize; i++){
		this->meLayer[i].delta = 0;
		for(size_t j = 0; j < this->meNextSize; j++){
			this->meLayer[i].delta += (this->meNextLayer[j].weight[i] * this->meNextLayer[j].delta);
		}
		this->meLayer[i].delta = this->meLayer[i].delta * fsigmoid(this->meLayer[i].input);
	}
//player layer back
	for(size_t k = 0; k < this->playerCount; k++){
		for(size_t i = 0; i < this->playerSize; i++){
			this->playerLayer[k][i].delta = 0;
			for(size_t j = 0; j < this->playerNextSize; j++){
				this->playerLayer[k][i].delta += (this->playerNextLayer[k][j].weight[i] * this->playerNextLayer[k][j].delta);
			}
			this->playerLayer[k][i].delta = this->playerLayer[k][i].delta * fsigmoid(this->playerLayer[k][i].input);
		}
	}
//table layer back
	for(size_t i = 0; i < this->tableSize; i++){
		this->tableLayer[i].delta = 0;
		for(size_t j = 0; j < this->tableNextSize; j++){
			this->tableLayer[i].delta += (this->tableNextLayer[j].weight[i] * this->tableNextLayer[j].delta);
		}
		this->tableLayer[i].delta = this->tableLayer[i].delta * fsigmoid(this->tableLayer[i].input);
	}

//---------------------------------------------
//me layer forward
	for(size_t i = 0; i < this->meSize; i++){
		for (size_t j = 0; j < this->meInSize; j++){
			this->meLayer[i].dweight[j] = E*this->meLayer[i].delta*this->in[j].output + A*this->meLayer[i].dweight[j];
			this->meLayer[i].weight[j] += this->meLayer[i].dweight[j];
		}
	}
//player layer forward
	for(size_t k = 0; k < this->playerCount; k++){
		for(size_t i = 0; i < this->playerSize; i++){
			for (size_t j = 0; j < this->playerInSize; j++){
				this->playerLayer[k][i].dweight[j] = E*this->playerLayer[k][i].delta * this->in[j + k * this->playerInSize + this->meInSize].output + A*this->playerLayer[k][i].dweight[j];
				this->playerLayer[k][i].weight[j] += this->playerLayer[k][i].dweight[j];
			}
		}
	}
//table layer forward
	for(size_t i = 0; i < this->tableSize; i++){
		for (size_t j = 0; j < this->tableInSize; j++){
			this->tableLayer[i].dweight[j] = E*this->tableLayer[i].delta*this->in[j + this->playerCount * this->playerInSize + this->meInSize].output + A*this->tableLayer[i].dweight[j];
			this->tableLayer[i].weight[j] += this->tableLayer[i].dweight[j];
		}
	}
//meNext layer forward
	for(size_t i = 0; i < this->meNextSize; i++){
		for (size_t j = 0; j < this->meSize; j++){
			this->meNextLayer[i].dweight[j] = E*this->meNextLayer[i].delta*this->meLayer[j].output + A*this->meNextLayer[i].dweight[j];
			this->meNextLayer[i].weight[j] += this->meNextLayer[i].dweight[j];
		}
	}
//playerNext layer forward
	for(size_t k = 0; k < this->playerCount; k++){
		for(size_t i = 0; i < this->playerNextSize; i++){
			for (size_t j = 0; j < this->playerSize; j++){
				this->playerNextLayer[k][i].dweight[j] = E*this->playerNextLayer[k][i].delta * this->playerLayer[k][j].output + A*this->playerNextLayer[k][i].dweight[j];
				this->playerNextLayer[k][i].weight[j] += this->playerNextLayer[k][i].dweight[j];
			}
		}
	}
//tableNext layer forward
	for(size_t i = 0; i < this->tableNextSize; i++){
		for (size_t j = 0; j < this->tableSize; j++){
			this->tableNextLayer[i].dweight[j] = E*this->tableNextLayer[i].delta*this->tableLayer[j].output + A*this->tableNextLayer[i].dweight[j];
			this->tableNextLayer[i].weight[j] += this->tableNextLayer[i].dweight[j];
		}
	}
//meSum layer forward
	for(size_t i = 0; i < this->meSumSize; i++){
		for (size_t j = 0; j < this->meNextSize; j++){
			this->meSumLayer[i].dweight[j] = E*this->meSumLayer[i].delta*this->meNextLayer[j].output + A*this->meSumLayer[i].dweight[j];
			this->meSumLayer[i].weight[j] += this->meSumLayer[i].dweight[j];
			//printf("forv sum%zu delta %.8lf sigm %.8lf inp %.8lf\n", i, this->meSumLayer[i].dweight[j], this->meNextLayer[j].output, this->meSumLayer[i].dweight[j]);
		}
	}
//playerSum layer forward
	for(size_t i = 0; i < this->playerSumSize; i++){
		for(size_t k = 0; k < this->playerCount; k++){
			for (size_t j = 0; j < this->playerNextSize; j++){
				this->playerSumLayer[i].dweight[j + k * this->playerNextSize] = E*this->playerSumLayer[i].delta * this->playerNextLayer[k][j].output + A*this->playerSumLayer[i].dweight[j + k * this->playerNextSize];
				this->playerSumLayer[i].weight[j + k * this->playerNextSize] += this->playerSumLayer[i].dweight[j + k * this->playerNextSize];
			}
		}
	}
//tableSum layer forward
	for(size_t i = 0; i < this->tableSumSize; i++){
		for (size_t j = 0; j < this->tableNextSize; j++){
			this->tableSumLayer[i].dweight[j] = E*this->tableSumLayer[i].delta*this->tableNextLayer[j].output + A*this->tableSumLayer[i].dweight[j];
			this->tableSumLayer[i].weight[j] += this->tableSumLayer[i].dweight[j];
		}
	}
	
//hiden layer forward
	for(size_t i = 0; i < this->hidSize; i++){
		for (size_t j = 0; j < this->meSumSize; j++){
			this->hiden[i].dweight[j] = E*this->hiden[i].delta*this->meSumLayer[j].output + A*this->hiden[i].dweight[j];
			this->hiden[i].weight[j] += this->hiden[i].dweight[j];
		}
		for (size_t j = 0; j < this->playerSumSize; j++){
			this->hiden[i].dweight[j + this->meSumSize] = E*this->hiden[i].delta*this->playerSumLayer[j].output + A*this->hiden[i].dweight[j + this->meSumSize];
			this->hiden[i].weight[j + this->meSumSize] += this->hiden[i].dweight[j + this->meSumSize];
		}
		for (size_t j = 0; j < this->tableSumSize; j++){
			this->hiden[i].dweight[j + this->meSumSize + this->playerSumSize] = E*this->hiden[i].delta*this->tableSumLayer[j].output + A*this->hiden[i].dweight[j + this->meSumSize + this->playerSumSize];
			this->hiden[i].weight[j + this->meSumSize + this->playerSumSize] += this->hiden[i].dweight[j + this->meSumSize + this->playerSumSize];
		}
	}
	for(size_t i = 0; i < this->outSize; i++){
		for(size_t j = 0; j < this->hidSize; j++){
			//printf("out%zu delta %.8lf hid out %.8lf weight %.8lf d %.8lf\n", i, this->out[i].delta, this->hiden[j].output, this->out[i].weight[j], this->out[i].dweight[j]);
			this->out[i].dweight[j] = E*this->out[i].delta*this->hiden[j].output + A*this->out[i].dweight[j];
			this->out[i].weight[j] += this->out[i].dweight[j];
			//printf("out%zu delta %.8lf hid out %.8lf weight %.8lf d %.8lf\n", i, this->out[i].delta, this->hiden[j].output, this->out[i].weight[j], this->out[i].dweight[j]);
		}
	}

//set equal weight for player
	this->set_all_player(0);
}

void web::epoh(double* arr, double* ans, size_t count){
	for (int i = 0; i < count; i++){
		fit(arr + (this->inSize)*i, ans[i]);
	}
}


double web::result(double* arr){
	this->init(arr);
//count first layer

	for(size_t i = 0; i < this->meSize; i++){
		this->meLayer[i].input = 0;
		for(size_t j = 0; j < this->meInSize; j++){
			this->meLayer[i].input += this->in[j].output * this->meLayer[i].weight[j];
		}
		this->meLayer[i].output = this->sigmoid(this->meLayer[i].input);
	}

	for(size_t k = 0; k < this->playerCount; k++){
		for(size_t i = 0; i < this->playerSize; i++){
			this->playerLayer[k][i].input = 0;
			for(size_t j = 0; j < this->playerInSize; j++){
				this->playerLayer[k][i].input += this->in[this->meInSize + k*this->playerInSize + j].output * this->playerLayer[k][i].weight[j];
			}
			this->playerLayer[k][i].output = this->sigmoid(this->playerLayer[k][i].input);
		}
	}

	for(size_t i = 0; i < this->tableSize; i++){
		this->tableLayer[i].input = 0;
		for(size_t j = 0; j < this->tableInSize; j++){
			this->tableLayer[i].input += this->in[this->meInSize + this->playerCount*this->playerInSize + j].output * this->tableLayer[i].weight[j];
		}
		this->tableLayer[i].output = this->sigmoid(this->tableLayer[i].input);
	}


//count second layer
	for(size_t i = 0; i < this->meNextSize; i++){
		this->meNextLayer[i].input = 0;
		for(size_t j = 0; j < this->meSize; j++){
			this->meNextLayer[i].input += this->meLayer[j].output * this->meNextLayer[i].weight[j];
		}
		this->meNextLayer[i].output = this->sigmoid(this->meNextLayer[i].input);
	}

	for(size_t k = 0; k < this->playerCount; k++){
		for(size_t i = 0; i < this->playerNextSize; i++){
			this->playerNextLayer[k][i].input = 0;
			for(size_t j = 0; j < this->playerSize; j++){
				this->playerNextLayer[k][i].input += this->playerLayer[k][j].output * this->playerNextLayer[k][i].weight[j];
			}
			this->playerNextLayer[k][i].output = this->sigmoid(this->playerNextLayer[k][i].input);
		}
	}

	for(size_t i = 0; i < this->tableNextSize; i++){
		this->tableNextLayer[i].input = 0;
		for(size_t j = 0; j < this->tableSize; j++){
			this->tableNextLayer[i].input += this->tableLayer[j].output * this->tableNextLayer[i].weight[j];
		}
		this->tableNextLayer[i].output = this->sigmoid(this->tableNextLayer[i].input);
	}


//count third layer
	for(size_t i = 0; i < this->meSumSize; i++){
		this->meSumLayer[i].input = 0;
		for(size_t j = 0; j < this->meNextSize; j++){
			this->meSumLayer[i].input += this->meNextLayer[j].output * this->meSumLayer[i].weight[j];
		}
		this->meSumLayer[i].output = this->sigmoid(this->meSumLayer[i].input);
	}


	for(size_t i = 0; i < this->playerSumSize; i++){
		this->playerSumLayer[i].input = 0;
		for(size_t k = 0; k < this->playerCount; k++){
			for(size_t j = 0; j < this->playerNextSize; j++){
				this->playerSumLayer[i].input += this->playerNextLayer[k][j].output * this->playerSumLayer[i].weight[j + k*this->playerNextSize];
			}
		}
		this->playerSumLayer[i].output = this->sigmoid(this->playerSumLayer[i].input);
	}


	for(size_t i = 0; i < this->tableSumSize; i++){
		this->tableSumLayer[i].input = 0;
		for(size_t j = 0; j < this->tableNextSize; j++){
			this->tableSumLayer[i].input += this->tableNextLayer[j].output * this->tableSumLayer[i].weight[j];
		}
		this->tableSumLayer[i].output = this->sigmoid(this->tableSumLayer[i].input);
	}

//count hide layer

	for(int i = 0; i < this->hidSize; i++){
		this->hiden[i].input = 0;
		for(int j = 0; j < this->meSumSize; j++){
			this->hiden[i].input += this->meSumLayer[j].output * this->hiden[i].weight[j];
		}
		for(int j = 0; j < this->playerSumSize; j++){
			this->hiden[i].input += this->playerSumLayer[j].output * this->hiden[i].weight[this->meSumSize + j];
		}
		for(int j = 0; j < this->tableSumSize; j++){
			this->hiden[i].input += this->tableSumLayer[j].output * this->hiden[i].weight[this->meSumSize + this->playerSumSize + j];
		}
		this->hiden[i].output = this->sigmoid(this->hiden[i].input);
	}

//out layer

	for(int i = 0; i < this->outSize; i++){
		this->out[i].input = 0;
		for(int j = 0; j < this->hidSize; j++){
			this->out[i].input += this->hiden[j].output * this->out[i].weight[j];
		}
		this->out[i].output = this->sigmoid(this->out[i].input);
	}


	return this->out[0].output;
}

void web::save(const char* file){
	FILE* fin = fopen(file, "w");
	if(fin == NULL)
		return;
//input layer
	for(size_t i = 0 ;i < this->inSize; i++)
		fprintf(fin, "%lf ", this->in[i].weight[0]);
	fprintf(fin, "\n");
//first layer
	for(size_t i = 0 ;i < this->meSize; i++)
		for(size_t j = 0; j < this->meInSize; j++)
			fprintf(fin, "%lf ", this->meLayer[i].weight[j]);
	fprintf(fin, "\n");
	for(size_t k = 0; k < this->playerCount; k++){
		for(size_t i = 0 ;i < this->playerSize; i++)
			for(size_t j = 0; j < this->playerInSize; j++)
				fprintf(fin, "%lf ", this->playerLayer[k][i].weight[j]);
	}
	for(size_t i = 0 ;i < this->tableSize; i++)
		for(size_t j = 0; j < this->tableInSize; j++)
			fprintf(fin, "%lf ", this->tableLayer[i].weight[j]);
	fprintf(fin, "\n");
//printf second layer
	for(size_t i = 0 ;i < this->meNextSize; i++)
		for(size_t j = 0; j < this->meSize; j++)
			fprintf(fin, "%lf ", this->meNextLayer[i].weight[j]);
	fprintf(fin, "\n");
	for(size_t k = 0; k < this->playerCount; k++){
		for(size_t i = 0 ;i < this->playerSize; i++)
			for(size_t j = 0; j < this->playerInSize; j++)
				fprintf(fin, "%lf ", this->playerLayer[k][i].weight[j]);
		fprintf(fin, "\n");
	}
	fprintf(fin, "\n");
	for(size_t i = 0 ;i < this->tableSize; i++)
		for(size_t j = 0; j < this->tableInSize; j++)
			fprintf(fin, "%lf ", this->tableLayer[i].weight[j]);
	fprintf(fin, "\n");	
//printf third layer
	for(size_t i = 0 ;i < this->meSumSize; i++)
		for(size_t j = 0; j < this->meNextSize; j++)
			fprintf(fin, "%lf ", this->meSumLayer[i].weight[j]);
	fprintf(fin, "\n");
	for(size_t i = 0 ;i < this->playerSumSize; i++)
		for(size_t k = 0; k < this->playerCount; k++)
			for(size_t j = 0; j < this->playerNextSize; j++)
				fprintf(fin, "%lf ", this->playerSumLayer[i].weight[j + k*this->playerNextSize]);
	fprintf(fin, "\n");
	for(size_t i = 0 ;i < this->tableSumSize; i++)
		for(size_t j = 0; j < this->tableNextSize; j++)
			fprintf(fin, "%lf ", this->tableNextLayer[i].weight[j]);
	fprintf(fin, "\n");

//printf hiden and out layer
	for(size_t i = 0 ;i < this->hidSize; i++)
		for(size_t j = 0; j < this->tableSumSize + this->playerSumSize + this->meSumSize; j++)
			fprintf(fin, "%lf ", this->hiden[i].weight[j]);
	fprintf(fin, "\n");
	for(size_t i = 0; i < this->outSize; i++)
		for(size_t j = 0; j < this->hidSize; j++)
			fprintf(fin, "%lf ", this->out[i].weight[j]);

	fclose(fin);
}


int web::load(const char* file){

	FILE* fin = fopen(file, "r");
	if (!fin)
		return -1;


//input layer
	for(size_t i = 0 ;i < this->inSize; i++)
		fscanf(fin, "%lf ", &(this->in[i].weight[0]));
//first layer
	for(size_t i = 0 ;i < this->meSize; i++)
		for(size_t j = 0; j < this->meInSize; j++)
			fscanf(fin, "%lf ", &(this->meLayer[i].weight[j]));
	for(size_t k = 0; k < this->playerCount; k++){
		for(size_t i = 0 ;i < this->playerSize; i++)
			for(size_t j = 0; j < this->playerInSize; j++)
				fscanf(fin, "%lf ", &(this->playerLayer[k][i].weight[j]));
	}
	for(size_t i = 0 ;i < this->tableSize; i++)
		for(size_t j = 0; j < this->tableInSize; j++)
			fscanf(fin, "%lf ", &(this->tableLayer[i].weight[j]));
//second layer
	for(size_t i = 0 ;i < this->meNextSize; i++)
		for(size_t j = 0; j < this->meSize; j++)
			fscanf(fin, "%lf ", &(this->meNextLayer[i].weight[j]));
	for(size_t k = 0; k < this->playerCount; k++){
		for(size_t i = 0 ;i < this->playerSize; i++)
			for(size_t j = 0; j < this->playerInSize; j++)
				fscanf(fin, "%lf ", &(this->playerLayer[k][i].weight[j]));
	}
	for(size_t i = 0 ;i < this->tableSize; i++)
		for(size_t j = 0; j < this->tableInSize; j++)
			fscanf(fin, "%lf ", &(this->tableLayer[i].weight[j]));
//third layer
	for(size_t i = 0 ;i < this->meSumSize; i++)
		for(size_t j = 0; j < this->meNextSize; j++)
			fscanf(fin, "%lf ", &(this->meSumLayer[i].weight[j]));
	for(size_t i = 0 ;i < this->playerSumSize; i++)
		for(size_t k = 0; k < this->playerCount; k++)
			for(size_t j = 0; j < this->playerNextSize; j++)
				fscanf(fin, "%lf ", &(this->playerSumLayer[i].weight[j + k*this->playerNextSize]));
	for(size_t i = 0 ;i < this->tableSumSize; i++)
		for(size_t j = 0; j < this->tableNextSize; j++)
			fscanf(fin, "%lf ", &(this->tableNextLayer[i].weight[j]));
//hiden and out layer
	for(size_t i = 0 ;i < this->hidSize; i++)
		for(size_t j = 0; j < this->tableSumSize + this->playerSumSize + this->meSumSize; j++)
			fscanf(fin, "%lf ", &(this->hiden[i].weight[j]));

	for(size_t i = 0; i < this->outSize; i++)
		for(size_t j = 0; j < this->hidSize; j++)
			fscanf(fin, "%lf ", &(this->out[i].weight[j]));

	fclose(fin);
	return 0;
}


