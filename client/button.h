#pragma once
#include <SDL.h>
#include <SDL_ttf.h>


class button{
public:
	SDL_Surface* texture;
	int x_pos;
	int y_pos;
	int x_size;
	int y_size;

	int x_gard;
	int y_gard;

	button(int, int, int, int, SDL_Surface*, const char*, TTF_Font*, void*, void*);
	button(int, int, int, int, const char*, const char*, TTF_Font*, void*, void*);
	button(int, int, int, int, SDL_Surface*, const char*, TTF_Font*, void*, void*, int, int);
	button(int, int, int, int, const char*, const char*, TTF_Font*, void*, void*, int, int);
	~button();
	
	bool status;
	bool clicking(SDL_Event*);
	bool in_button(int, int);
	void drow(SDL_Surface*);

	SDL_Color text_color;

	void change_mode(long);
	char text[256];
	TTF_Font* font;
	int mode;


	int (*action)(void*);
	void* action_data;
};
