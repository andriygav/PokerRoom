#include "button.h"

#define BUTTON_ACTIVE 1
#define BUTTON_NOT_ACTIVE 0

static SDL_Surface* load_image(const char * filename){
	SDL_Surface* load = SDL_LoadBMP(filename);
	if(load != NULL){
		SDL_Surface* optimal = SDL_DisplayFormat(load);
		SDL_FreeSurface(load);
		Uint32 colorkey = SDL_MapRGB(optimal->format, 200, 200, 200);
		SDL_SetColorKey(optimal, SDL_SRCCOLORKEY, colorkey );
		return optimal;
	}
	return NULL;
}

button::~button(){
	SDL_FreeSurface(texture);
	this->action = NULL;
	this->action_data = NULL;
	this->texture = NULL;
}

void button::drow(SDL_Surface* screen){
	SDL_Surface* texture = SDL_DisplayFormat(this->texture);

	SDL_Surface* text_surf = TTF_RenderText_Solid(this->font, this->text, this->text_color);

	SDL_Rect offset;
	SDL_GetClipRect(text_surf, &offset);
	offset.x = (this->x_size - offset.w)/2;
	offset.y = (this->y_size - offset.h)/2;

	SDL_BlitSurface(text_surf, NULL, texture, &offset);

	SDL_FreeSurface(text_surf);

	offset.x = this->x_pos;
	offset.y = this->y_pos;

	if(this->status == true)
		SDL_BlitSurface(texture, NULL, screen, &offset);

	SDL_FreeSurface(texture);
	return;
}


bool button::in_button(int x, int y){
	if(x >= this->x_pos + this->x_gard && x <= this->x_pos + this->x_size - this->x_gard && y >= this->y_pos + this->y_gard && y <= this->y_pos + this->y_size - this->y_gard){
		return true;
	}
	return false;
}

void button::change_mode(long mode){

	switch(mode){
		case BUTTON_ACTIVE:
			this->mode = BUTTON_ACTIVE;
			this->text_color.r = 255;
			this->text_color.g = 0;
			this->text_color.b = 0;
			break;
		case BUTTON_NOT_ACTIVE:
			this->mode = BUTTON_NOT_ACTIVE;
			this->text_color.r = 0;
			this->text_color.g = 0;
			this->text_color.b = 0;
			break;

	}
	SDL_Surface* text_surf = TTF_RenderText_Solid(this->font, this->text, this->text_color);

	SDL_Rect offset;
	SDL_GetClipRect(text_surf, &offset);
	offset.x = (this->x_size - offset.w)/2;
	offset.y = (this->y_size - offset.h)/2;

	SDL_BlitSurface(text_surf, NULL, this->texture, &offset);

	SDL_FreeSurface(text_surf);
}

bool button::clicking(SDL_Event* event){
	if(this->status == false)
		return false;

	if(event->type == SDL_MOUSEBUTTONDOWN){
		if(event->button.button == SDL_BUTTON_LEFT){
			if(this->in_button(event->button.x, event->button.y)){
				if(this->action != NULL){
					this->action(this->action_data);
				}
				return true;
			}
		}
	}
	if(event->type == SDL_MOUSEMOTION){
		if(this->in_button(event->button.x, event->button.y)){
			if(this->mode == BUTTON_NOT_ACTIVE){
				this->change_mode(BUTTON_ACTIVE);
				return true;
			}
		}else{
			if(this->mode == BUTTON_ACTIVE){
				this->change_mode(BUTTON_NOT_ACTIVE);
				return true;
			}
		}
	}
	return false;
}



button::button(int x, int y, int x_size, int y_size, const char* source, const char* text, TTF_Font* font, void* func, void* data, int x_gur, int y_gur){
	this->x_pos = x;
	this->y_pos = y;
	this->x_gard = x_gur;
	this->y_gard = y_gur;
	this->x_size = x_size;
	this->y_size = y_size;
	this->texture = load_image(source);
	this->action = (int (*)(void*))func;
	this->action_data = data;
	this->font = font;
	this->status = true;
	this->mode = BUTTON_NOT_ACTIVE;
	for(int i = 0; i < 256; i++)
		this->text[i] = 0;
	sprintf(this->text, " %s", text);

	this->text_color.r = 0;
	this->text_color.g = 0;
	this->text_color.b = 0;
	int mx = 0, my = 0;
	SDL_GetMouseState(&mx, &my);
	if(this->in_button(mx, my)){
		if(this->mode == BUTTON_NOT_ACTIVE){
			this->change_mode(BUTTON_ACTIVE);
		}
	}else{
		if(this->mode == BUTTON_ACTIVE){
			this->change_mode(BUTTON_NOT_ACTIVE);
		}
	}
}

button::button(int x, int y, int x_size, int y_size, SDL_Surface* source, const char* text, TTF_Font* font, void* func, void* data, int x_gur, int y_gur){
	this->x_pos = x;
	this->y_pos = y;
	this->x_size = x_size;
	this->y_size = y_size;
	this->x_gard = x_gur;
	this->y_gard = y_gur;
	this->texture = SDL_DisplayFormat(source);
	this->action = (int (*)(void*))func;
	this->action_data = data;
	this->font = font;
	this->status = true;
	this->mode = BUTTON_NOT_ACTIVE;
	for(int i = 0; i < 256; i++)
		this->text[i] = 0;
	sprintf(this->text, " %s", text);
	int mx = 0, my = 0;
	SDL_GetMouseState(&mx, &my);

	this->text_color.r = 0;
	this->text_color.g = 0;
	this->text_color.b = 0;
	
	if(this->in_button(mx, my)){
		if(this->mode == BUTTON_NOT_ACTIVE){
			this->change_mode(BUTTON_ACTIVE);
		}
	}else{
		if(this->mode == BUTTON_ACTIVE){
			this->change_mode(BUTTON_NOT_ACTIVE);
		}
	}
}

button::button(int x, int y, int x_size, int y_size, const char* source, const char* text, TTF_Font* font, void* func, void* data){
	this->x_pos = x;
	this->y_pos = y;
	this->x_gard = 4;
	this->y_gard = 4;
	this->x_size = x_size;
	this->y_size = y_size;
	this->texture = load_image(source);
	this->action = (int (*)(void*))func;
	this->action_data = data;
	this->font = font;
	this->status = true;
	this->mode = BUTTON_NOT_ACTIVE;
	for(int i = 0; i < 256; i++)
		this->text[i] = 0;
	sprintf(this->text, " %s", text);

	this->text_color.r = 0;
	this->text_color.g = 0;
	this->text_color.b = 0;
	int mx = 0, my = 0;
	SDL_GetMouseState(&mx, &my);
	if(this->in_button(mx, my)){
		if(this->mode == BUTTON_NOT_ACTIVE){
			this->change_mode(BUTTON_ACTIVE);
		}
	}else{
		if(this->mode == BUTTON_ACTIVE){
			this->change_mode(BUTTON_NOT_ACTIVE);
		}
	}
}

button::button(int x, int y, int x_size, int y_size, SDL_Surface* source, const char* text, TTF_Font* font, void* func, void* data){
	this->x_pos = x;
	this->y_pos = y;
	this->x_size = x_size;
	this->y_size = y_size;
	this->x_gard = 4;
	this->y_gard = 4;
	this->texture = SDL_DisplayFormat(source);
	this->action = (int (*)(void*))func;
	this->action_data = data;
	this->font = font;
	this->status = true;
	this->mode = BUTTON_NOT_ACTIVE;
	for(int i = 0; i < 256; i++)
		this->text[i] = 0;
	sprintf(this->text, " %s", text);
	int mx = 0, my = 0;
	SDL_GetMouseState(&mx, &my);

	this->text_color.r = 0;
	this->text_color.g = 0;
	this->text_color.b = 0;
	
	if(this->in_button(mx, my)){
		if(this->mode == BUTTON_NOT_ACTIVE){
			this->change_mode(BUTTON_ACTIVE);
		}
	}else{
		if(this->mode == BUTTON_ACTIVE){
			this->change_mode(BUTTON_NOT_ACTIVE);
		}
	}
}
