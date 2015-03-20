/*

	1、作AllJoyn服务端，运行在PC或openwrt上，接收来自AllJoyn客户端的请求
	2、同时作socket客户端，向Python服务端发送JSON格式的设备配置和资源数据

 */

#include <cstdio>
#include <alljoyn/version.h>
#include "common.h"
#include "mybusobject.h"
#include "mybuslistener.h"
#include "servicebusattachment.h"


// 定义全局变量
MyBusObject* s_obj = NULL;
ServiceBusAttachment* servicebus = NULL;
MyBusListener busListener;

const char* OBJ_PATH = "/";
const char* SERVICE_NAME = "org.service1";
const char* INTERFACE_NAME = "org.intf1";

int fd_dev_prop;
int fd_resdata;
int fd_picdata;

void printHelp()
{
	printf("--------------------  The Program Flow   -------------------\n\n");
	printf("\t1.Create a server BusAttachment object 'servicebus'\n");
	printf("\t2.Create interfaces for 'servicebus'\n");
	printf("\t3.Register BusListener for 'servicebus'\n");
	printf("\t4.Start 'servicebus'\n");
	printf("\t5.Create BusObject 'myobj'\n");
	printf("\t6.Register 'myobj' for 'servicebus'\n");
	printf("\t7.Connect to router\n");
	printf("\t8.Start to advertise service\n\n");
	printf("--------------------   End               -------------------\n\n");
}

const char* host;
int port;

char *pData;

int main(int argc, char** argv)
{	
	printHelp();
	pData = (char*)malloc(600*1024);
	if (pData == NULL) {
		printf("malloc pData failed\n");
	}
	memset(pData,0,600*1024);

#ifdef _WIN32
	init_winsock();//必须初使化
#endif

	//连接python服务端
	create_client_sock(&fd_dev_prop,LOCALNAME,SOCKET_DEV_PROP_PORT);//设备属性端
	create_client_sock(&fd_resdata,LOCALNAME,SOCKET_RES_DATA_PORT);//资源数据端
	create_client_sock(&fd_picdata,LOCALNAME,SOCKET_PIC_DATA_PORT);//摄像头socket端
	
    QStatus status = ER_OK;

	servicebus = new ServiceBusAttachment("myapp",true);
	servicebus->createInterface(INTERFACE_NAME);

   	//servicebus->RegisterBusListener(busListener);//加入SessionMemberRemoved之后就不能在这里注册监听了

    servicebus->Start();
	
    s_obj = new MyBusObject(*servicebus, OBJ_PATH);
    servicebus->RegisterBusObject(*s_obj);

    servicebus->Connect();

	const TransportMask SERVICE_TRANSPORT_TYPE = TRANSPORT_ANY;
	servicebus->advertiseService(SERVICE_NAME,SERVICE_TRANSPORT_TYPE,busListener);
	   
	printf("waiting client to join...\n");
    while (1) {
#ifdef _WIN32
		Sleep(200);
#else
		sleep(1);
#endif
	}

	delete s_obj;
	delete servicebus;
    return (int) status;
}






