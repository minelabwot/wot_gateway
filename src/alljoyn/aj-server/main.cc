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

int fd_cli;
int fd_resdata;

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

void parse_prompt(int argc,char** argv)
{
	if (argc < 2) {
		printf("arg error\nclient.exe [host] {port}\n");
		exit(1);
	}
	host = argv[1];
	port = 0;

	if (argc == 3)
		port = atoi(argv[2]);

}

int main(int argc, char** argv)
{	
	printHelp();

	parse_prompt(argc,argv);

#ifdef _WIN32
	init_winsock();//必须初使化
#endif

	if (port == 0) {
		create_client_sock(&fd_cli,host,SOCKET_DEV_PROP_PORT);//连接python服务端
		create_client_sock(&fd_resdata,host,SOCKET_RES_DATA_PORT);
	}
	else {
		create_client_sock(&fd_cli,host,port);//连接python服务端
		create_client_sock(&fd_resdata,host,port);
	}

    QStatus status = ER_OK;
    printf("%s\n",ajn::GetBuildInfo());

	servicebus = new ServiceBusAttachment("myapp",true);
	servicebus->createInterface(INTERFACE_NAME);
   	servicebus->RegisterBusListener(busListener);
    servicebus->Start();

    MyBusObject myobj(*servicebus, OBJ_PATH);
    s_obj = &myobj;

    servicebus->RegisterBusObject(*s_obj);
    status = servicebus->Connect();
    if (ER_OK == status) {
        printf("Connect to '%s' succeeded.\n", servicebus->GetConnectSpec().c_str());
	} else {
		printf("Failed to connect to '%s' (%s).\n", servicebus->GetConnectSpec().c_str(), QCC_StatusText(status));
	}

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
    return (int) status;
}






