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

	extern struct sockaddr_in server;
	extern WSADATA wsaData;
	extern SOCKET cs;
	extern int alive_;

	typedef struct c_i {
		char* ip;
		int port;
	} i_tup;

	int es_init();
	BOOL alive();
	int es_connect(const char* ip, int port);
	int es_send(const char* msg);
	response es_recv(int rcv_s);
	int es_cleanup();


#ifdef __cplusplus
}
#endif
#endif
