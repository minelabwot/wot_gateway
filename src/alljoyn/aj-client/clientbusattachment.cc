#include "clientbusattachment.h"
#include <iostream>
#include <signal.h>
#include "mybusobject.h"

using namespace std;

extern ClientBusAttachment* clientbus;
extern const char* SERVICE_NAME;
extern volatile sig_atomic_t g_joinComplete;


ClientBusAttachment::ClientBusAttachment(const char* appname,bool f):BusAttachment(appname,f)
{

}

/** Create the interface, report the result to stdout, and return the result status. */
void ClientBusAttachment::createInterface(const char* intfName)
{
    InterfaceDescription* intf = NULL;
    QStatus status = CreateInterface(intfName, intf);

    if (ER_OK == status) {
        //intf->AddSignal("sig_test", "(sq(sqq))qa(qq)",  "arg1,arg2,arg3", 0);//这是成功的范例，保留参考
		//intf->AddSignal("sig_test","(sqaq)aq","arg1,arg2",0);//设备属性信号
		intf->AddSignal("sig_test","s","arg1",0);

		//intf->AddSignal("sig_resdata","a(sqd)","arg1",0);
		intf->AddSignal("sig_resdata","s","arg1",0);

        intf->Activate();
    } else {
        cout << "Failed to create interface\n";
    }
}

void ClientBusAttachment::startMessageBus(void)
{
    QStatus status = Start();

    if (ER_OK != status) {
        cout << "Start of BusAttachment failed\n";
    }
}

void ClientBusAttachment::registerBusObject(MyBusObject& obj)
{
    QStatus status = RegisterBusObject(obj);

    if (ER_OK != status) {
        cout << "RegisterBusObject failed\n";
    }
}

void ClientBusAttachment::connectBusAttachment(void)
{
    QStatus status = Connect();

    if (ER_OK == status) {
        printf("Connect to '%s' succeeded.\n", this->GetConnectSpec().c_str());
	} else {
		printf("Failed to connect to '%s' (%s).\n", this->GetConnectSpec().c_str(), QCC_StatusText(status));
	}
}

/** Begin discovery on the well-known name of the service to be called, report the result to
   stdout, and return the result status. */
void ClientBusAttachment::findAdvertisedName(void)
{
    /* Begin discovery on the well-known name of the service to be called */
    QStatus status = FindAdvertisedName(SERVICE_NAME);

    if (status != ER_OK) {
       printf("FindAdvertisedName ('%s') failed (%s).\n", SERVICE_NAME, QCC_StatusText(status));
    } 
}

/** Wait for join session to complete, report the event to stdout, and return the result status. */
QStatus ClientBusAttachment::waitForJoinSessionCompletion(void)
{
    unsigned int count = 0;

    while (!g_joinComplete) {
        if (0 == (count++ % 100)) {
            cout << "Waited " << count/100 << " seconds for JoinSession completion.\n";
        }

#ifdef _WIN32
        Sleep(10);
#else
        usleep(10 * 1000);
#endif
    }

    return g_joinComplete ? ER_OK : ER_ALLJOYN_JOINSESSION_REPLY_CONNECT_FAILED;
}
