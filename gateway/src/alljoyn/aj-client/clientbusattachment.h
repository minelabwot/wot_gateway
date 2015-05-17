#ifndef _CLIENTBUSATTACHMENT_H_
#define _CLIENTBUSATTACHMENT_H_

#include <alljoyn/BusAttachment.h>

class MyBusObject;

class ClientBusAttachment:public ajn::BusAttachment
{
public:
	ClientBusAttachment(const char* appname,bool f);

	void createInterface(const char* intfName);
	void startMessageBus(void);
	void registerBusObject(MyBusObject& obj);
	void connectBusAttachment(void);
	void findAdvertisedName(void);
	QStatus waitForJoinSessionCompletion(void);

};

#endif // !_CLIENTBUSATTACHMENT_H_