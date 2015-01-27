#ifdef _WIN32
#include <WinSock2.h>

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#endif

#include "mybusobject.h"
#include "common.h"

#include <cstdio>
#include <string>

using namespace std;

extern const char* INTERFACE_NAME;
extern SessionId g_sessionId;

extern int fd_cli;
extern int fd_resdata;

uint8_t n;//作为第三个消息参数，即结构体数组的元素个数，如果作局部变量，VS会报堆栈错误，所以调整为全局变量
uint8_t n_resdata;

MyBusObject::MyBusObject(BusAttachment& bus, const char* path) : BusObject(path),prop_name("")
{
	QStatus status;

	const InterfaceDescription* chatIntf = bus.GetInterface(INTERFACE_NAME);
	AddInterface(*chatIntf);

	/* Store the Chat signal member away so it can be quickly looked up when signals are sent */
	devPropSigMember = chatIntf->GetMember("sig_test");
	resDataSigMember = chatIntf->GetMember("sig_resdata");

	const MethodEntry methodEntries[] = {
		{ chatIntf->GetMember("dev_prop"),static_cast<MessageReceiver::MethodHandler>(&MyBusObject::dev_prop_handler) },
		{ chatIntf->GetMember("res_data"),static_cast<MessageReceiver::MethodHandler>(&MyBusObject::res_data_handler) }
	};
	AddMethodHandlers(methodEntries,sizeof(methodEntries)/sizeof(methodEntries[0]));

	
	status = bus.RegisterSignalHandler(this,
		static_cast<MessageReceiver::SignalHandler>(&MyBusObject::devPropHandler),
		devPropSigMember,
		NULL);

	status = bus.RegisterSignalHandler(this,
		static_cast<MessageReceiver::SignalHandler>(&MyBusObject::resDataHandler_2),
		resDataSigMember,
		NULL);

	if (ER_OK != status) {
		printf("Failed to register signal handler for ChatObject::Chat (%s)\n", QCC_StatusText(status));
	}
}

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
	*/
}

void MyBusObject::devPropHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg)
{
	// send device property

	const MsgArg* arg1 = msg->GetArg(0);
	const char* dev_prop  = arg1->v_string.str;
	printf("dev_prop:%s\n",dev_prop);
	
	//if MSG_NOSIGNAL is not given, send call will throw a signal to OS and stop program
	int size = send(fd_cli,dev_prop,strlen(dev_prop),MSG_NOSIGNAL);

	printf("sending %d char\n",size);
}

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
	*/
}

void MyBusObject::resDataHandler_2(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg)
{
	const MsgArg* arg1 = msg->GetArg(0);
	const char* res_data  = arg1->v_string.str;
	int size = send(fd_resdata,res_data,strlen(res_data),MSG_NOSIGNAL);
	//printf("sending %d char\n",size);
	printf("res_data:%s from %s\n\n",res_data,msg->GetSender());

}

void MyBusObject::dev_prop_handler(const InterfaceDescription::Member* member,ajn::Message& msg)
{
	// 古宗海瘦客户端的方法处理
	devPropHandler(member,"",msg);
}

void MyBusObject::res_data_handler(const InterfaceDescription::Member* member,ajn::Message& msg)
{
	resDataHandler_2(member,"",msg);
}
