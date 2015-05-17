#include "servicebusattachment.h"
#include "mybuslistener.h"
#include <cstdio>
#include "common.h"

extern const char* SERVICE_NAME;
extern const char* INTERFACE_NAME;

extern ServiceBusAttachment* servicebus;

ServiceBusAttachment::ServiceBusAttachment(const char* appname,bool f):BusAttachment(appname,f)
{
		
}

void ServiceBusAttachment::advertiseService(const char* serviceName,const TransportMask mask,MyBusListener& busListener)
{
	// 发布服务三步骤
	uint32_t f = DBUS_NAME_FLAG_DO_NOT_QUEUE;
	requestName(serviceName,f);
	createSession(mask,busListener);
	advertiseName(serviceName,mask);
}

void ServiceBusAttachment::createInterface(const char* intfName)
{
	InterfaceDescription* intf = NULL;
	QStatus status = CreateInterface(intfName, intf);

	if (ER_OK == status) {
		//intf->AddSignal("sig_test", "(sq(sqq))qa(qq)",  "arg1,arg2,arg3", 0);
		//intf->AddSignal("sig_test","(sqaq)aq","arg1,arg2",0);
		intf->AddSignal("sig_dev_prop","s","arg1",0);

		//intf->AddSignal("sig_resdata","a(sqd)","arg1",0);
		intf->AddSignal("sig_resdata","s","arg1",0);
		intf->AddSignal("sig_pic","says","name,data,macstr",0);


		intf->AddMethod("dev_prop","s",NULL,"inStr",0);//古宗海添加方法
		intf->AddMethod("res_data","s",NULL,"inStr",0);

		intf->Activate();
	} else {
		printf("Failed to create interface \"%s\" (%s)\n", INTERFACE_NAME, QCC_StatusText(status));
	}
}

QStatus ServiceBusAttachment::requestName(const char* serviceName,uint32_t flag)
{
	QStatus status = servicebus->RequestName(serviceName, flag);

	if (ER_OK == status) {
		//printf("\nservice '%s' request ok\n",serviceName);
	}
	else {
		printf("RequestName('%s') failed (status=%s).\n",serviceName, QCC_StatusText(status));
	} 
	return status;
}

QStatus ServiceBusAttachment::createSession(const TransportMask mask,MyBusListener& busListener)
{
	SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, mask);
	SessionPort sp = MyNameSpace::PORT;
	QStatus status = servicebus->BindSessionPort(sp, opts, busListener);

	if (ER_OK == status) {
		//printf("bind session port %u ok\n",MyNameSpace::PORT);
	}
	else {
		printf("BindSessionPort failed (%s).\n", QCC_StatusText(status));
	}
	return status;
}

/** Advertise the service name, report the result to stdout, and return the status code. */
QStatus ServiceBusAttachment::advertiseName(const char* serviceName,TransportMask mask)
{
	QStatus status = servicebus->AdvertiseName(serviceName, mask);

	if (ER_OK == status) {
		//printf("advertise service '%s' ok\n",serviceName);
	}
	else {
		printf("Failed to advertise name '%s' (%s).\n",serviceName, QCC_StatusText(status));
	}
	return status;
}

