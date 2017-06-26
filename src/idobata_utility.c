#include "mynet.h"
#include "idobata_meeting.h"
#include <errno.h>

#define L_USERNAME 15	/* ユーザ名サイズ */
#define ALL 0	/* 全てのクライアントを表す */

//これをutility内で定義するとこの中でしか使えないserver.cとかで使いたいならヘッダで定義する
//typedef struct{
//	char header[4];   /* パケットのヘッダ部(4バイト) */
//	char sep;         /* セパレータ(空白、またはゼロ) */
//	char data[];      /* データ部分(メッセージ本体) */
//}idobata;

typedef struct _imember {
	char username[L_USERNAME];     /* ユーザ名 */
	int  sock;                     /* ソケット番号 */
	struct _imember *next;        /* 次のユーザ */
} imember;


/*プライベート変数でここに置いといたら、このファイル内なら関数の引数としてわたさんでよくなる*/
/* プライベート変数 */
static int Max_sd = 0;
static int N_Client = 0;
static imember header;  //ダミー要素を用意
static imember *Login_client;
static imember *Logout_client;

/* プライベート関数 */  //ここでしか呼び出されない関数を宣言しなあかん
static char *chop_nl(char *s);
static void client_login(int sock, char *name);
static void client_logout(int sock);
static void server_send_message(int socket, char *buf);



//受信メッセージの先頭のパケットによって決められた処理に分岐させる関数
//この関数はもう使わんけど、他のとこでパケット処理書き終わるまで参考のため消してない
//void process_infulenced_packet(int socket, char buf[R_BUFSIZE]){
//	//	idobata *packet;
//	packet = (idobata *)buf; /* packetがバッファの先頭を指すようにする */
//
//	switch( analyze_header(packet->header) ){ /* ヘッダに応じて分岐 */
//	case HELLO:
//		/* HELOパケットを受けとった時の処理はstart_idobata.cに記載済みなので、ここでは何もしない */
//		break;
//	case HERE: //クライアント側にHEREパケットを送り返す処理
//		//from_len = sizeof(from_adrs);
//		/* 文字列をクライアントに送信する */
//		Sendto(socket, packet->header, strlen(packet->header), 0, NULL, NULL);
//		break;
//	case JOIN: //サーバ側がJOINパケットを受け取ったときの処理
//		chop_nl( packet->data ); /* Usernameに改行があれば除く */
//		/* 以下、新規メンバー登録処理 */
//		break;
//	case POST:
//
//		break;
//	case MESSAGE:
//		break;
//	case QUIT:
//
//		break;
//		/* 以下、省略 */
//	}
//}

void tcp_server_loop(int sock, char *name){
	fd_set mask, readfds;
	char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];

	imember *p = header.next;
	idobata *packet;


	/* ビットマスクの準備 */
	FD_ZERO(&mask);  //maskをすべて０に初期化
	FD_SET(0, &mask); //0ビット目を０と設定。標準出力を監視
	FD_SET(sock, &mask);

	for(;;){
		/* 受信データの有無をチェック */
		readfds = mask;
		select( sock+1, &readfds, NULL, NULL, NULL );

		if( FD_ISSET(sock, &readfds) ){ //真であれば先ほど接続したばかりのクライアントからパケットが到着していることが分かる
			Recv(sock, r_buf, R_BUFSIZE-1, 0);  //strsizeいらんよな

			packet = (idobata *)r_buf; /* packetがバッファの先頭を指すようにする */
			/* MESGパケットを受け取ったらメッセージを表示 */
			if(analize_header(packet->header) == JOIN){  //なんでここビルド通らんの～
				client_login(sock, name);
				//continue;  //いんのこれ
			}

		}
		if( FD_ISSET(0, &readfds) ){ //真であればキーボードから入力があったことが分かる
			fgets(s_buf, S_BUFSIZE, stdin);
			server_send_message(ALL, s_buf);
		}

		while(header.next != NULL){  //クライアントのリストを前からNULLまで参照して１回１回sockをマスクに格納して監視している

		}

	}

}

/*クライアントのログイン処理*/
void client_login(int sock, char *name){

	imember *tail = &header;
	header.next = NULL;

	/* クライアント情報の保存用構造体の初期化 */
	if( (Login_client=(imember *)malloc(sizeof(imember)))==NULL ){
		exit_errmesg("malloc()");
	}

	strcpy(Login_client->username, name);
	Login_client->sock = sock;

	Login_client->next = NULL;
	tail->next = Login_client;
	tail = Login_client;
}


void client_logout(int sock){

}

/* 全てのクライアントにメッセージを送信する関数（第一引数で指定されたクライアントを除いて） */
void server_send_message(int socket, char *buf){  //リスト処理意味不明やから見直して
	imember *p = header.next;

	char tag_username[L_USERNAME + S_BUFSIZE + 2] = "[username]";  //初期化できてるかあやしい
	strcat(tag_username, buf);  //文字を連結してsend_msgに格納

	buf = create_packet(MESSAGE, tag_username);

	/* 指定クライアント以外は送信 */
	while(p != NULL){
		if(p->sock != socket){
			Send(p->sock, buf, strlen(buf), MSG_NOSIGNAL);
			if(errno == EPIPE){
				client_logout(p->sock);
				break;
			}else{
				exit_errmesg("recvfrom()");
			}
		}
		p = p->next;
	}

}


static char *chop_nl(char *s)
{
	int len;
	len = strlen(s);
	if( s[len-1] == '\n' ){
		s[len-1] = '\0';
	}
	return(s);
}



// * 	/* ビットマスクの準備 */
//	FD_ZERO(&mask);  //maskをすべて０に初期化
//
//
//	for(client_id=0; client_id<N_client; client_id++){  /* 3つのクライアントそれぞれのビットマスクの準備 */
//		FD_SET(Client[client_id].sock, &mask);
//	}
//
//	for(;;){
//		for(client_id = 0; client_id < N_client; client_id++){
//
//		}
//	}
//	/* 受信データの有無をチェック */
//	readfds = mask;
//	select( sock+1, &readfds, NULL, NULL, NULL );
//
//	if( FD_ISSET(0, &readfds) ){ //真であればキーボードから入力があったことが分かる
//		/* キーボードから文字列を入力する */
//		//			int i;
//		//			for(i = 0; i <= strsize; i++){  //s_bufを初期化
//		//				s_buf[i] = '\0';
//		//			}
//		fgets(s_buf, S_BUFSIZE, stdin);
//
//		if(strcmp(s_buf, "QUIT") == 0){
//			strcpy(s_buf, create_packet(QUIT, NULL));
//			Send(sock, s_buf, strlen(s_buf), 0);
//
//			close(sock);
//			printf("See you!\n");
//		}else{ //発言メッセージとしてサーバに送信する
//			strcpy(s_buf, create_packet(POST, s_buf));
//			Send(sock, s_buf, strlen(s_buf), 0);
//		}
//	}
//
//	if( FD_ISSET(sock, &readfds) ){ //真であればサーバからパケットが到着していることが分かる
//		/* サーバから文字列を受信する */
//		strsize = Recv(sock, r_buf, R_BUFSIZE-1, 0);
//		r_buf[strsize] = '\0';  //いらん? strsize - 1にいれる？
//
//		packet = (idobata *)r_buf; /* packetがバッファの先頭を指すようにする */
//
//		/* MESGパケットを受け取ったらメッセージを表示 */
//		if(analize_header(packet->header) == MESSAGE)
//		{
//			printf("%s \n", packet->data);
//			fflush(stdout);
//		}
//
//	}
//
