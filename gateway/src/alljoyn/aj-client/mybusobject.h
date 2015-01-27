#ifndef MYBUSOBJECT_H_
#define MYBUSOBJECT_H_

#include <alljoyn/BusAttachment.h>

using namespace ajn;

class MyBusObject : public BusObject
{
public:

	MyBusObject(BusAttachment& bus, const char* path);
	
	QStatus sendTestSignal(const char* msg);
	QStatus sendResdataSignal(uint8_t index);

	QStatus sendTestSignal_2(const char* msg);
	QStatus sendResdataSignal_2(uint8_t __resid,double val);

	//void testSigHandler(const InterfaceDescription::Member* member, const char* srcPath, Message& msg);

private:
	const InterfaceDescription::Member* testSigMember;
	const InterfaceDescription::Member* resDataSigMember;
};

#endif // !MYBUSOBJECT_H_