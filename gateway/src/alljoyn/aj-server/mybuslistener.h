#ifndef MYBUSLISTENER_H_
#define MYBUSLISTENER_H_

#include <alljoyn/BusAttachment.h>

using namespace ajn;

class MyBusListener : public BusListener, public SessionPortListener, public SessionListener
{
public:

	virtual bool AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts);
	virtual void SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner);
	virtual void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner);

	virtual void SessionMemberAdded(SessionId sessionId,const char* uniqueName);
	virtual void SessionMemberRemoved(SessionId sessionId,const char* uniqueName);
};

#endif // !MYBUSLISTENER_H_
