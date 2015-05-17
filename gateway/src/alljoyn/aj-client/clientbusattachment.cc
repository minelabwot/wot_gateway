#include "clientbusattachment.h"
#include "mybusobject.h"

#include <iostream>
#include <signal.h>

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
		intf->AddSignal("sig_dev_prop","s","arg1",0);

		//intf->AddSignal("sig_resdata","a(sqd)","arg1",0);
		intf->AddSignal("sig_resdata","s","arg1",0);
		//intf->AddSignal("sig_pic","say","name,data",0);//传图片信号
		intf->AddSignal("sig_pic","says","name,data,macstr",0);//传图片信号

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

    if (ER_OK != status) {
        cout << "Failed to connect to " << clientbus->GetConnectSpec().c_str() << endl;
    }
}

void ClientBusAttachment::findAdvertisedName(void)
{
    QStatus status = FindAdvertisedName(SERVICE_NAME);

    if (status != ER_OK) {
       cout << "FindAdvertisedName failed\n";
    } 
}

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