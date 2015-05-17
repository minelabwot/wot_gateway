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

struct res *root;

void handle_prompt(int argc,char** argv)
{
	char tmp[128] = "";
	sprintf(tmp,"%d",argc-1);

	if (argc < 2) {
		printf("usage:ajtest_client {resnum:restype} ...\n");
		exit(0);
	}
	struct res *pRes = (struct res *)malloc(sizeof(struct res) * (argc-1));
	root = pRes;

	if (pRes == NULL) {
		printf("malloc failed\n");
		return;
	}

	strcat(dev_prop,macstr);
	strcat(dev_prop,"\",\"Res_num\":");
	strcat(dev_prop,tmp);
	strcat(dev_prop,",\"flags\":0,\"Res\":[");

	for (int i=1; i<argc; i++) {
		char restmp[10];
		
		std::string tmpstr(argv[i]);
		int pos = tmpstr.find(':');
		std::string type = tmpstr.substr(pos+1);
		std::string id = tmpstr.substr(0,pos);

		pRes->resnum = atoi(id.c_str());
		strcpy(tmp,"{\"Res_name\":\"\",\"Res_port\":");
		strcat(tmp,id.c_str());
		strcpy(restmp,type.c_str());
		strcpy(pRes->type,type.c_str());

		strcat(tmp,",\"Res_unit\":\"\",\"Res_type\":\"");
		strcat(tmp,restmp);
		strcat(tmp,"\"}");

		if (i != argc-1) {
			strcat(tmp,",");
			strcat(dev_prop,tmp);	
		}
		else {
			strcat(dev_prop,tmp);
		}
		++pRes;
	}
	strcat(dev_prop,"]}");
	printf("dev_prop: %s\n",dev_prop);
}

int main(int argc, char** argv)
{
	get_mac(macstr);

	handle_prompt(argc,argv);

	//print_client_help();

	//printf("%s\n",ajn::GetBuildInfo());
	QStatus status = ER_OK;

	clientbus = new ClientBusAttachment("aj_client",true);
	clientbus->createInterface(INTERFACE_NAME);
	clientbus->RegisterBusListener(busListener);
	clientbus->startMessageBus();

	obj = new MyBusObject(*clientbus, OBJ_PATH);

	clientbus->registerBusObject(*obj);

    clientbus->connectBusAttachment();
	
	clientbus->findAdvertisedName();
	clientbus->waitForJoinSessionCompletion();

	srand((unsigned)time(NULL));
	printf("\nstarting to send res data_info...\n");

	double val;
	while (ER_OK == status) {
		for (int i=0; i<argc-1; i++) {

			if (strcmp(root[i].type,"temp") == 0 || strcmp(root[i].type,"light") == 0 ){
#if _WIN32
				//如果在WIN下，传感器用随机值
				val = rand() % 100;
#else
				//这里会有阻塞
				val = get_sensor_data();
#endif
				obj->sendResdataSignal(root[i].resnum,val);//发送传感器值
			}

			//如果是摄像头或照相机
			else if (strcmp(root[i].type,"monitor") == 0) {
#ifdef _WIN32
				std::string filename = "image\\";
#else
				std::string filename = "image/";
#endif
				//获取摄像头驱动产生的在image目录下的图片
				char resid[3];
				sprintf(resid,"%d",root[i].resnum);
				filename.append(resid);
				filename.append(".jpg");

				//printf("debug filename:%s\n",filename.c_str());
				obj->sendPicSignal(filename.c_str());

			}
			else {
				//当传感器类型不匹配时，作测试用
				printf("sensor type input error\n");
				val = rand() % 100;
				obj->sendResdataSignal(root[i].resnum,val);//发送传感器测试值
			}
		}

		printf("\n");
		
		//一个设备的所有资源数据发送一次后，定时一段时间再发送
#ifdef _WIN32
		Sleep(1000*30);
#else
		sleep(10);
#endif
	}
    return 0;
}





