/*
 *按照教程学习串口的编程
 *网址1：http://blog.csdn.net/shanzhizi/article/details/9241393
 *网址2：http://blog.csdn.net/shui1025701856/article/details/7571686
 *网址3：http://wenku.baidu.com/link?url=TsZc-rSDq_8ayNdWXYzkJRHLEowtx7OQMiFj_BH4M3VfvDrw4hU785aIciwS1xAmViJmz6pN0ety1_Zv8-AM2OAflOB3-UiMu9RvcSQKrAG
 */
//头文件
#include <stdio.h> 	//标准输入输出定义
#include <stdlib.h>	//标准函数库定义
#include <unistd.h>	//Unix标准函数定义
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>//时钟同步函数库
#include <sys/socket.h>
#include <fcntl.h> 	//文件控制定义
#include <termios.h>	//POSIX中断控制定义
#include <errno.h>	//错误号定义
#include <string.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#define serial_device "/dev/ttyS1"

//设定文件，以便保存mac信息
FILE *fp;

char buff[512] = "";//定义数据字符串
char readbuff[100] = "";//串口读取
int package_flag=0;//定义数据包的标志位
int mac_num=0;//定义节点设备的数量
int mac_num_recent=0;//定义当前节点mac的位置
int mac_add=0;//判断当前是否为增加设备
int port_add=0;//判断当前是否为增加资源
//定义数据包各个内容名称
char mac_address[100][100];
char res_num[100];
char res_flags[100];
char res_name[100][100];
char res_type[100][100];
char res_unit[100][100];
char res_port[100][100];
char res_val[100];


//定义红外数据存储
char IR_address[100][100];
int IR_address_num=0;



//打开串口
int open_port(void)
{
	int fd;		//串口的标识符
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
		printf("open ttys1 .....\n");
		return(fd);
	}
}

//设置波特率
void set_speed_and_parity(int fd, int speed)
{
	int i=0;		//设置循环标志——注意不要在for内设置，否则会出错
	struct termios Opt;	//定义termios结构
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
		perror("tcsetattr fd");
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
	
//  Opt.c_cflag &= ~INPCK;
// 	Opt.c_cflag |= (CLOCAL | CREAD);
//	
// 	Opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
// 
//  Opt.c_oflag &= ~OPOST;
	Opt.c_oflag &= ~(ONLCR | OCRNL);    
//
	Opt.c_iflag &= ~(ICRNL | INLCR);
//  Opt.c_iflag &= ~(IXON | IXOFF | IXANY);    
//    
//  Opt.c_cc[VTIME] = 0;
//  Opt.c_cc[VMIN] = 0;
    
	tcflush(fd, TCIOFLUSH);

}


/**
  *串口发送数据函数
  *fd：串口描述符
  *data：待发送数据
  *datalen：数据长度
  */
int serial_write(int fd ,char* buff, int datalen)
{
	int len=0;
	//获取实际传输数据的长度
	len=write(fd,buff,datalen);
	printf("send data OK! datalen=%d\n",len);
	return len;	
}

/** 
  *串口接收数据 
  *要求启动后，在pc端发送ascii文件 
  */ 
int serial_read(int fd,char readbuff[],int datalen)
{
	int nread=0;
	printf("Ready for receiving data...");
	nread=read(fd,readbuff,datalen);
	if(nread>0)
	{
		printf("readlength=%d\n",nread);
		//printf("%s\n",readbuff);
	}
	return nread;
}

//串口控制
int serialport()
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

//时钟同步函数
void clock_synchronized(int fd)
{
    char buff_temp[100]="";//传输值
    
    //获取当前系统时间
    time_t timep;
    time (&timep);
//    struct tm *timenow;
//    timenow = localtime(&timep);
    char* recent_time=ctime(&timep);
    printf("%s",recent_time);
    
    //时间同步时钟传输值
    strcat(buff_temp,mac_address[mac_num_recent]);
    strcat(buff_temp,",");
    strcat(buff_temp,"s:");
    strcat(buff_temp,recent_time);
    
    printf("%s",buff_temp);
    
    //int nwrite=serial_write(fd,buff_temp,512);
}

//数据分析——截取出有用字段
int data_analysis(int fd, char readbuff[])
{
	int stop_buf = 0;//作为标识符，检测是否存在规定的字符串。
	printf("start to analysis...\n");
	char *readbuff_temp;
	int i = 0, j = 0;
//	if (readbuff[0] == 's'&&readbuff[1] == 't'&&readbuff[2] == 't'&&readbuff[3] == ':')
    if(strstr(readbuff,"stt:"))
	{
		if(strlen(readbuff)>18)
        {
			stop_buf=0;
//			for (i = 0; i < strlen(readbuff); i++)
//			{
//				if (readbuff[i] == ':' && readbuff[i+1] == 's' && readbuff[i+2] == 't' && readbuff[i+3] == 't')
			if(strstr(readbuff,":stt"))
			{
				stop_buf = 1;
//				for(j=i+4;j<strlen(readbuff)+1;j++)
//					readbuff[j] = '\0';
				//break;
			}
//            }
			printf("readbuff=%s\n",readbuff);
			strcpy(buff,readbuff);
            printf("buff=%s\n", buff);
            printf("stop_buff=%d\n",stop_buf);
            memset(readbuff, 0, sizeof(readbuff));
//			bzero(readbuff, sizeof(readbuff));
//			while(serial_read(fd, readbuff, 512)>3);
//            strcat(buff,readbuff);
		}
		//serial_write(fd, buff, sizeof(buff));
		//printf("buff=%s\n", buff);
	}
    return(stop_buf);
}

//数据解封装
void data_unpackaging(int fd, char buff[])
{
	printf("start to package...\n");
	//char res_val[100] = ",\"Res_val\":";
	//char res_val_temp[100]="";

	int i=0,j=0,k=0;//循环标识符
    
    //定义一个mac_address的中间变量
    char mac_address_temp[100];
    
    //设置资源名称、资源类型、资源端口、资源内容的数量
    int res_name_num=0;
    int res_unit_num=0;
    int res_type_num=0;
    int res_port_num=0;
    
	for (i = 0; i<strlen(buff); i++)
    {
        if(buff[i]==','&&buff[i+2]==':')
        {
            switch (buff[i+1])
            {
                case 'm':
                    for(j=0;j<strlen(buff)-i-3;j++)
                    {
                        if (buff[j+i+3]==','||buff[j+i+3]==':')
                        {
                            i=j+i+2;
							mac_address_temp[j] = '\0';
                            for(k=0;k<mac_num;k++)
                            {
                                //printf("k=%d\n",k);
                                if(strcmp(mac_address_temp,mac_address[k])==0)
                                {
                                    mac_num_recent=k;
                                    mac_add=0;
                                    break;
                                }
                                else
                                {
                                    mac_add=1;
                                }
                            }
                            if(mac_add==1||mac_num==0)
                            {
                                strcat(mac_address[mac_num],mac_address_temp);
                                mac_num_recent=mac_num;
                                mac_add=1;
                                mac_num++;
                                //printf("HERE\n");
                            }
                            break;
                        }
						mac_address_temp[j] = buff[j + i + 3];
                    }
                    break;
                case 'c':
					for (j = 0; j<strlen(buff) - i - 3; j++)
                    {
						if (buff[j + i + 3] == ','||buff[j+i+3]==':')
                        {
							i = j + i + 2;
							res_num[j] = '\0';
                            int res_number=atoi(res_num);
                            printf("res_number=%d\n",res_number);
                            break;
                        }
						res_num[j] = buff[j + i + 3];
                    }
                    break;
                case 'f':
                    for (j = 0; j<strlen(buff) - i - 3; j++)
                    {
                        if (buff[j + i + 3] == ','||buff[j+i+3]==':')
                        {
                            i = j + i + 2;
                            res_flags[j] = '\0';
                            break;
                        }
                        res_flags[j] = buff[j + i + 3];
                     }
                     break;

                case 'n':
					for (j = 0; j<strlen(buff) - i - 3; j++)
                    {
						if (buff[j + i + 3] == ','||buff[j+i+3]==':')
                        {
							i = j + i + 2;
							res_name[res_name_num][j] = '\0';
                            res_name_num++;
                            break;
                        }
						res_name[res_name_num][j] = buff[j + i + 3];
                    }
                    break;
                case 't':
					for (j = 0; j<strlen(buff) - i - 3; j++)
                    {
						if (buff[j + i + 3] == ','||buff[j+i+3]==':')
                        {
							i = j + i + 2;
							res_type[res_type_num][j] = '\0';
//                            if((strcmp(res_type[res_type_num],"TV")==0)||(strcmp(res_type[res_type_num],"camera")==0))
//							{
//								fp=fopen("file_map","w");
//								fprintf(fp,"%s\n",mac_address[mac_num_recent]);
//								fclose(fp);
//							}
//							res_type_num++;
                            break;
                        }
						res_type[res_type_num][j] = buff[j + i + 3];
                    }
                    break;
                case 'u':
					for (j = 0; j<strlen(buff) - i - 3; j++)
                    {
						if (buff[j + i + 3] == ','||buff[j+i+3]==':')
                        {
							i = j + i + 2;
							res_unit[res_unit_num][j] = '\0';
                            res_unit_num++;
                            break;
                        }
						res_unit[res_unit_num][j] = buff[j + i + 3];
                    }
					break;
				case 'p':
					for (j = 0; j<strlen(buff) - i - 3; j++)
					{
						if (buff[j + i + 3] == ','||buff[j+i+3]==':')
						{
							i = j + i + 2;
							res_port[res_port_num][j] = '\0';
                            res_port_num++;
							break;
						}
						res_port[res_port_num][j] = buff[j + i + 3];
					}
					break;
				case 'v':
					for (j = 0 ; j<strlen(buff) - i - 3; j++)
					{
						if (buff[j + i + 3] == ','||buff[j+i+3]==':')
						{
							i = j + i + 2;
							//res_val_temp[j] = '\0';
							//strcat(res_val,res_val_temp);
							res_val[j]='\0';
							break;
						}
						//res_val_temp[j] = buff[j + i + 3];
						res_val[j]=buff[j+i+3];
					}
					break;
                case 's':
					for (j = 0 ; j<strlen(buff) - i - 3; j++)
					{
						if (buff[j + i + 3] == ','||buff[j+i+3]==':')
						{
							i = j + i + 2;
							break;
						}
                        if (buff[j+i+3]=='r')
                        {
                            package_flag=0;
                        }
                        else if (buff[j+i+3]=='d')
                        {
                            package_flag=1;
                        }
											
                    }
					break;
            }
        }
    }
    
	//数据解析
	printf("buff=%s\n", buff);

}

//数据socket传输
void data_socket(const char* ip,int port,char sendline[])
{
    int    sockfd, n,rec_len;
    struct sockaddr_in    servaddr;
    //	const char* ip=argv[1];
    //	int port=atoi(argv[2]);
    
    
    //  Json::Reader reader;
    //	sendline="{\"Mac_address\":\"afgeqw\",\"Res_num\":2}";
	printf("sendline ready");
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    }
    
    
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if( inet_pton(AF_INET,ip, &servaddr.sin_addr) <= 0){
        //      printf("inet_pton error for %s\n",argv[1]);
        printf("inet_pton error for %s\n",ip);
        exit(0);
    }
    
	printf("send begin");
    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
    
    printf("send msg to server: \n");
    
	printf("buff:%s\n", sendline);
    //  printf("data socket is begin %s\n",sendline);
    // fgets(sendline, 4096, stdin);
    if( send(sockfd, sendline, strlen(sendline), 0) < 0)
    {
		printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
    }
    printf("send msg end!!\n");
    close(sockfd);
    //exit(0);
}

//红外数据存储
void IR_data()
{
	//调整IR_address内的内容
	if(package_flag==0 && strstr(res_flags,"0"))
	{
		if(strstr(res_type[0],"TV"))
		{
			strcpy(IR_address[IR_address_num],mac_address[mac_num_recent]);
			strcat(IR_address[IR_address_num],",TV");
			IR_address_num++;
		}
		else if(strstr(res_type[0],"camera"))
		{
			strcpy(IR_address[IR_address_num],mac_address[mac_num_recent]);
			strcat(IR_address[IR_address_num],",camera");
			IR_address_num++;
		}
	}
	else if(package_flag==0 && strstr(res_flags,"1"))
	{
		int i=0;
		for(;i<IR_address_num;i++)
		{
			if(strstr(IR_address[i],res_type[0]))
			{
				memset(IR_address[i],0,sizeof(IR_address[i]));
				break;
			}
		}
	}
	//如果是注册或删除信息，就重新生成file_map文件
	if(package_flag==0 && (strstr(res_type[0],"TV") || strstr(res_type[0],"camera")))
	{
		fp=fopen("file_map","w");
		int i=0;
		for(;i<IR_address_num;i++)
		{
				fprintf(fp,"%s\n",IR_address[i]);
		}
		fclose(fp);
	}
}



//数据封装
void data_packaging(int fd,char buff[])
{
    int i=0;
    int res_number=atoi(res_num);
    if (package_flag==0)
    {
        //进一步封装，以结构体封装
        strcpy(buff,"{\"Mac_address\":\"");
        strcat(buff, mac_address[mac_num_recent]);
        strcat(buff, "\",\"Res_num\":");
        strcat(buff,res_num);
        strcat(buff, ",\"flags\":");
        strcat(buff,res_flags);
        strcat(buff, ",\"Res\":[");
        for(i=0;i<res_number;i++)
        {
            if(i>0)
            {
                strcat(buff, ",");
            }
            strcat(buff, "{\"Res_name\":\"");
            strcat(buff, res_name[i]);
            strcat(buff, "\",\"Res_type\":\"");
            strcat(buff, res_type[i]);
            strcat(buff, "\",\"Res_unit\":\"");
            strcat(buff, res_unit[i]);
            strcat(buff, "\",\"Res_port\":");
            strcat(buff, res_port[i]);
            strcat(buff,"}");
        }
        strcat(buff,"]}");
        printf("before socket,buff:%s\n", buff);
        
        clock_synchronized(fd);
    }
    else if(package_flag==1)
    {
        //进一步封装，以结构体封装
        //判断设备是否已经注册，如果没有注册，会首先进行注册
//        if (mac_add==1)
//        {
//            char buff_temp[100] = "{\"Mac_address\":\"";
//            strcat(buff_temp,mac_address[mac_num_recent]);
//            strcat(buff_temp,"\",\"Res_num\":1,\"flags\":0,\"Res\":[{\"Res_port\":");
//            strcat(buff_temp,res_port[0]);
//            strcat(buff_temp,"}]}");
//            char* ip_add="127.0.0.1";
//            int port_add=8000;
//            
//            clock_synchronized(fd);
//            
//            data_socket(ip_add, port_add, buff_temp);
//        }
        strcpy(buff,"{\"Mac_addr\":\"");
        strcat(buff, mac_address[mac_num_recent]);
        strcat(buff, "\",\"Res_port\":");
        strcat(buff,res_port[0]);
        strcat(buff, ",\"Res_val\":");
        strcat(buff, res_val);
        strcat(buff,"}");
        printf("before socket,buff:%s\n", buff);
    }
}

//数据清空函数（为防止冲突，而将设置的数据在用完后全部清零。）同时清空标志位
void data_delete()
{
    memset(buff, 0, sizeof(buff));
	memset(readbuff,0,sizeof(buff));
    memset(res_num, 0, sizeof(res_num));
    memset(res_flags, 0, sizeof(res_flags));
    memset(res_name, 0, sizeof(res_name));
    memset(res_type, 0, sizeof(res_type));
    memset(res_unit, 0, sizeof(res_unit));
    memset(res_port, 0, sizeof(res_port));
    memset(res_val, 0, sizeof(res_val));
}
//主函数
int main(void)
{
	int fd;
	int nread,i,n =0,datalen=255, len = 0;
	char testbuff[]="Hello\r\n";
	//char *buff="";
	char* ip = "127.0.0.1";
	int port = 8000;
	
	fd=serialport();
	printf("fd=%d\n",fd);
//	//尝试先写内容
//	nread=write(fd,testbuff,datalen);
//	if(nread==-1)
//	{
//		printf("write bad\n");
//	}
//	else{
//		printf("the test is ok!\n");
//	}	
	//循环读取串口并输出
	while(1)
	{
        printf("enter the while loop\n");
		bzero(readbuff, sizeof(readbuff));
		nread = serial_read(fd, readbuff, 512);
		if(nread>0&&data_analysis(fd, readbuff)==1)
		{
			//data_analysis(fd, readbuff);
            data_unpackaging(fd, buff);
			data_packaging(fd, buff);
			IR_data();
			if(package_flag==0)
			{
				port=8000;
                printf("port=%d\n",port);
                clock_synchronized(fd);
			}
			else if(package_flag==1)
			{
				port=8001;
                printf("port=%d\n",port);
			}
			data_socket(ip, port, buff);
            data_delete();
            //bzero(buff, '\0', sizeof(buff));
        }
		//serial_write(fd, buff, datalen);
		/*while（1）
		{
			bzero(readbuff, sizeof(readbuff));
			datalen = serial_read(fd, readbuff, 512);
			if (datalen > 0)
			{
				if (readbuff[0] == 'O'&&readbuff[1] == 'F'&&readbuff[2] == 'F')
				{
					break;
				}
				serial_write(fd, readbuff, datalen);
			}
		}*/
	}
}
