#include "mynet.h"
#include "idobata_meeting.h"
#include <errno.h>

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
	struct _imember *prev;        /* 次のユーザ */
} imember;


/*プライベート変数でここに置いといたら、このファイル内なら関数の引数としてわたさんでよくなる*/
/* プライベート変数 */
//static int Max_sd = 0;
//static int N_Client = 0;
static imember header;  //ダミー要素を用意
static imember *Login_client;
//static imember *Logout_client;

/* プライベート関数 */  //ここでしか呼び出されない関数を宣言しなあかん
//static char *chop_nl(char *s);
static void client_login(int sock, char *name);
static void client_logout(int sock);
static char *server_send_message(int socket, char *buf);


void tcp_server_loop(int sock, char *name){
	fd_set mask, readfds;
	int mask_flag = 0;
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

			if(analyze_header(packet->header) == JOIN){
				client_login(sock, name);
				//continue;  //いんのこれ
			}

		}
		if( FD_ISSET(0, &readfds) ){ //真であればキーボードから入力があったことが分かる
			fgets(s_buf, S_BUFSIZE, stdin);
			server_send_message(ALL, s_buf);
		}

		p = header.next;
		while(p != NULL){  //クライアントのリストを前からNULLまで参照して１回１回sockをマスクに格納して監視している
			if(mask_flag == 0){
				FD_SET(p->sock, &mask);
			}

			if( FD_ISSET(p->sock, &readfds) ){ //真であれば先ほど接続したばかりのクライアントからパケットが到着していることが分かる
				Recv(p->sock, r_buf, R_BUFSIZE-1, 0);  //strsizeいらんよな

				char *msg;
				packet = (idobata *)r_buf; /* packetがバッファの先頭を指すようにする */
				switch(analyze_header(packet->header)){
				   case POST:
					   msg = server_send_message(p->sock, packet->data);
					   printf("%s\n", msg);
					   break;
				   case QUIT:
					   client_logout(p->sock);
					   printf("%d logs out of the system.\n", p->sock);
					   break;
				}
			}

			if(mask_flag == 0){
				mask_flag++;
			}
			p = p->next;
		}

	}
}


/*クライアントのログイン処理をする関数*/
void client_login(int sock, char *name){
	//双方向リスト使おう。
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

//	Max_sdを返す？
}


/*クライアントのログアウト処理をする関数*/
void client_logout(int sock){
	/*リストから指定されたソケットを削除してつなぎ直す*/
}


/* 全てのクライアントにメッセージを送信する関数（第一引数で指定されたクライアントを除いて） */
char *server_send_message(int socket, char *buf){  //リスト処理意味不明やから見直して
	imember *p = header.next;

	static char tag_username[L_USERNAME + S_BUFSIZE + 2] = "[username]";  //初期化できてるかあやしい  関数がローカルの変数のアドレスを返していますって起こられるからstaticつけた
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

	return tag_username;
}


///* 文字列の末尾に改行コードがあれば取り除く関数 */
//char *chop_nl(char *s){
//	int len;
//	len = strlen(s);
//	if( s[len-1] == '\n' ){
//		s[len-1] = '\0';
//	}
//	return(s);
//}
