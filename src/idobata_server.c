#include "mynet.h"
#include "idobata_meeting.h"
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#ifdef __CYGWIN__
#include <sys/ioctl.h>
#endif


/*プライベート関数*/
static void action_client_acceptance(int signo);


/*プライベート変数*/
static int udp_sock;

void idobata_server(char *username, int port){
	int tcp_sock;

	struct sigaction action;
#ifdef __CYGWIN__
	int enable=1;
#endif

	/* UDP, TCPサーバの初期化 */
	udp_sock = init_udpserver( (in_port_t)port );
	tcp_sock = init_tcpserver(port, 5);

	/* シグナルハンドラを設定する */
	action.sa_handler = action_client_acceptance;
	if(sigfillset(&action.sa_mask) == -1){
		exit_errmesg("sigfillset()");
	}
	action.sa_flags = 0;
	if(sigaction(SIGIO, &action, NULL) == -1){
		exit_errmesg("sigaction()");
	}

	/* ソケットの所有者を自分自身にする */
	if(fcntl(udp_sock, F_SETOWN, getpid()) == -1){
		exit_errmesg("fcntl():F_SETOWN");
	}

	/* ソケットをノンブロッキング, 非同期モードにする */
#ifdef __CYGWIN__
	if(fcntl(sock, F_SETFL, O_NONBLOCK) == -1){
		exit_errmesg("fcntl():F_SETFL");
	}
	if(ioctl(sock, FIOASYNC, &enable) == -1){
		exit_errmesg("ioctl():FIOASYNC");
	}
#else
	if(fcntl(udp_sock, F_SETFL, O_NONBLOCK|O_ASYNC) == -1){
		exit_errmesg("fcntl():F_SETFL");
	}
#endif

	for(;;){
		/* 待ち時間に何かする */
		tcp_server_loop(tcp_sock, username);
	}

	close(udp_sock);



}


//UDP通信によりクライアントの接続を受け付ける関数
void action_client_acceptance(int signo){
	struct sockaddr_in from_adrs;
	socklen_t from_len;
	char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
	int strsize;
	idobata *packet;

	for(;;){
		/* プログラム起動時にブロードキャストされるパケットを受信する */
		from_len = sizeof(from_adrs);
		if((strsize=recvfrom(udp_sock, r_buf, R_BUFSIZE, 0, (struct sockaddr *)&from_adrs, &from_len)) == -1){
			if(errno == EWOULDBLOCK){
				break;
			}else{
				exit_errmesg("recvfrom()");
			}
		}

		//show_adrsinfo(&from_adrs);

		packet = (idobata *)r_buf; /* packetがバッファの先頭を指すようにする */
		/* HELOパケットを受け取ったらHEREパケットを送り返す */
		if(analize_header(packet->header) == HELLO)
		{
			strcpy(s_buf, create_packet(HERE, NULL));
			Sendto(udp_sock, s_buf, strlen(s_buf), 0, (struct sockaddr *)&from_adrs, sizeof(from_adrs));
			break;
		}
		//HELO以外が送られてきたときのエラー処理せんでいいかな？
	}

	//close(udp_sock);  /*これソケット閉じたらあかんよな。*/
}
