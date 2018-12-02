#pragma once
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <unistd.h>
#include <iostream>

void* display(void* tb, int md, const char* pwd);
