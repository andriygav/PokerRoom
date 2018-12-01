#pragma once

#pragma pack(push, 1)
struct publplayinfo{
	char login[256];
	double cash;
	int last_action;
	double bet;
	int status;
	int blind;
	int card[2];
	int comb;
};
#pragma pack(pop)
#pragma pack(push, 1)
struct publtableinfo{
	int card[5];
	double bank;
	double bet;
	double blind;
	int num;
};
#pragma pack(pop)
#pragma pack(push, 1)
struct sendinf{
	struct publplayinfo player[6];
	struct publtableinfo table;
	int card[2];
	int num;
	int final_table[6];
};
#pragma pack(pop)


#pragma pack(push, 1)
struct recivesock{
	long code;
	size_t id;
	struct sendinf inf;
};
#pragma pack(pop)
#pragma pack(push, 1)
struct recive_t{
	char buf[256];
	char login[256];
	size_t id;
};
#pragma pack(pop)

