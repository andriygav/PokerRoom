#include "slide.h"

static SDL_Surface* load_image(const char * filename){
	SDL_Surface* load = SDL_LoadBMP(filename);
	if(load != NULL){
		SDL_Surface* optimal = SDL_DisplayFormat(load);
		SDL_FreeSurface(load);
		Uint32 colorkey = SDL_MapRGB(optimal->format, 128, 128, 0);
		SDL_SetColorKey(optimal, SDL_SRCCOLORKEY, colorkey );
		return optimal;
	}
	return NULL;
}

double slide::value(double val){
	return val*this->but_pos;
}

slide::slide(int x, int y, int x_size, int y_size, const char* source, const char* sourceBack, const char* sourceLine, TTF_Font* font){
	this->x_pos = x;
	this->y_pos = y;
	this->x_size = x_size;
	this->y_size = y_size;
	this->but_pos = 0;
	this->font = font;
	this->texture = load_image(sourceBack);

	SDL_Surface* textur = load_image(sourceLine);
	SDL_BlitSurface(textur, NULL, this->texture, NULL);
	SDL_FreeSurface(textur);

	this->active = false;
	this->status = true;

	this->text_color.r = 1;
	this->text_color.g = 1;
	this->text_color.b = 1;

	for(int i = 0; i < sizeof(this->text); i++){
		this->text[i] = 0;
	}

	sprintf(this->text, "%lf", 0.0);

	this->but = new button(0, 0, 10, 10, source, "", this->font, NULL, NULL, 0, 0);

}

bool slide::clicking(SDL_Event* event){

	if(event->type == SDL_MOUSEBUTTONDOWN){
		if(event->button.button == SDL_BUTTON_LEFT){
			if(this->but->in_button(event->button.x - this->x_pos, event->button.y - this->y_pos)){
				this->active = true;
				return true;
			}
		}
	}
	if(event->type == SDL_MOUSEBUTTONUP){
		if(event->button.button == SDL_BUTTON_LEFT){
			this->active = false;
			return true;
		}
	}

	if(event->type == SDL_MOUSEMOTION){
		if(this->active){
			if(event->button.x < this->x_pos + this->x_size/2){
				this->but->x_pos = ((event->button.x - this->x_pos >= 0)?(event->button.x - this->x_pos):0);
			}else{
				this->but->x_pos = ((event->button.x - this->x_pos < this->x_size - this->but->x_size)?(event->button.x - this->x_pos):(this->x_size - this->but->x_size));
			}
			this->but_pos = (this->but->x_pos*1.0)/((this->x_size - this->but->x_size)*1.0);
			return true;
		}
	}
	

	return true;
}



void slide::drow(SDL_Surface* screen, double cash){
	SDL_Surface* texture = SDL_DisplayFormat(this->texture);

	sprintf(this->text, "%lf", this->but_pos * cash);

	SDL_Surface* text_surf = TTF_RenderText_Solid(this->font, this->text, this->text_color);

	SDL_Rect offset;
	SDL_GetClipRect(text_surf, &offset);
	offset.x = (this->x_size - offset.w)/2;
	offset.y = (this->y_size - 10 - offset.h)/2 + 10;

	SDL_BlitSurface(text_surf, NULL, texture, &offset);

	SDL_FreeSurface(text_surf);

	this->but->drow(texture);

	offset.x = this->x_pos;
	offset.y = this->y_pos;

	if(this->status == true)
		SDL_BlitSurface(texture, NULL, screen, &offset);

	SDL_FreeSurface(texture);
	return;
}

slide::~slide(){
	SDL_FreeSurface(texture);
	delete(this->but);
	this->texture = NULL;
}
