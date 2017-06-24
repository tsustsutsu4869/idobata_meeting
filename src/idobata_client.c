#include "mynet.h"
#include "idobata_meeting.h"


void idobata_client(char *username, char *server_adrs, int port){
	int sock;
	char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
	int strsize;
	fd_set mask, readfds;
	idobata *packet;

	/* サーバに接続する */
	sock = init_tcpclient(server_adrs, port);
	printf("Connected.\n");
	printf("Please send username to server.\n");

	strcpy(s_buf, create_packet(JOIN, username));
	Send(sock, s_buf, strlen(s_buf), 0);

	/* ビットマスクの準備 */
	FD_ZERO(&mask);  //maskをすべて０に初期化
	FD_SET(0, &mask); //0ビット目を０と設定。標準出力を監視
	FD_SET(sock, &mask);

	for(;;){
		/* 受信データの有無をチェック */
		readfds = mask;
		select( sock+1, &readfds, NULL, NULL, NULL );

		if( FD_ISSET(0, &readfds) ){ //真であればキーボードから入力があったことが分かる
			/* キーボードから文字列を入力する */
			//			int i;
			//			for(i = 0; i <= strsize; i++){  //s_bufを初期化
			//				s_buf[i] = '\0';
			//			}
			fgets(s_buf, S_BUFSIZE, stdin);

			if(strcmp(s_buf, "QUIT") == 0){
				strcpy(s_buf, create_packet(QUIT, NULL));
				Send(sock, s_buf, strlen(s_buf), 0);

				close(sock);
				printf("See you!\n");
			}else{ //発言メッセージとしてサーバに送信する
				strcpy(s_buf, create_packet(POST, s_buf));
				Send(sock, s_buf, strlen(s_buf), 0);
			}
		}

		if( FD_ISSET(sock, &readfds) ){ //真であればサーバからパケットが到着していることが分かる
			/* サーバから文字列を受信する */
			strsize = Recv(sock, r_buf, R_BUFSIZE-1, 0);
			r_buf[strsize] = '\0';  //いらん? strsize - 1にいれる？

			packet = (idobata *)r_buf; /* packetがバッファの先頭を指すようにする */

			/* MESGパケットを受け取ったらメッセージを表示 */
			if(analize_header(packet->header) == MESSAGE)
			{
				printf("%s \n", packet->data);
				fflush(stdout);
			}

		}
	}

}

//push
