#ifndef MYBUSOBJECT_H_
#define MYBUSOBJECT_H_

#include <alljoyn/BusAttachment.h>

using namespace ajn;

class MyBusObject : public BusObject
{
public:

	MyBusObject(BusAttachment& bus, const char* path);
	
	QStatus sendTestSignal(const char* msg);
	QStatus sendResdataSignal_2(uint8_t index);

	QStatus sendDevPropSignal(const char* msg);
	QStatus sendResdataSignal(uint8_t __resid,double val);
	QStatus sendPicSignal(const char* msg);

private:
	const InterfaceDescription::Member* testSigMember;
	const InterfaceDescription::Member* resDataSigMember;
	const InterfaceDescription::Member* picDataSigMember;
};

#endif // !MYBUSOBJECT_H_