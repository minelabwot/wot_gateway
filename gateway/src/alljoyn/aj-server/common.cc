#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <cstdio>
#include "common.h"

#ifdef _WIN32
void init_winsock()
{
	int ret;
	WSADATA wsaData;

	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0) {
		printf("WSAStartup failed: %d\n", ret);
		exit(-1);
	}
}
#endif

/*
IN_ADDR get_addr_info(const char* hostname)
{
	int ret;
	char ipstr[16];
	struct addrinfo hint;
	struct addrinfo *ans,*cur;

	//ZeroMemory(&hint, sizeof(hint));
	memset(&hint,0,sizeof(hint));//利于跨平台，且必须清0
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	ret = getaddrinfo(hostname,NULL,&hint,&ans);
	if (ret != 0) {
		fprintf(stderr,"getaddrinfo failed:%s\n",gai_strerror(ret));
		exit(-1);
	}
	
	
	for (cur = ans; cur != NULL; cur = cur->ai_next) {
		inet_ntop(AF_INET,&(((struct sockaddr_in*)(cur->ai_addr))->sin_addr),
			ipstr,16);
		printf("%s\n",ipstr);
	}
	
	//假定返回的是第一个地址结构
	return ((struct sockaddr_in*)ans->ai_addr)->sin_addr;
}
*/


/*
	fd_cli:客户套接字描述符指针
	hostip:欲连接的远程的主机ip
*/
void create_client_sock(int* fd_cli,const char* hostip,int port)
{
	struct sockaddr_in cliaddr;//定义不能放后面
	int ret;

	/* 此函数已经过时
	host = gethostbyname("api.yeelink.com");
	if( host == NULL ) {
		printf("DNS failed\r\n");
		return;
	}*/

	*fd_cli = socket(AF_INET, SOCK_STREAM, 0);

	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(port);
	//cliaddr.sin_addr = get_addr_info(LCZ_HOST);

	inet_pton(AF_INET,hostip,(void*)&cliaddr.sin_addr);
	
	memset(&(cliaddr.sin_zero), 0, sizeof(cliaddr.sin_zero));

	ret = connect(*fd_cli, (struct sockaddr *)&cliaddr, sizeof(struct sockaddr));
	if( ret == 0 ){
		printf("connect %s:%d ok\n",hostip,port);
	}
	else {
		printf("connect to python server socket failed\n");
	}
}
