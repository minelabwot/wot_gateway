#ifndef SERVICEBUSATTACHMENT_H_
#define SERVICEBUSATTACHMENT_H_

#include <alljoyn/BusAttachment.h>
#include "mybuslistener.h"

using namespace ajn;

class ServiceBusAttachment:public BusAttachment
{
	
public:
	ServiceBusAttachment(const char* appname,bool f);

	void advertiseService(const char* serviceName,const TransportMask mask,MyBusListener& busListener);
	
	void createInterface(const char* intfName);
	
private:
	
	QStatus requestName(const char* serviceName,uint32_t flag);
	QStatus advertiseName(const char* serviceName,TransportMask mask);
	QStatus createSession(const TransportMask mask,MyBusListener& busListener);

};

#endif
