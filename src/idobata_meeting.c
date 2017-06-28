#include "mynet.h"
#include "idobata_meeting.h"
#include <sys/select.h> //この下３ついらん？
#include <sys/time.h>
#include <arpa/inet.h>

#define SERVER_LEN 256     /* サーバ名格納用バッファサイズ */
#define DEFAULT_PORT 50001 /* ポート番号既定値 */
#define DEFAULT_NCLIENT 3  /* クライアント数 */
#define NAMELENGTH 15 /* 名前の長さ制限 */

//#define S_BUFSIZE 100   /* 送信用バッファサイズ */
//#define R_BUFSIZE 100   /* 受信用バッファサイズ */


extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char *argv[]) {

	int port_number = DEFAULT_PORT;
	char *server_adrs;
	char username[NAMELENGTH];
	int c;

	/* オプション文字列の取得 */
	opterr = 0;
	while( 1 ){
		c = getopt(argc, argv, "n:p:h");
		if( c == -1 ) break;

		switch( c ){
		case 'n' :  /* ユーザ名の指定 */
			snprintf(username, SERVER_LEN, "%s", optarg);
			break;

		case 'p':  /* ポート番号の指定 */
			port_number = atoi(optarg);
			break;

		case '?' :
			fprintf(stderr,"Unknown option '%c'\n", optopt );
		case 'h' :
			fprintf(stderr,"Usage: %s -n user_name -p port_number\n", argv[0]);
			exit(EXIT_FAILURE);
			break;
		}
	}

	printf("name:%s,port:%d\n",username, port_number);
	fflush(stdout);

	server_adrs = start_idobata(port_number);  //サーバかクライアントかを判断する関数modeにcかsを代入
	server_adrs = "localhost";  //クライアントを走らせたいから書いてるだけ。サーバできたら削除して

	if(strncmp(server_adrs, "server", 6) == 0){
		printf("server\n");
		idobata_server(username, port_number);
	}else{
		idobata_client(username, server_adrs, port_number);
	}

	return EXIT_SUCCESS;
}


