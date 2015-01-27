#include "mybuslistener.h"
#include "servicebusattachment.h"
#include <iostream>
#include <qcc/Debug.h>
#include "common.h"

using namespace std;

extern ServiceBusAttachment* servicebus;
SessionId g_sessionId;

const char* g_joiner;

// 以下三函数在linux下并未真正调用 ？？？

void MyBusListener::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
{
	// 总是被自动连接调用好几次，为什么 ？

	cout << "NameOwnerChanged: name=" << busName << ", oldOwner=" << (previousOwner ? previousOwner : "<none>") <<
		", newOwner=" << (newOwner ? newOwner : "<none>") << endl;

	if (previousOwner && !newOwner) {
		cout << previousOwner << " exited" << endl;
	}
	/*
	cout << g_joiner << "," << previousOwner;
	if (g_joiner) {
		cout << g_joiner << previousOwner;
		if (strcmp(previousOwner,g_joiner) == 0) {
				cout << "client exited" << endl;
			}
	}
	*/
}

bool MyBusListener::AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts)
{
	// 服务端调用
	if (sessionPort != MyNameSpace::PORT) {
		cout << "Rejecting join attempt on wrong session port" << endl;
		return false;
	}

	g_joiner = joiner;

	cout << "\nAccepting join session request from " << joiner << endl;
	return true;
}

void MyBusListener::SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner)
{
	g_sessionId = id;
	cout << "Session Joined, sessionId:" << g_sessionId << endl;

	servicebus->EnableConcurrentCallbacks();
	uint32_t timeout = 20;
	QStatus status = servicebus->SetLinkTimeout(g_sessionId, timeout);
	if (ER_OK != status) {
		QCC_SyncPrintf("Set link timeout failed\n");
	} 
}

void MyBusListener::SessionMemberAdded(SessionId sessionId,const char* uniqueName)
{
	printf("%s added\n",uniqueName);
}

void MyBusListener::SessionMemberRemoved(SessionId sessionId,const char* uniqueName)
{
	printf("%s left\n",uniqueName);
}
