#ifndef COMMON_H_
#define COMMON_H_

#include <alljoyn/Session.h>

#define CSHI_HOST		"cshi-PC"
#define CSHI_IP			"192.168.13.111"

#define LOCALNAME		"127.0.0.1"

#define	SOCKET_DEV_PROP_PORT		8000
#define SOCKET_RES_DATA_PORT		8001
#define SOCKET_PIC_DATA_PORT		8002

namespace MyNameSpace
{
	const ajn::SessionPort PORT = 250;
}

extern void init_winsock();
extern void create_client_sock(int* fd_cli,const char* hostip,int port);
extern void print_server_help();

#define MAX_IMAGE_SIZE	700*1024 //¼ÙÉèÍ¼Æ¬×î´ó700K
#endif