#ifdef _WIN32
#include <WinSock2.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#endif

#include "mybusobject.h"
#include "common.h"

#include <cstdio>
#include <string>
#include <fstream>

//#include <qcc/platform.h>

using namespace std;

extern const char* INTERFACE_NAME;
extern SessionId g_sessionId;

extern int fd_dev_prop;
extern int fd_resdata;
extern int fd_picdata;

uint8_t n;//作为第三个消息参数，即结构体数组的元素个数，如果作局部变量，VS会报堆栈错误，所以调整为全局变量
uint8_t n_resdata;

string DevProp;

extern char *pData;

MyBusObject::MyBusObject(BusAttachment& bus, const char* path) : BusObject(path),prop_name("")
{
	QStatus status;
	outputStream = NULL;
	const InterfaceDescription* intfDesc = bus.GetInterface(INTERFACE_NAME);
	AddInterface(*intfDesc);

	/* Store the Chat signal member away so it can be quickly looked up when signals are sent */
	devPropSigMember = intfDesc->GetMember("sig_dev_prop");
	resDataSigMember = intfDesc->GetMember("sig_resdata");
	picDataSigMember = intfDesc->GetMember("sig_pic");

	// 古宗海添加，为适应瘦客户端
	const MethodEntry methodEntries[] = {
		{ intfDesc->GetMember("dev_prop"),static_cast<MessageReceiver::MethodHandler>(&MyBusObject::dev_prop_handler) },
		{ intfDesc->GetMember("res_data"),static_cast<MessageReceiver::MethodHandler>(&MyBusObject::res_data_handler) }
	};
	AddMethodHandlers(methodEntries,sizeof(methodEntries)/sizeof(methodEntries[0]));
	
	//注册三个信号处理函数
	status =  bus.RegisterSignalHandler(this,
		static_cast<MessageReceiver::SignalHandler>(&MyBusObject::devPropHandler),
		devPropSigMember,
		NULL);

	status =  bus.RegisterSignalHandler(this,
		static_cast<MessageReceiver::SignalHandler>(&MyBusObject::sensorDataHandler),
		resDataSigMember,
		NULL);
	
	status =  bus.RegisterSignalHandler(this,
		static_cast<MessageReceiver::SignalHandler>(&MyBusObject::cameraDataHandler),
		picDataSigMember,
		NULL);

	if (ER_OK != status) {
		printf("Failed to register signal handler for ChatObject::Chat (%s)\n", QCC_StatusText(status));
	}
}

/*
void MyBusObject::ObjectRegistered()
{
	BusObject::ObjectRegistered();
	printf("object registered\n");
}

QStatus MyBusObject::Get(const char* ifcName, const char* propName, MsgArg& val)
{
	// get property of interface
	QStatus status;
	if (0 == strcmp("a property name",propName)) {
		val.typeId = ALLJOYN_STRING;
		val.v_string.str = prop_name.c_str();
		val.v_string.len = prop_name.length();
	}
	else {
		status = ER_BUS_NO_SUCH_PROPERTY;
	}
	return status;
}

QStatus MyBusObject::Set(const char* ifcName, const char* propName, MsgArg& val)
{
	// set property
	QStatus status;
	if ((0 == strcmp("a property name",propName)) && (val.typeId == ALLJOYN_STRING)) {
		prop_name = val.v_string.str;
	}
	else {
		status = ER_BUS_NO_SUCH_PROPERTY;
	}
	return status;
}
*/
/*
//用结构体实现解析
void MyBusObject::testSigHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg)
{		
	printf("\nparsing dev_info...\n");
	const MsgArg* arg1 = msg->GetArg(0);
	const MsgArg* arg2 = msg->GetArg(1);
	uint8_t* arg2_array;

	//int num = arg1->v_struct.numMembers;//arg1，结构体参数的元素个数
	//printf("num:%d\n",num);

	const char* ele1_macstr = arg1->v_struct.members->v_string.str;
	uint8_t ele2_rescnt = (arg1->v_struct.members + 1)->v_byte;
	printf("macaddr:%s, rescnt:%d, ",ele1_macstr,ele2_rescnt);
	// 可以尝试解析arg1的第三个参数

	arg2->Get("aq",&n,&arg2_array);//获取信号消息的第二个参数，是个资源id数组
	printf("res id: ");
	for (int i=0; i<n; i++) {
		printf(" %d,",*(arg2_array + i));
	}

	// 发送设备mac地址
	printf("\nsending dev_info...\n");
	int nsend1 = send(fd_cli,ele1_macstr,12,0);
	printf("macaddr nsend1:%d\n",nsend1);

	// 发送资源数目
	int nsend2 = send(fd_cli,(const char*)&ele2_rescnt,1,0);
	printf("rescnt nsend2:%d\n",nsend2);

	// 发送资源内部id
	int nsend3 = send(fd_cli,(const char*)arg2_array,n,0);
	printf("__resid nsend3:%d\n",nsend3);


	/*
	//成功案例
	struct A {
		uint8_t a;
		uint8_t b;
	}v1;

	const char* sender = msg->GetSender();
	const MsgArg* arg1 = msg->GetArg(0);
	const MsgArg* arg2 = msg->GetArg(1);

	// 第三个参数，即结构体数组的解析
	MsgArg* entries;
	const MsgArg* arg3 = msg->GetArg(2);
	arg3->Get("a(qq)",&n,&entries);
	for (int i=0; i<n; i++) {
		entries[i].Get("(qq)",&v1);
		printf("a:%d, b:%d\n",v1.a,v1.b);
	}
	
	// 当签名为(sq(sqq))时，下面的写法是没问题的
	//int num = arg1->v_struct.numMembers;//arg1，结构体参数的元素个数
	//printf("num:%d\n",num);

	const char* ele1_macstr = arg1->v_struct.members->v_string.str;
	uint8_t ele2_rescnt = (arg1->v_struct.members + 1)->v_byte;

	MsgArg* tmp = (arg1->v_struct.members + 2)->v_struct.members;
	//printf("tmp arg num:%d\n",(arg1->v_struct.members + 2)->v_struct.numMembers);

	const char* ele3_1_unit = tmp->v_string.str;
	uint8_t ele3_2_resid = (tmp + 1)->v_byte;
	uint8_t ele3_3_type = (tmp + 2)->v_byte;//这个返回有问题！！！

	ele3_3_type = arg2->v_byte;//于是乎将"资源类型域"以第二参数返回

	printf("\n[%s msg]\n\tdevice name:%s\n\trescnt:%d\n\tres unit:%s\n\t__resid:%d\n\tres type:%d\n",
		sender,ele1_macstr,ele2_rescnt,ele3_1_unit,ele3_2_resid,ele3_3_type);

	char devinfo[32] = {'\0'};
	char* p;

	//char* a = "abc";
	int nsend = send(fd_cli,ele1_macstr,13,0); //不用write，方便移植
	printf("nsend:%d\n",nsend);
	
}
*/

void MyBusObject::devPropHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg)
{
	// send device property

	const MsgArg* arg1 = msg->GetArg(0);
	const char* dev_prop  = arg1->v_string.str;
	printf("\ndev_prop:%s from %s\n\n",dev_prop,msg->GetSender());
	
	DevProp = dev_prop;

#ifdef _WIN32
	int size = send(fd_dev_prop,dev_prop,strlen(dev_prop),0);
#else
	//if MSG_NOSIGNAL is not given on Linux,
	//send call will throw a signal to OS and stop program
	int size = send(fd_dev_prop,dev_prop,strlen(dev_prop),MSG_NOSIGNAL);
#endif

	//printf("sending %d char,strlen(dev_prop):%d\n",size,strlen(dev_prop));

}

/* 
//用结构体实现解析
void MyBusObject::resDataHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg)
{
	struct data_info v;

	//int n_resdata;
	MsgArg* entries;

	// 这种方式出了问题
	const MsgArg* arg1 = msg->GetArg(0);
	arg1->Get("a(sqd)",&n_resdata,&entries);
	printf("n_resdata:%d\n",n_resdata);

	for (int i=0; i<n_resdata; i++) {
		entries[i].Get("(sqd)",&v);
		printf("mac: %s, __resid: %d, val: %.1f\n",v.mac,v.__resid,v.val);//这里解析有问题！！！
	}


	/*
	如果参数签名是(sqd)而不是a(sqd)，则这样解析
	
	const MsgArg* arg1 = msg->GetArg(0);//获取第一个参数

	//int num = arg1->v_struct.numMembers;//arg1，结构体参数的元素个数
	//printf("resdata num:%d\n",num);

	MsgArg* tmp = arg1->v_struct.members;

	const char* macstr = tmp->v_string.str;
	uint8_t __resid = (tmp + 1)->v_byte;
	double val = (tmp + 2)->v_double;

	printf("macstr: %s, __resid: %d, val: %.1f\n",macstr,__resid,val);
	
}*/

void MyBusObject::sensorDataHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg)
{
	const MsgArg* arg1 = msg->GetArg(0);
	const char* res_data  = arg1->v_string.str;
#ifdef _WIN32
	int size = send(fd_resdata,res_data,strlen(res_data),0);
#else
	//if MSG_NOSIGNAL is not given on Linux,
	//send call will throw a signal to OS and stop program
	int size = send(fd_resdata,res_data,strlen(res_data),MSG_NOSIGNAL);
#endif
	//printf("sending %d char\n",size);
	printf("res_data:%s from %s\n\n",res_data,msg->GetSender());

	//printf("sending %d char,strlen(dev_prop):%d\n",size,strlen(res_data));
}

void MyBusObject::cameraDataHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg)
{
	uint8_t* data;
	size_t size;
	static unsigned int offset = 0;

	msg->GetArg(1)->Get("ay", &size, &data);
	
	if (size != 0) {
		char* filePathAndName;

		msg->GetArg(0)->Get("s", &filePathAndName);

		if (NULL == outputStream) {
			char* fileName = filePathAndName;
			char* fileNameBack = strrchr(filePathAndName, '\\');
			char* fileNameForward = strrchr(filePathAndName, '/');

			if (fileNameBack && fileNameForward) {
				fileName = (fileNameBack > fileNameForward ? fileNameBack : fileNameForward) + 1;
			} else {
				if (fileNameForward) {
					fileName = fileNameForward + 1;
				}
				if (fileNameBack) {
					fileName = fileNameBack + 1;
				}
			}

			printf("Opening the output stream to write file '%s'.\n", fileName);
			outputStream = new ofstream(fileName, ios::out | ios::binary);
		}

		if (this->outputStream->is_open()) {
			this->outputStream->write((char*)data, size);
		}
		memcpy(pData+offset,data,size);
		offset += size;

	} else {
		if (this->outputStream->is_open()) {
			printf("The file was received sucessfully.\n");
			outputStream->close();
			
			//发送给python服务端
#ifdef _WIN32
			int sendsize = send(fd_picdata,pData,offset,0);
#else
			//if MSG_NOSIGNAL is not given on Linux,
			//send call will throw a signal to OS and stop program
			int sendsize = send(fd_picdata,pData,offset,0,MSG_NOSIGNAL);
#endif
			printf("pic size:%d\n",offset);
			offset = 0;
		}

		delete outputStream;
		outputStream = NULL;
	}
}

void MyBusObject::dev_prop_handler(const InterfaceDescription::Member* member,ajn::Message& msg)
{
	devPropHandler(member,"",msg);
}

void MyBusObject::res_data_handler(const InterfaceDescription::Member* member,ajn::Message& msg)
{
	sensorDataHandler(member,"",msg);
}
