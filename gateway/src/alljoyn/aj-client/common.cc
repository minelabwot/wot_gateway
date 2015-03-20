#ifdef _WIN32
#include <WinSock2.h>
#include <IPHlpApi.h>
#endif

#include <cstdio>

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
#ifdef _WIN32
	/*
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
	*/

	ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
	PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	// Make an initial call to GetAdaptersAddresses to get the necessary size into the ulOutBufLen variable
	if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	}

	if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == NO_ERROR)
	{
		// If successful, output some information from the data we received
		for(PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
		{
			// 确保MAC地址的长度为 00-00-00-00-00-00
			if(pCurrAddresses->PhysicalAddressLength != 6)
				continue;
			sprintf(macstr, "%02X%02X%02X%02X%02X%02X",
				int (pCurrAddresses->PhysicalAddress[0]),
				int (pCurrAddresses->PhysicalAddress[1]),
				int (pCurrAddresses->PhysicalAddress[2]),
				int (pCurrAddresses->PhysicalAddress[3]),
				int (pCurrAddresses->PhysicalAddress[4]),
				int (pCurrAddresses->PhysicalAddress[5]));
			break;
		}
	}
	free(pAddresses);
	//printf("mac:%s\n",macstr);
	//strcpy(macstr,"DEFAULT");
#else
	char mac_addr[13];
	int sockfd;
	struct ifreq tmp;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&tmp,0,sizeof(struct ifreq));
	strncpy(tmp.ifr_name,"eth0",sizeof(tmp.ifr_name) - 1);

	ioctl(sockfd,SIOCGIFHWADDR,&tmp);

	sprintf(mac_addr,"%02x%02x%02x%02x%02x%02x",
		(unsigned char)tmp.ifr_hwaddr.sa_data[0],
		(unsigned char)tmp.ifr_hwaddr.sa_data[1],
		(unsigned char)tmp.ifr_hwaddr.sa_data[2],
		(unsigned char)tmp.ifr_hwaddr.sa_data[3],
		(unsigned char)tmp.ifr_hwaddr.sa_data[4],
		(unsigned char)tmp.ifr_hwaddr.sa_data[5]
	);
	printf("mac:%s\n",mac_addr);

#endif
}