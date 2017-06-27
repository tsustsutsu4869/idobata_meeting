#include "mynet.h"
#include "idobata_meeting.h"

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
	int r;
	if((r=accept(s,addr,addrlen))== -1){
		exit_errmesg("accept()");
	}
	return(r);
}

int Send(int s, void *buf, size_t len, int flags)
{
	int r;
	if((r=send(s,buf,len,flags))== -1){
		exit_errmesg("send()");
	}
	return(r);
}

int Recv(int s, void *buf, size_t len, int flags)
{
	int r;
	if((r=recv(s,buf,len,flags))== -1){
		exit_errmesg("recv()");
	}else if(r == 0){ //この処理いる？
		printf("Server is down.\n");
		exit(EXIT_SUCCESS);
	}
	return(r);
}
