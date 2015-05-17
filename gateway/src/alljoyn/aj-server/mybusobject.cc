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

string part_string;
string DevProp;

char *pData;

MyBusObject::MyBusObject(BusAttachment& bus, const char* path) : BusObject(path),prop_name("")
{
	QStatus status;
	outputStream = NULL;
	const InterfaceDescription* intfDesc = bus.GetInterface(INTERFACE_NAME);
	AddInterface(*intfDesc);

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

//Get,Set,ObjectRegistered Method
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


void MyBusObject::devPropHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg)
{
	// send device property

	const MsgArg* arg1 = msg->GetArg(0);
	const char* dev_prop  = arg1->v_string.str;
	printf("\ndev_prop:%s from %s\n\n",dev_prop,msg->GetSender());
	
	DevProp = dev_prop;
	part_string = DevProp.substr(0,40);//后面可再接一个资源编号
	
	if (DevProp.find("monitor") != string::npos) {
		printf("has moniter\n");
		//至少有一个摄像头节点,目前还没考虑一个设备有多个摄像头
		pData = (char*)malloc(MAX_IMAGE_SIZE);
		if (pData == NULL) {
			printf("malloc pData failed\n");
		}
		memset(pData,0,MAX_IMAGE_SIZE);
	}

#ifdef _WIN32
	int size = send(fd_dev_prop,dev_prop,strlen(dev_prop),0);
#else
	//if MSG_NOSIGNAL is not given on Linux,
	//send call will throw a signal to OS and stop program
	int size = send(fd_dev_prop,dev_prop,strlen(dev_prop),MSG_NOSIGNAL);
#endif
}


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
}


/*
//这个是一个完好的版本，如果另一个不行，采取它
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
			char *fileName = filePathAndName;
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
			
			char* filePathAndName;
			char *macstr;

			msg->GetArg(0)->Get("s", &filePathAndName);
			msg->GetArg(2)->Get("s", &macstr);

			char *fileName = filePathAndName;
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

			string resid_str = fileName;

			string image_header = "{\"Mac_addr\":\"";
			image_header.append(macstr);
			image_header.append("\",\"Res_port\":");
			image_header.append(resid_str.substr(0,resid_str.find('.')));
			image_header.append("}");
			int len = image_header.size();

			printf("debug:%s\n",image_header.c_str());
		
			memcpy(pData+offset,image_header.c_str(),len);
			printf("image_header size:%d,length:%d\n",len,image_header.length());
			offset = offset + len;		

			//发送给python服务端
#ifdef _WIN32
			int sendsize = send(fd_picdata,pData,offset,0);
			
			//为了再发mac和内部资源号
			
			//printf("second send:%d bytes\n",send(fd_picdata,tmp,strlen(tmp),0));
#else
			//if MSG_NOSIGNAL is not given on Linux,
			//send call will throw a signal to OS and stop program
			int sendsize = send(fd_picdata,pData,offset,MSG_NOSIGNAL);
#endif
			printf("pic size:%d\n",offset);
			offset = 0;
		}

		delete outputStream;
		outputStream = NULL;
	}
}
*/

void MyBusObject::dev_prop_handler(const InterfaceDescription::Member* member,ajn::Message& msg)
{
	devPropHandler(member,"",msg);
}

void MyBusObject::res_data_handler(const InterfaceDescription::Member* member,ajn::Message& msg)
{
	sensorDataHandler(member,"",msg);
}


void MyBusObject::cameraDataHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg)
{
	uint8_t* data;
	size_t size;
	static unsigned int offset = 0;

	msg->GetArg(1)->Get("ay", &size, &data);

	if (size != 0) {
		memcpy(pData+offset,data,size);
		offset += size;

	} else {
		if (offset > MAX_IMAGE_SIZE) {
			//图片大小过大
			printf("image oversize:larger than 700K\n");
			return;
		}

		//图片流发完了
		if (outputStream == NULL) {
			printf("The file received ok\n");

			char* filePathAndName;
			char *macstr;

			msg->GetArg(0)->Get("s", &filePathAndName);
			msg->GetArg(2)->Get("s", &macstr);

			char *fileName = filePathAndName;
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
			printf("opening the output stream to write file '%s'.\n", fileName);
			outputStream = new ofstream(fileName, ios::out | ios::binary);

			outputStream->write(pData,offset);
			
			string resid_str(fileName);
			string image_header("{\"Mac_addr\":\"");
			image_header.append(macstr);
			image_header.append("\",\"Res_port\":");
			image_header.append(resid_str.substr(0,resid_str.find('.')));
			image_header.append("}");
			int len = image_header.size();

			//在图片数据基础上加上包含Macaddr和资源ID的头部
			memcpy(pData+offset,image_header.c_str(),len);		

			//发送给python服务端
#ifdef _WIN32
			int sendsize = send(fd_picdata,pData,offset+len,0);
#else
			//if MSG_NOSIGNAL is not given on Linux,
			//send call will throw a signal to OS and stop program
			int sendsize = send(fd_picdata,pData,offset+len,MSG_NOSIGNAL);
#endif
			if(sendsize == -1) {
				printf("image socket send bytes failed\n");
			}
			else {
				if (offset < 1024 *10)
					printf("image socket send total bytes size:(%d B + %d B)=%d\n\n",offset,len,offset+len);
				else
					printf("image socket send total bytes size:(%.2f KB + %d B)=%d\n\n",offset/1024.0,len,offset+len);
			}		
			offset = 0;
		}

		delete outputStream;
		outputStream = NULL;
	}
}