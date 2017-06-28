/*
 * idobata_meeting.h
 *
 *  Created on: 2017/06/21
 *      Author: b5122030
 */

#ifndef IDOBATA_MEETING_H_
#define IDOBATA_MEETING_H_

#include "mynet.h"
#include <stdint.h>

/* パケットの種類を表す */
#define HELLO 1
#define HERE 2
#define JOIN 3
#define POST 4
#define MESSAGE 5
#define QUIT 6

#define S_BUFSIZE 512	/* 送信用バッファサイズ */
#define R_BUFSIZE 512	/* 受信用バッファサイズ */
#define MESG_BUFSIZE 488	/* 発言メッセージサイズ */
#define L_USERNAME 15	/* ユーザ名サイズ */

typedef struct{
	char header[4];   /* パケットのヘッダ部(4バイト) */
	char sep;         /* セパレータ(空白、またはゼロ) */
	char data[];      /* データ部分(メッセージ本体) */
}idobata;


char* start_idobata(int port);

void idobata_server(char *username, int port);

void tcp_server_loop(int sock, char *name);

void idobata_client(char *username, char *server_adrs, int port);

void process_infulenced_packet(int socket, char *buf);

char *create_packet(u_int32_t type, char *message);

u_int32_t analyze_header(char *header);

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);

int Send(int s, void *buf, size_t len, int flags);

int Recv(int s, void *buf, size_t len, int flags);


#endif /* IDOBATA_MEETING_H_ */
