#include "mynet.h"
#include "idobata_meeting.h"

#define L_USERNAME 15	/* ユーザ名サイズ */

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
} *imember;


/*プライベート変数でここに置いといたら、このファイル内なら関数の引数としてわたさんでよくなる*/

/* プライベート関数 */  //ここでしか呼び出されない関数を宣言しなあかん
static char *chop_nl(char *s);
static void init_client(int sock);



//受信メッセージの先頭のパケットによって決められた処理に分岐させる関数
//この関数はもう使わんけど、他のとこでパケット処理書き終わるまで参考のため消してない
void process_infulenced_packet(int socket, char buf[R_BUFSIZE]){
	//	idobata *packet;
	packet = (idobata *)buf; /* packetがバッファの先頭を指すようにする */

	switch( analyze_header(packet->header) ){ /* ヘッダに応じて分岐 */
	case HELLO:
		/* HELOパケットを受けとった時の処理はstart_idobata.cに記載済みなので、ここでは何もしない */
		break;
	case HERE: //クライアント側にHEREパケットを送り返す処理
		//from_len = sizeof(from_adrs);
		/* 文字列をクライアントに送信する */
		Sendto(socket, packet->header, strlen(packet->header), 0, NULL, NULL);
		break;
	case JOIN: //サーバ側がJOINパケットを受け取ったときの処理
		chop_nl( packet->data ); /* Usernameに改行があれば除く */
		/* 以下、新規メンバー登録処理 */
		break;
	case POST:

		break;
	case MESSAGE:
		break;
	case QUIT:

		break;
		/* 以下、省略 */
	}
}

void tcp_server_loop(int sock, char *name){
	fd_set mask, readfds;
	char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];

	/* ビットマスクの準備 */
	FD_ZERO(&mask);  //maskをすべて０に初期化
	FD_SET(0, &mask); //0ビット目を０と設定。標準出力を監視
	FD_SET(sock, &mask);

	for(;;){
		/* 受信データの有無をチェック */
		readfds = mask;
		select( sock+1, &readfds, NULL, NULL, NULL );

		if( FD_ISSET(sock, &readfds) ){ //真であれば先ほど接続したばかりのクライアントからパケットが到着していることが分かる
			client_login(sock, name);
		}
		if( FD_ISSET(0, &readfds) ){ //真であればキーボードから入力があったことが分かる
			server_send_message();
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

		while(){  //クライアントのリストを前からNULLまで参照して１回１回sockをマスクに格納して監視している

		}

	}

}

/*クライアントのログイン処理*/
void client_login(int sock, char *name){
	fd_set mask, readfds;
	idobata *packet;

	/* クライアント情報の保存用構造体の初期化 */
	if( (Client=(client_info *)malloc(N_client*sizeof(client_info)))==NULL ){
		exit_errmesg("malloc()");
	}


}


void server_send_message(){

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
