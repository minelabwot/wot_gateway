#ifdef _WIN32
#include <WinSock2.h>
#include <IPHlpApi.h>
#endif
#include <cstdio>

#ifdef _WIN32
void print_mac_info(PIP_ADAPTER_INFO adapterInfo)
{
	while (adapterInfo) {
		printf("adapter name: %s\n",adapterInfo->AdapterName);
		printf("adapter description: %s\n",adapterInfo->Description);
		printf("mac addr:");
		
		for (int i=0; i<adapterInfo->AddressLength; i++) {
			printf("%2x",adapterInfo->Address[i]);
		}
		printf("\n");

		//可能网卡有多IP,因此通过循环去判断
		IP_ADDR_STRING *pIpAddrString =&(adapterInfo->IpAddressList);
		do 
		{
			printf("%s\n",pIpAddrString->IpAddress.String);
			pIpAddrString=pIpAddrString->Next;
		} while (pIpAddrString);
		adapterInfo = adapterInfo->Next;
	}
}

void get_mac(char macstr[])
{
	PIP_ADAPTER_INFO adapterInfo = new IP_ADAPTER_INFO();
	unsigned long size = sizeof(IP_ADAPTER_INFO);

	int ret = GetAdaptersInfo(adapterInfo,&size);
	if (ERROR_BUFFER_OVERFLOW == ret) {
		// 参数传递的内存空间不够，同时传出size大小，表示其需要的空间
		// 需要重新申请
		delete adapterInfo;

		adapterInfo = (PIP_ADAPTER_INFO)new BYTE(size);
		ret = GetAdaptersInfo(adapterInfo,&size);
	}
	if (ERROR_SUCCESS == ret) {
		//print_mac_info(adapterInfo);
	}

	for (int i=0; i<adapterInfo->AddressLength; i++) {
		sprintf(macstr,"%2x",adapterInfo->Address[i]);
		macstr += 2;
	}
}
#endif
