#ifndef MYBUSLISTENER_H_
#define MYBUSLISTENER_H_

#include <alljoyn/BusAttachment.h>

using namespace ajn;

//假若服务端的listner继承了SessionListner，那在主程序调用RegisterListener就会出现下面的异常，而应该
//选择在SessionJoined中调用SetSessionListner
/*
Assertion failed:m_constructed && "IpNameService::" "\RegisterListener\"" "():
Singleton not constructed", file alljoyn_core\router\ns\IpNameService.cc,line 531
*/

//如果再次出现上述错误，可以尝试添加继承,public SessionListener
class MyBusListener : public BusListener, public SessionPortListener, public SessionListener
{
public:
	
	//以下两个是继承自SessionPortListener
	virtual bool AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts);
	virtual void SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner);

	//继承自BusListener
	virtual void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner);


	//SessionListener，加了这两个似乎就不会调用NameOwnerChanged
	virtual void SessionMemberAdded(SessionId sessionId, const char* uniqueName);
	virtual void SessionMemberRemoved(SessionId sessionId, const char* uniqueName);

};

#endif // !MYBUSLISTENER_H_