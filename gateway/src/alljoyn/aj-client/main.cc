/*
	当启动第二个客户时，也加入了客户1的session id中了，为什么 ？
	怎么让两个客户在不同的session中？

	数据类型时，什么时候用char，什么时候用unsigned char ?

*/

#include <cstdlib>
#include <cstring>

#include <iostream>
#include <time.h>
#include <signal.h>
#include <qcc/Debug.h>
#include <alljoyn/version.h>

#include "clientbusattachment.h"

#include "mybuslistener.h"
#include "mybusobject.h"
#include "common.h"

using namespace std;

ClientBusAttachment* clientbus = NULL;

MyBusObject* obj = NULL;
MyBusListener busListener;

const char* OBJ_PATH = "/";
const char* SERVICE_NAME = "org.service1";
const char* INTERFACE_NAME = "org.intf1";
SessionPort PORT = 250; // 不能加const修饰，否则在mybuslistner中extern时，链接出错

ajn::SessionId sessionId;
volatile sig_atomic_t g_joinComplete = false;

char macstr[13] = {'\0'};//mac地址

char dev_prop[1024] = "{\"Mac_address\":\"";
char** _resid;

void printHelp()
{
	printf("--------------------  The Program Flow   -------------------\n\n");
	printf("\t1.Create a server BusAttachment object 'clientbus'\n");
	printf("\t2.Create interfaces for 'clientbus'\n");
	printf("\t3.Register BusListener for 'clientbus'\n");
	printf("\t4.Start 'clientbus'\n");
	printf("\t5.Create BusObject 'myobj'\n");
	printf("\t6.Register 'myobj' for 'servicebus'\n");
	printf("\t7.Connect to router\n");
	printf("\t8.Start to find service and do work\n\n");
	printf("--------------------   End               -------------------\n\n");
}

void handle_prompt(int argc,char** argv)
{
	char tmp[64] = "";
	sprintf(tmp,"%d",argc-2);

	if (argc < 3) {
		printf("usage:ajtest_client [macaddr] [resnum] ...\n");
		exit(0);
	}

	_resid = (char**)malloc(sizeof(char) * (argc-2));
	strcpy(macstr,argv[1]);
	strcat(dev_prop,macstr);
	strcat(dev_prop,"\",\"Res_num\":");
	strcat(dev_prop,tmp);
	strcat(dev_prop,",\"flags\":0,\"Res\":[");
	for (int i=2; i<argc; i++) {	
		strcpy(tmp,"{\"Res_name\":\"\",\"Res_type\":\"\",\"Res_unit\":\"\",\"Res_port\":");
		strcat(tmp,argv[i]);
		strcat(tmp,"}");
		if (i != argc-1) {
			strcat(tmp,",");
			strcat(dev_prop,tmp);	
		}
		else {
			strcat(dev_prop,tmp);
		}

		*_resid = argv[i];
		//printf("%s\n",*_resid);
		_resid++;
	}
	strcat(dev_prop,"]}");
	printf("dev_prop: %s\n",dev_prop);

}

int main(int argc, char** argv)
{
	handle_prompt(argc,argv);

	printHelp();

	printf("%s\n",ajn::GetBuildInfo());
	QStatus status = ER_OK;

	clientbus = new ClientBusAttachment("myapp",true);
	clientbus->createInterface(INTERFACE_NAME);
	clientbus->RegisterBusListener(busListener);
	clientbus->startMessageBus();

    MyBusObject c_obj(*clientbus, OBJ_PATH);
	obj = &c_obj;

	clientbus->registerBusObject(*obj);

    clientbus->connectBusAttachment();
	
	clientbus->findAdvertisedName();
	clientbus->waitForJoinSessionCompletion();

	srand((unsigned)time(NULL));
	printf("\nstarting to send res data_info...\n");

	double val;
	while (ER_OK == status) {
		for (int i=0; i<argc-2; i++) {
			val = rand() % 100;
			obj->sendResdataSignal_2(atoi(*(_resid - (argc-2-i))),val);
		}
		
		printf("\n");
		
		//一个设备的所有资源数据发送一次后，定时一段时间再发送
#ifdef _WIN32
		Sleep(1000*40);
#else 
		sleep(40);
#endif
	}
    return 0;
}





