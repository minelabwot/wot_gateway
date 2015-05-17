#ifdef _WIN32
#include <WinSock2.h>
#include <IPHlpApi.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include <cstring>
#include <cstdio>  
#include <cstdlib> 

#include <unistd.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>   
#include <termios.h>   
#include <errno.h>    
 
#define serial_device "/dev/ttyS1"
#endif

#include <cstdio>

/*
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
*/

void get_mac(char macstr[])
{
#ifdef _WIN32
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
#else
	int sockfd;
	struct ifreq tmp;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&tmp,0,sizeof(struct ifreq));

	strncpy(tmp.ifr_name,"apcli0",sizeof(tmp.ifr_name) - 1);

	ioctl(sockfd,SIOCGIFHWADDR,&tmp);

	sprintf(macstr,"%02x%02x%02x%02x%02x%02x",
		(unsigned char)tmp.ifr_hwaddr.sa_data[0],
		(unsigned char)tmp.ifr_hwaddr.sa_data[1],
		(unsigned char)tmp.ifr_hwaddr.sa_data[2],
		(unsigned char)tmp.ifr_hwaddr.sa_data[3],
		(unsigned char)tmp.ifr_hwaddr.sa_data[4],
		(unsigned char)tmp.ifr_hwaddr.sa_data[5]
	);

#endif
}

void print_client_help()
{
	printf("--------------------  The Program Flow   -------------------\n\n");
	printf("\t1.Create a client BusAttachment instance\n");
	printf("\t2.Create interfaces for the instance\n");
	printf("\t3.Register BusListener for the instance\n");
	printf("\t4.Start bus instance\n");
	printf("\t5.Create BusObject instance\n");
	printf("\t6.Register BusObject for bus instance\n");
	printf("\t7.Connect to router\n");
	printf("\t8.Start to find service and do work\n\n");
	printf("--------------------   End               -------------------\n\n");
}

#ifndef _WIN32
  
//打开串口  
static int open_port(void)  
{  
    int fd;     //串口的标识符  
    //O_NOCTTY用来告诉Linux这个程序不会成为“控制终端”  
    //O_NDELAY用来告诉Linux这个程序不关心DCD信号  
    fd=open(serial_device,O_RDWR | O_NOCTTY | O_NDELAY);  
    if(fd == -1)  
    {  
        //不能打开串口  
        perror("open_port: Unable to open /dev/ttyS0 -");  
        return(fd);  
    }  
    else  
    {  
        fcntl(fd, F_SETFL, 0);  
        //printf("open ttys1 .....\n");  
        return(fd);  
    }  
}  
  
//设置波特率  
static void set_speed_and_parity(int fd, int speed)  
{  
    int i=0;        //设置循环标志——注意不要在for内设置，否则会出错  
    struct termios Opt; //定义termios结构  
    if(tcgetattr(fd,&Opt)!=0)  
    {  
        perror("tcgetattr fd");  
        return;  
    }  
    tcflush(fd, TCIOFLUSH);  
    cfsetispeed(&Opt, speed);  
    cfsetospeed(&Opt, speed);  
    /*tcsetattr函数标志： 
    TCSANOW：立即执行而不等待数据发送或者接受完成。 
    TCSADRAIN：等待所有数据传递完成后执行。 
    TCSAFLUSH：Flush input and output buffers and make the change 
    */  
    if(tcsetattr(fd, TCSANOW, &Opt) != 0)  
    {     
        //perror("tcsetattr fd");  
        return;  
    }  
    tcflush(fd, TCIOFLUSH);  
    //设置奇偶校验——默认8个数据位、没有校验位  
          
    Opt.c_cflag &= ~PARENB;  
    Opt.c_cflag &= ~CSTOPB;  
    Opt.c_cflag &= ~CSIZE;  
    Opt.c_cflag |= CS8;  
      
    //其他的一些配置  
    //原始输入，输入字符只是被原封不动的接收  
    Opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
    //软件流控制无效，因为硬件没有硬件流控制，所以就不需要管了  
    Opt.c_iflag &= ~(IXON | IXOFF | IXANY);  
    //原始输出方式可以通过在c_oflag中重置OPOST选项来选择：  
    Opt.c_oflag |= ~OPOST;  
    //VMIN可以指定读取的最小字符数。如果它被设置为0，那么VTIME值则会指定每个字符读取的等待时间。  
    Opt.c_cc[VTIME] = 0;  
    Opt.c_cc[VMIN] = 0;  
      
//      Opt.c_cflag &= ~INPCK;  
//      Opt.c_cflag |= (CLOCAL | CREAD);  
//    
//      Opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
//   
//      Opt.c_oflag &= ~OPOST;  
    Opt.c_oflag &= ~(ONLCR | OCRNL);      
  
    Opt.c_iflag &= ~(ICRNL | INLCR);  
//      Opt.c_iflag &= ~(IXON | IXOFF | IXANY);      
//      
//      Opt.c_cc[VTIME] = 0;  
//      Opt.c_cc[VMIN] = 0;  
      
    tcflush(fd, TCIOFLUSH);  
  
}  
  
  
/** 
  *串口发送数据函数 
  *fd：串口描述符 
  *data：待发送数据 
  *datalen：数据长度 
  */  
static int serial_write(int fd ,char *data, int datalen)  
{  
    int len=0;  
    //获取实际传输数据的长度  
    len=write(fd,data,datalen);  
    //printf("send data OK! datalen=%d\n",len);  
    return len;   
}  
  
/**  
  *串口接收数据  
  *要求启动后，在pc端发送ascii文件  
  */   
static int serial_read(int fd,char buff[],int datalen)  
{  
    int nread=0;  
    //printf("Ready for receiving data...");  
    nread=read(fd,buff,datalen);  
    if(nread>0&&buff[0]!='\n')  
    {  
        //printf("readlength=%d\n",nread);  
        buff[nread]='\0';  
        //printf("%s\n",buff);  
    }  
    return nread;  
}  
  
static int serialport()  
{  
    int fd;   
    //打开串口  
    if((fd=open_port())<0)  
        {  
            perror("open_port error");  
            return 0;  
        }  
    //设置波特率和校验位  
    set_speed_and_parity(fd,115200);  
    return (fd);  
}  
  
double get_sensor_data()  
{  
    int fd;  
	double data = 0;
    int nread,i,n =0,datalen=16, len = 0;  
    char testbuff[]="Hello\r\n";  
    char readbuff[16];  
      
    fd=serialport();  
    //printf("fd=%d\n",fd);  
    //尝试先写内容  
    nread=write(fd,testbuff,datalen);  
    if(nread==-1) {  
        return 0;  
    }  
    else{  
        //printf("the test is ok!\n");  
    }     

	bzero(readbuff, sizeof(readbuff));  
	datalen=serial_read(fd,readbuff,16);  
	if(datalen > 0&&readbuff[0]!='\n')  
	{  
		//printf("readbuff:%s\n",readbuff);
		data = strtod(readbuff,NULL);//字符串转成double
    } 
	return data;
}  

#endif