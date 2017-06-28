#include "idobata_meeting.h"

#define MESSAGE_BUFSIZE MESG_BUFSIZE+L_USERNAME+6	/* 発言メッセージサイズ */

static char Buffer[MESSAGE_BUFSIZE];
/*
  パケットの種類=type のパケットを作成する
  パケットのデータは 内部的なバッファ(Buffer)に作成される
*/
//各種のパケットを作成する関数
char *create_packet(u_int32_t type, char *message ){

  switch(type){
  case HELLO:
    snprintf( Buffer, MESSAGE_BUFSIZE, "HELO" );
    break;
  case HERE:
    snprintf( Buffer, MESSAGE_BUFSIZE, "HERE" );
    break;
  case JOIN:
    snprintf( Buffer, MESSAGE_BUFSIZE, "JOIN %s", message );
    break;
  case POST:
    snprintf( Buffer, MESSAGE_BUFSIZE, "POST %s", message );
    break;
  case MESSAGE:
    snprintf( Buffer, MESSAGE_BUFSIZE, "MESG %s", message );
    break;
  case QUIT:
    snprintf( Buffer, MESSAGE_BUFSIZE, "QUIT" );
    break;
  default:
    /* Undefined packet type */
    break;
  }
  return Buffer;
}


//パケットのヘッダを解析する関数
u_int32_t analyze_header(char *header){
  if( strncmp( header, "HELO", 4 )==0 ){
	  return(HELLO);
  }
  if( strncmp( header, "HERE", 4 )==0 ){
	  return(HERE);
  }
  if( strncmp( header, "JOIN", 4 )==0 ){
	  return(JOIN);
  }
  if( strncmp( header, "POST", 4 )==0 ){
	  return(POST);
  }
  if( strncmp( header, "MESG", 4 )==0 ){
	  return(MESSAGE);
  }
  if( strncmp( header, "QUIT", 4 )==0 ){
	  return(QUIT);
  }

  return 0;
}


