#ifdef _WIN32
#include <WinSock2.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#endif

#include "mybuslistener.h"
#include "servicebusattachment.h"
#include <iostream>
#include <qcc/Debug.h>
#include "common.h"

using namespace std;

extern ServiceBusAttachment* servicebus;
extern string DevProp;
extern int fd_dev_prop;

SessionId g_sessionId;

void MyBusListener::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
{
	// 总是被自动连接调用好几次，为什么 ？

	cout << "NameOwnerChanged: name=" << busName << ", oldOwner=" << (previousOwner ? previousOwner : "<none>") <<
		", newOwner=" << (newOwner ? newOwner : "<none>") << endl;

}

bool MyBusListener::AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts)
{
	// 服务端调用
	if (sessionPort != MyNameSpace::PORT) {
		cout << "Rejecting join attempt on wrong session port" << endl;
		return false;
	}

	cout << "[AcceptSessionJoiner] Accepting join session request from " << joiner << endl;
	return true;
}

void MyBusListener::SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner)
{
	g_sessionId = id;
	cout << "[SessionJoined] Session Joined, sessionId:" << g_sessionId << endl;

	servicebus->EnableConcurrentCallbacks();

	QStatus status = servicebus->SetSessionListener(id, this);

	uint32_t timeout = 40;
	status = servicebus->SetLinkTimeout(g_sessionId, timeout);
	if (ER_OK != status) {
		QCC_SyncPrintf("Set link timeout failed\n");
	} 
}


void MyBusListener::SessionMemberAdded(SessionId sessionId, const char* uniqueName)
{
	printf("[SessionMemberAdded] %s joined session %u\n",uniqueName,sessionId);
}


void MyBusListener::SessionMemberRemoved(SessionId sessionId, const char* uniqueName)
{
	printf("[SessionMemberRemoved] %s left session %u\n",uniqueName,sessionId);
	char dev_left_info[64] = "\0";
	strncpy(dev_left_info,DevProp.c_str(),(DevProp.find('"',16)+1));//检测第4个"的位置
	//printf("dev_left_info:%s\n",dev_left_info);
	strcat(dev_left_info,",\"flags\":1}");

#ifdef _WIN32
	int size = send(fd_dev_prop,dev_left_info,strlen(dev_left_info),0);
#else
	//if MSG_NOSIGNAL is not given on Linux,
	//send call will throw a signal to OS and stop program
	int size = send(fd_dev_prop,dev_prop,strlen(dev_prop),MSG_NOSIGNAL);
#endif
}


