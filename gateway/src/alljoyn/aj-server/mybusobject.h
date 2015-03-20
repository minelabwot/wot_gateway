#ifndef MYBUSOBJECT_H_
#define MYBUSOBJECT_H_

#include <alljoyn/BusAttachment.h>
#include <fstream>

using namespace ajn;

class MyBusObject : public BusObject
{
public:
	MyBusObject(BusAttachment& bus, const char* path);
	
	//对暂时不使用的特性就不使用
	//void ObjectRegistered();
	//QStatus Get(const char* ifcName, const char* propName, MsgArg& val);
	//QStatus Set(const char* ifcName, const char* propName, MsgArg& val);

	//QStatus sendCmdSignal(const char* cmd);
	//void testSigHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg);
	void devPropHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg);
	
	//void resDataHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg);
	void sensorDataHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg);
	void cameraDataHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg);

	void dev_prop_handler(const InterfaceDescription::Member* member,ajn::Message& msg);
	void res_data_handler(const InterfaceDescription::Member* member,ajn::Message& msg);

private:
	const InterfaceDescription::Member* devPropSigMember;
	const InterfaceDescription::Member* resDataSigMember;
	const InterfaceDescription::Member* picDataSigMember;

	//const InterfaceDescription::Member* commandSendedSignalMember;

	qcc::String prop_name;

	std::ofstream* outputStream;

};

#endif // !MYBUSOBJECT_H_