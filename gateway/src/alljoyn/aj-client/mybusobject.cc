#include "mybusobject.h"
#include "common.h"
#include <iostream>
#include <fstream>

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
	const InterfaceDescription* intf = bus.GetInterface(INTERFACE_NAME);
	AddInterface(*intf);

	testSigMember = intf->GetMember("sig_dev_prop");
	resDataSigMember = intf->GetMember("sig_resdata");
	picDataSigMember = intf->GetMember("sig_pic");
}

QStatus MyBusObject::sendDevPropSignal(const char* msg) 
{
	MsgArg arg;

	arg.Set("s",msg);
	printf("msg:%s\n",msg);

	if (0 == g_sessionId) {
		cout << "Sending Chat signal without a session id" << endl;
	}
	return Signal(NULL, g_sessionId, *testSigMember, &arg,1, 0, 0);
}

QStatus MyBusObject::sendResdataSignal(uint8_t __resid,double val)
{
	MsgArg arg;
	char tmp[32];

	// 传感器数据信息封装成json格式字符串

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

QStatus MyBusObject::sendPicSignal(const char* filename)
{
	char* buf = new char[ALLJOYN_MAX_ARRAY_LEN];
	memset(buf,0,ALLJOYN_MAX_ARRAY_LEN);

	long l,length,len;
	ifstream inputStream(filename, ios::in | ios::binary);

	if (inputStream.is_open()) {
		MsgArg args[3];
		QStatus status = ER_OK;
		const uint8_t flags = ALLJOYN_FLAG_GLOBAL_BROADCAST;

		l=inputStream.tellg();
		inputStream.seekg(0,ios::end);
		length=inputStream.tellg();
		len = length;

		inputStream.seekg(0);

		while (len > 0) {
			//数组最大长度：ALLJOYN_MAX_ARRAY_LEN

			long bufferLength = ALLJOYN_MAX_ARRAY_LEN;

			if (len > (filebuf::pos_type)ALLJOYN_MAX_ARRAY_LEN) {
				len -= (filebuf::pos_type)ALLJOYN_MAX_ARRAY_LEN;
			} else {
				bufferLength = len;
				len = 0;
			}

			inputStream.read(buf, bufferLength);

			args[0].Set("s", filename);
			args[1].Set("ay", bufferLength, buf);
			args[2].Set("s",NULL);

			status = Signal(NULL, g_sessionId, *picDataSigMember, args, 3, 0, flags);
		}
		//在最后一次传输时，将文件名和mac传过来
		args[0].Set("s", filename);
		args[1].Set("ay", 0, NULL);//这个是表明传输结束	
		args[2].Set("s",macstr);

		status = Signal(NULL, g_sessionId, *picDataSigMember, args, 3, 0, flags);
		printf("sent %s ok. size:%d\n",filename,length);
	} else {
		printf("The file doesn't exist or the permissions is stopping the app from opening the file.\n");
	}

	delete [] buf;
	return ER_OK;
}


