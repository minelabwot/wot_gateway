#ifndef COMMON_H_
#define COMMON_H_

#include <alljoyn/Session.h>

#define CSHI_HOST		"cshi-PC"
#define CSHI_IP			"192.168.13.111"

// ¡ı¥ø’√PC
#define LCZ_HOST		"seven-Lenovo"
#define LCZ_IP			"192.168.13.118"

#define MICROWRT_IP		"192.168.13.102"

#define LOCALNAME		"127.0.0.1"

#define	SOCKET_DEV_PROP_PORT		8000
#define SOCKET_RES_DATA_PORT		8001
#define SOCKET_PIC_DATA_PORT		8002

struct data_info {
	char* mac;
	uint8_t __resid;
	double val;
};

namespace MyNameSpace
{
	const ajn::SessionPort PORT = 250;
}

extern void init_winsock();
extern void create_client_sock(int* fd_cli,const char* hostip,int port);

#endif