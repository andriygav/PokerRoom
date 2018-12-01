#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/poll.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>

#include "table.h"


#define REFRESH 10001
#define CONECT 10002
#define DISCONECT 10003
#define ACTION_CALL 10004
#define ACTION_RAISE 10005
#define ACTION_FOLD 10006
#define ACTION_NEW 10007
#define GETMONEY 10008
#define PUTMONEY 10009

#define FROM_SERVER_TO_TABLE 1000000

#define SERVER_COMAND_EXIT 100001
#define SERVER_DESTROY_ROOM 100002
#define SERVER_COMAND_SHOW_INFO 100003
#define SERVER_COMAND_DONT_SHOW_INFO 100004
#define SERVER_COMAND_SEND_ALL_INFO 100005
 

#pragma pack(push, 1)
struct msg_from_server_t{
	long num;
	long comand;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct poker_argumets_t{
	int mds;
	struct client* cl;
	struct table_t* tb;
	int num;
	FILE* file;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct get_info_from_client_argumets_t{
	int i;
	pthread_mutex_t* mut;
	struct client* cl;
	class pokroom* obj;
};
#pragma pack(pop)



void* get_info_from_client(void* argv){
	int i = ((struct get_info_from_client_argumets_t*)argv)->i;
	pthread_mutex_t* mut = ((struct get_info_from_client_argumets_t*)argv)->mut;
	struct client* cl = ((struct get_info_from_client_argumets_t*)argv)->cl;
	class pokroom* obj = ((struct get_info_from_client_argumets_t*)argv)->obj;

	free((struct get_info_from_client_argumets_t*)argv);

	struct msg_to_room_t msg_recive;
	set_msg_to_room_t(&msg_recive, 0, 0);


	while(1){
		if(msgrcv(cl->PokMesDis, (void*)(&msg_recive), sizeof(msg_recive.buf), i + MAX_CLIENT_NUM, 0) == -1){
			log(cl->fd_poker_log, "msgrcv on poker: %s\n", strerror(errno));
			return NULL;
		}
		log(cl->fd_poker_log, "recive info from %ld %ld %lf\n", cl->num[i], msg_recive.buf.comand, msg_recive.buf.rs);
		pthread_mutex_lock(mut);
		log(cl->fd_poker_log,  "mut lock %d\n", cl->PokMesDis);
		log(cl->fd_poker_log,  "login is %s\n", msg_recive.buf.login);
		switch(msg_recive.buf.comand){
			case CONECT:
				obj->conect(i, msg_recive.buf.login);
				obj->sendtoall();
				break;
			case REFRESH:
				obj->prsend(i);
				break;
			case GETMONEY:
				if(obj->status == ROOM_WAIT){
					obj->getmoney(i, msg_recive.buf.rs);
					obj->sendtoall();
				}
				break;
			case PUTMONEY:
				if(obj->status == ROOM_WAIT){
					obj->putmoney(i, msg_recive.buf.rs);
					obj->sendtoall();
				}
				break;
			case DISCONECT:
				obj->fold(i);
				obj->disconect(i);
				obj->sendtoall();
				break;
			case ACTION_CALL:
				obj->call(i);
				break;
			case ACTION_RAISE:
				obj->rais(i, msg_recive.buf.rs);
				break;
			case ACTION_FOLD:
				obj->fold(i);
				break;
			case ACTION_NEW:
				obj->new_game(i);
				obj->prsend(i);
				break;
			default:
				break;
		}
		int status = obj->status;
		obj->game();
		while(status != obj->status){
			status = obj->status;
			obj->game();
		}
		pthread_mutex_unlock(mut);
		log(cl->fd_poker_log, "mut unlock %d\n", cl->PokMesDis);
	}
	
	return NULL;
}

#pragma pack(push, 1)
struct get_info_from_server_argumets_t{
	int mds;
	int num;
	struct client* cl;
	class pokroom* obj;
	pthread_mutex_t* mut;
};
#pragma pack(pop)

void* get_info_from_server(void* argv){
	int mds = ((struct get_info_from_server_argumets_t*)argv)->mds;
	int num = ((struct get_info_from_server_argumets_t*)argv)->num;
	pthread_mutex_t* mut= ((struct get_info_from_server_argumets_t*)argv)->mut;
	struct client* cl = ((struct get_info_from_server_argumets_t*)argv)->cl;
	class pokroom* obj = ((struct get_info_from_server_argumets_t*)argv)->obj;
	free((struct get_info_from_server_argumets_t*)argv);

	struct msg_from_server_t msg_from_serv;
	msg_from_serv.num = 0;
	msg_from_serv.comand = 0;

	while(1){		
		if(msgrcv(mds, (void*)(&msg_from_serv), sizeof(msg_from_serv), FROM_SERVER_TO_TABLE + num, 0) == -1){
			log(cl->fd_poker_log, "msgrcv get_info_from_server: %s in room №%d\n", strerror(errno), num);
			goto out;
		}
		log(cl->fd_poker_log, "recive info from server - room %d\n", num);
		
		switch(msg_from_serv.comand){
			case SERVER_COMAND_SHOW_INFO:
				log(cl->fd_poker_log, "comand show info %d\n", num);
				pthread_mutex_lock(mut);
				obj->nead_send_info_to_server = true;
				obj->send_public_info();
				pthread_mutex_unlock(mut);
				break;
			case SERVER_COMAND_DONT_SHOW_INFO:
				log(cl->fd_poker_log, "comand dont show info %d\n", num);
				pthread_mutex_lock(mut);
				obj->nead_send_info_to_server = false;
				pthread_mutex_unlock(mut);
				break;
			case SERVER_COMAND_EXIT:
				goto out;
				break;
			default:
				break;
		}
	}
out:

	return NULL;
}

void* poker(void* argumets){
	int mds = ((struct poker_argumets_t*)argumets)->mds;
	struct client* cl = ((struct poker_argumets_t*)argumets)->cl;
	struct table_t* tb = ((struct poker_argumets_t*)argumets)->tb;
	int num = ((struct poker_argumets_t*)argumets)->num;
	FILE* file = ((struct poker_argumets_t*)argumets)->file;
	free(((struct poker_argumets_t*)argumets));

	char str[256];
	for(int i = 0; i < 256; i++){
		str[i] = 0;
	}

	sprintf(str, "log/PokerRoomLog%d.log", num);

//	pthread_mutex_lock(&(tb->mut_read_client));
	cl->fd_poker_log = open(str, O_CREAT|O_RDWR|O_APPEND, 0666);
	cl->offset = lseek(cl->fd_poker_log, 0, SEEK_END);
//	pthread_mutex_unlock(&(tb->mut_read_client));

	if(cl->fd_poker_log < 0){
		perror(str);
		return NULL;
	}
	log(cl->fd_poker_log, "--------------------------------------------------\n", num);
	log(cl->fd_poker_log, "open poker room %d correct\n", num);

	
	class pokroom obj(cl->PokMesDis, cl, num, file);

	pthread_t thread[7] = {0, 0, 0, 0, 0, 0, 0};
	void* argv;

	pthread_mutex_t mut_get_info_from_client;
	if(pthread_mutex_init(&mut_get_info_from_client, NULL)){
		log(cl->fd_poker_log, "mut_get_info_from_client - init: %s\n", strerror(errno));
		return NULL;
	}

	for(int i = 0; i < 6; i++){
		argv = (struct get_info_from_client_argumets_t*)malloc(sizeof(struct get_info_from_client_argumets_t));
		((struct get_info_from_client_argumets_t*) argv)->i = i;
		((struct get_info_from_client_argumets_t*) argv)->mut = &mut_get_info_from_client;
		((struct get_info_from_client_argumets_t*) argv)->cl = cl;
		((struct get_info_from_client_argumets_t*) argv)->obj = &obj;
		pthread_create(&(thread[i + 1]), NULL, get_info_from_client, argv);
	}

	argv = (struct get_info_from_server_argumets_t*)malloc(sizeof(struct get_info_from_server_argumets_t));
	((struct get_info_from_server_argumets_t*)argv)->mds = mds;
	((struct get_info_from_server_argumets_t*)argv)->num = num;
	((struct get_info_from_server_argumets_t*)argv)->cl = cl;
	((struct get_info_from_server_argumets_t*)argv)->obj = &obj;
	((struct get_info_from_server_argumets_t*)argv)->mut = &mut_get_info_from_client;
	pthread_create(&(thread[0]), NULL, get_info_from_server, (void*)argv);
	pthread_join(thread[0], NULL);

	pthread_mutex_lock(&(tb->mut_read_client));
	msgctl(cl->PokMesDis, IPC_RMID, NULL);
	cl->PokMesDis = -1;
	pthread_mutex_unlock(&(tb->mut_read_client));

	for(int i = 1; i < 7; i++){
		pthread_join(thread[i], NULL);
	}

	pthread_mutex_destroy(&mut_get_info_from_client);
	log(cl->fd_poker_log, "close poker room %d correct\n", num);

	if(cl->fd_poker_log >= 0){
		close(cl->fd_poker_log);
		cl->fd_poker_log = -1;
		cl->offset = 0;
	}
	return NULL;
}
