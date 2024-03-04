#pragma once

#ifndef ES2_H
#define ES2_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include<winsock2.h>
#include <windows.h>
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ENCODED 1

#pragma comment(lib,"ws2_32.lib")

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct response {
		char* data;
		int buf_sz;
	} response;

	extern struct sockaddr_in server, host, client;
	extern WSADATA wsaData;
	extern SOCKET ss, conns, cs;
	extern char rep[10000];
	extern int alive_;

	typedef struct c_i {
		char* ip;
		int port;
	} i_tup;

	int es_init();
	BOOL alive();
	int es_bind(int port);
	int es_listen(int nc);
	i_tup es_accept();
	int es_connect(const char* ip, int port);
	int es_send(const char* msg, int mode);
	response es_recv(int rcv_s, int mode, int timeout);
	int es_cleanup();


#ifdef __cplusplus
}
#endif
#endif