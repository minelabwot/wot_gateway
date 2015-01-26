#include "mybusobject.h"
#include "common.h"
#include <iostream>

using namespace std;

extern const char* INTERFACE_NAME;
extern SessionId g_sessionId;

//extern struct dev_info thisdev;
//extern struct data_info resdata[];
extern int mynum;

extern char macstr[13];

MyBusObject::MyBusObject(BusAttachment& bus, const char* path) : BusObject(path)
{
	QStatus status;

	/* Add the chat interface to this object */
	const InterfaceDescription* intf = bus.GetInterface(INTERFACE_NAME);
	AddInterface(*intf);

	/* Store the Chat signal member away so it can be quickly looked up when signals are sent */
	testSigMember = intf->GetMember("sig_test");
	resDataSigMember = intf->GetMember("sig_resdata");
	
	/*
	status =  bus.RegisterSignalHandler(this,
		static_cast<MessageReceiver::SignalHandler>(&MyBusObject::testSigHandler),
		testSigMember,
		NULL);*/
}

/*
QStatus MyBusObject::sendTestSignal(const char* msg) 
{
	static int dev_num = 0;

	struct A{
		uint8_t a;
		uint8_t b;
	}v1;
	struct A test[3] = {{1,2},{3,4},{10,6}};

	MsgArg arg[2];

	arg[0].Set("(sqaq)",thisdev);//如果写上后面的aq，则传过去的struct的元素数目就是3，也可以没有aq，则为2

	int n = CUR_RES_NUM;//当前的资源数目

	arg[1].Set("aq",n,thisdev.res);

	
	//成功范例
	MsgArg arg[3];
	arg[0].Set("(sq(sqq))",thisdev);
	arg[1].Set("q",thisdev.res[0].type);

	MsgArg tmp[3];
	tmp[0].Set("(qq)",test[0]);
	tmp[1].Set("(qq)",test[1]);
	tmp[2].Set("(qq)",test[2]);
	arg[2].Set("a(qq)",sizeof(test)/sizeof(struct A),tmp);//第二个参数必须给
	

	if (0 == g_sessionId) {
		cout << "Sending Chat signal without a session id" << endl;
	}
	printf("sending dev_info... %d devices in all\n",++dev_num);
	return Signal(NULL, g_sessionId, *testSigMember, arg,2, 0, 0);
}

QStatus MyBusObject::sendResdataSignal(uint8_t index)
{
	MsgArg arg;

	MsgArg tmp[CUR_RES_NUM];
	for (int i=0; i<CUR_RES_NUM; i++) {
		tmp[i].Set("(sqd)",resdata[i]);
	}
	arg.Set("a(sqd)",mynum,tmp);
	
	if (0 == g_sessionId) {
		cout << "Sending Chat signal without a session id" << endl;
	}
	//printf("sending res %d data_info... val: %.1f\n",index+1,resdata[index].val);
	return Signal(NULL, g_sessionId, *resDataSigMember, &arg,1, 0, 0);
}

*/

QStatus MyBusObject::sendTestSignal_2(const char* msg) 
{
	MsgArg arg;

	// 将设备属性封装成json格式字符串
	/*
	char tmp[1024] = "{\"Mac_address\":\"";
	strcat(tmp,macstr);
	//char* p = "','Res_num':2,'flags':1,'Res':[{'Res_name':'','Res_type':'','Res_unit':'','Res_port':1},{'Res_name':'','Res_type':'','Res_unit':'','Res_port':2}]}";
	char* p = "\",\"Res_num\":2,\"flags\":0,\"Res\":[{\"Res_name\":\"\",\"Res_type\":\"\",\"Res_unit\":\"\",\"Res_port\":3},\
{\"Res_name\":\"\",\"Res_type\":\"\",\"Res_unit\":\"\",\"Res_port\":4}]}";

	strcat(tmp,p);
	printf("dev_prop string :%s\n",tmp);
	*/

	arg.Set("s",msg);
	printf("msg:%s\n",msg);

	if (0 == g_sessionId) {
		cout << "Sending Chat signal without a session id" << endl;
	}
	return Signal(NULL, g_sessionId, *testSigMember, &arg,1, 0, 0);
}

QStatus MyBusObject::sendResdataSignal_2(uint8_t __resid,double val)
{
	MsgArg arg;
	char tmp[32];

	// 数据信息封装成json格式字符串

	char resdata[1024] = "{\"Mac_addr\":\"";
	strcat(resdata,macstr);
	
	strcat(resdata,"\",\"Res_port\":");

	sprintf(tmp,"%d",__resid);	
	strcat(resdata,tmp);
	strcat(resdata,",\"Res_val\":");
	sprintf(tmp,"%.2f",val);
	strcat(resdata,tmp);
	strcat(resdata,"}");
	printf("resdata string :%s\n",resdata);
	
	arg.Set("s",resdata);

	if (0 == g_sessionId) {
		cout << "Sending Chat signal without a session id" << endl;
	}

	return Signal(NULL, g_sessionId, *resDataSigMember, &arg,1, 0, 0);
}

