#include "mynet.h"
#include "idobata_meeting.h"
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define TIMEOUT_SEC 3

char* start_idobata(int port){
	struct sockaddr_in broadcast_adrs;
	struct sockaddr_in from_adrs;
	socklen_t from_len;

	int sock;
	int broadcast_sw = 1;
	fd_set mask, readfds;
	struct timeval timeout;
   char s_buf[R_BUFSIZE], r_buf[R_BUFSIZE];
	int strsize;

	char *server_adrs;
	idobata *packet;

	/* ブロードキャストアドレスの情報をsockaddr_in構造体に格納する */
	set_sockaddr_in_broadcast(&broadcast_adrs, port);

	/* ソケットをDGRAMモードで作成する */
	sock = init_udpclient();

	/* ソケットをブロードキャスト可能にする */
	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *)&broadcast_sw, sizeof(broadcast_sw)) == -1){
		exit_errmesg("setsockopt()");
	}

	/* ビットマスクの準備 */
	FD_ZERO(&mask);
	FD_SET(sock, &mask);

	/* 受信データの有無をチェックするための準備 */
	readfds = mask;
	timeout.tv_sec = TIMEOUT_SEC;
	timeout.tv_usec = 0;

	/* HELOパケット作成 */
	strcpy(s_buf, create_packet(HELLO, s_buf));

	/* HELOパケットを3回ブロードキャストする */
	int count = 0;
	for(; count < 3; count++){
		Sendto(sock, s_buf, strlen(s_buf), 0, (struct sockaddr *)&broadcast_adrs, sizeof(broadcast_adrs) );

		if( select( sock+1, &readfds, NULL, NULL, &timeout ) == 0 ){
			printf("Time out[%d].\n", count);
			//fflush(stdout);
			continue;
		}else{
			from_len = sizeof(from_adrs);
			strsize = Recvfrom(sock, r_buf, R_BUFSIZE-1, 0, (struct sockaddr *)&from_adrs, &from_len);
			r_buf[strsize] = '\0';

			packet = (idobata *)r_buf; /* packetがバッファの先頭を指すようにする */
			if(analyze_header(packet->header) == HERE){
				printf("You are a client.\n");
				server_adrs = inet_ntoa(from_adrs.sin_addr);  //char server_adrs[256];　なら、strcpy(server, inet_ntoa(from_adrs.sin_addr));
				close(sock);
				return(server_adrs);
			}
		}
		//printf("[%s] %s",inet_ntoa(from_adrs.sin_addr), r_buf);
	}

	close(sock);             /* ソケットを閉じる */
	server_adrs = "server";
	printf("You are a server.\n");
	return(server_adrs);
}

