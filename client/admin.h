#pragma once
#include "play_info.h"

#define EXIT -1
#define MENU 1

#define ONLY_STR 1
#define SHOWROOMLOG 2

class admin_t{
	public:
		bool argument[256];
		admin_t(int status, int sock, bool* argument, int fd);
		~admin_t();

		int menu();

		int status;
		int sock;
		int fd;
		int id;
};

