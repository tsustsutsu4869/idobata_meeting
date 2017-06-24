#include "mynet.h"
#include "idobata_meeting.h"

void idobata_server(char *username, int port){
	int sock;
	struct sockaddr_in from_adrs;
	socklen_t from_len;
	char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
	int strsize;
	fd_set mask, readfds;
	idobata *packet;

	/* UDPサーバの初期化 */
	sock = init_udpserver( (in_port_t)port );

	for(;;){
		/* 文字列をクライアントから受信する */
		from_len = sizeof(from_adrs);
		strsize = Recvfrom(sock, r_buf, R_BUFSIZE, 0,	(struct sockaddr *)&from_adrs, &from_len);

		//show_adrsinfo(&from_adrs);

		packet = (idobata *)r_buf; /* packetがバッファの先頭を指すようにする */

					/* MESGパケットを受け取ったらメッセージを表示 */
					if(analize_header(packet->header) == HELLO)
					{
						strcpy(s_buf, create_packet(HERE, NULL));
						Sendto(sock, s_buf, strlen(s_buf), 0, (struct sockaddr *)&from_adrs, sizeof(from_adrs));
						break;
					}
	}

	close(sock);

}
