#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "button.h"


class slide{
public:
	SDL_Surface* texture;
	int x_pos;
	int y_pos;
	int x_size;
	int y_size;
	TTF_Font* font;
	SDL_Color text_color;
	class button* but;

	bool status;

	bool active;

	void drow(SDL_Surface*, double);

	char text[256];

	double but_pos;

	bool clicking(SDL_Event*);

	double value(double val);


	slide(int, int, int, int, const char*, const char*, const char*, TTF_Font*);
	//slide(int, int, int, int, const char*, const char*, TTF_Font*, void*, void*);
	~slide();
};
