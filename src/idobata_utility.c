#include "mynet.h"
#include "idobata_meeting.h"

//これをutility内で定義するとこの中でしか使えないserver.cとかで使いたいならヘッダで定義する
//typedef struct{
//	char header[4];   /* パケットのヘッダ部(4バイト) */
//	char sep;         /* セパレータ(空白、またはゼロ) */
//	char data[];      /* データ部分(メッセージ本体) */
//}idobata;

/* プライベート関数 */  //ここでしか呼び出されない関数を宣言しなあかん
static char *chop_nl(char *s);


//受信メッセージの先頭のパケットによって決められた処理に分岐させる関数
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

/*第５引数には 相手先のアドレス 情報を格納するためのsockaddr構造体へのポインタを指定します。 （上の例では、実際には sockaddr_in構造体を使用するから sockaddr *に 型変換している） 第６引数は、socklen_t型変数へのポインタを指定しますが、この引数は 入力としても出力としても使用するパラメータであることに注意します。 すなわち、呼び出し時には第５引数で指定した構造体のサイズを指定しておき、 recvfrom()から戻ってきたときには、実際の相手先のアドレスのサイズが 入っています。

第５引数と第６引数は、どこから到着したパケットか調べる必要がなければ NULLにしておいても構いません。（例題の場合もNULLでよいのですが、使い方を 解説するために入れてあります。）
 *
 *
 *
 *
 * */






static char *chop_nl(char *s)
{
	int len;
	len = strlen(s);
	if( s[len-1] == '\n' ){
		s[len-1] = '\0';
	}
	return(s);
}

