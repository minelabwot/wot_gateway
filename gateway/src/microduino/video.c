#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>                                                                                                                            

typedef struct
{
        void *start;
        int length;
}BUFTYPE;

BUFTYPE *user_buf;
int n_buffer = 0;
char *_pictureName;

//开启摄像头，开启失败返回错误信息                                                         
int OpenCamera()
{
        int fd;

        if((fd = open("/dev/video0",O_RDWR | O_NONBLOCK)) < 0)
        {
                perror("Fail to open");
                exit(EXIT_FAILURE);
        }

        return fd;
}


//设置摄像头                                                         
int SetCamera(fd)
{
        struct v4l2_fmtdesc fmt;
        struct v4l2_capability cap;
        struct v4l2_format stream_fmt;
        int ret;

        //.......................................                               
        memset(&fmt,0,sizeof(fmt));
        fmt.index = 0;
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        while((ret = ioctl(fd,VIDIOC_ENUM_FMT,&fmt)) == 0)
        {
                fmt.index ++ ;

                printf("{pixelformat = %c%c%c%c},description = '%s'\n",
                                fmt.pixelformat & 0xff,(fmt.pixelformat >> 8)&0xff,
                                (fmt.pixelformat >> 16) & 0xff,(fmt.pixelformat >> 24)&0xff,
                                fmt.description);
        }

        //.................................                                     
        ret = ioctl(fd,VIDIOC_QUERYCAP,&cap);
        if(ret < 0){
                perror("FAIL to ioctl VIDIOC_QUERYCAP");
                exit(EXIT_FAILURE);
        }

        //.......................................                               
        if(!(cap.capabilities & V4L2_BUF_TYPE_VIDEO_CAPTURE))
        {
                printf("The Current device is not a video capture device\n");
                exit(EXIT_FAILURE);

        }

        //.................................                                     
        if(!(cap.capabilities & V4L2_CAP_STREAMING))
        {
                printf("The Current device does not support streaming i/o\n");
                exit(EXIT_FAILURE);
        }

        //............................................................          
        //...,..................(JPEG,YUYV,MJPEG.........)                      
        stream_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        stream_fmt.fmt.pix.width = 680;
        stream_fmt.fmt.pix.height = 480;
        stream_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
        stream_fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

        if(-1 == ioctl(fd,VIDIOC_S_FMT,&stream_fmt))
        {
                perror("Fail to ioctl");
                exit(EXIT_FAILURE);
        }

        //设备文件映射到内存，mmap（）函数 加速了I/O的访问                                                                                               
        int i;
        struct v4l2_requestbuffers reqbuf;

        bzero(&reqbuf,sizeof(reqbuf));
        reqbuf.count = 4;
        reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        reqbuf.memory = V4L2_MEMORY_MMAP;

        //.....................(................................................
        //..............................reqbuf.count............................
        if(-1 == ioctl(fd,VIDIOC_REQBUFS,&reqbuf))
        {
                perror("Fail to ioctl 'VIDIOC_REQBUFS'");
                exit(EXIT_FAILURE);
        }

        n_buffer = reqbuf.count;

        printf("n_buffer = %d\n",n_buffer);

        //calloc 在内存的动态存储区reqbuf.count个长度为(*user_buf)的连续空间，返回其实地址
        //的指针                                                                               
        user_buf = (BUFTYPE*)calloc(reqbuf.count,sizeof(*user_buf));
        if(user_buf == NULL){
                fprintf(stderr,"Out of memory\n");
                exit(EXIT_FAILURE);
        }

        //.............................................                         
        for(i = 0; i < reqbuf.count; i ++)
        {
                struct v4l2_buffer buf;

                bzero(&buf,sizeof(buf));
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;
                //.......................................                       
                if(-1 == ioctl(fd,VIDIOC_QUERYBUF,&buf))
                {
                        perror("Fail to ioctl : VIDIOC_QUERYBUF");
                        exit(EXIT_FAILURE);
                }

                user_buf[i].length = buf.length;
                user_buf[i].start =
                        mmap(
                                        NULL,/*start anywhere*/
                                        buf.length,
                                        PROT_READ | PROT_WRITE,
                                        MAP_SHARED,
                                        fd,buf.m.offset
                                );
               if(MAP_FAILED == user_buf[i].start)
                {

                        perror("Fail to mmap");
                        exit(EXIT_FAILURE);
                }
        }


        enum v4l2_buf_type type;

        //驱动摄像头开始抓取照片的准备工作，涉及到内核  V4L2                  
        for(i = 0;i < n_buffer;i ++)
        {
                struct v4l2_buffer buf;

                bzero(&buf,sizeof(buf));
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;

                if(-1 == ioctl(fd,VIDIOC_QBUF,&buf))
                {
                        perror("Fail to ioctl 'VIDIOC_QBUF'");
                        exit(EXIT_FAILURE);
                }
        }

        //ioctl是驱动程序中对设备的I/O通道进行管理的函数                                                    
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if(-1 == ioctl(fd,VIDIOC_STREAMON,&type))
        {
                printf("i = %d.\n",i);
                perror("Fail to ioctl 'VIDIOC_STREAMON'");
                exit(EXIT_FAILURE);
        }

        return 0;

}


//....................................     
int process_image(void*addr,int length)
{
        FILE *fp;
//      static int num = 0;  
        char picture_name[256];

        //将抓取的照片存入下面路径                                                                        
        sprintf(picture_name,"/mnt/usb/image/%s.jpg",_pictureName);

        if((fp = fopen(picture_name,"w")) == NULL)
        {
                perror("Fail to fopen");
                exit(EXIT_FAILURE);
        }

        fwrite(addr,length,1,fp);
        usleep(500);

        fclose(fp);

        return 0;
}

int read_frame(int fd)
{
        //v4l2(video 4 linux 2）是针对uvc免驱usb设备的编程框架，涉及到内核                                                            
        struct v4l2_buffer buf;
        unsigned int i;

        bzero(&buf,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        //ioctl是驱动程序中对设备的I/O通道进行管理的函数                                              
        if(-1 == ioctl(fd,VIDIOC_DQBUF,&buf))
        {
                perror("Fail to ioctl 'VIDIOC_DQBUF'");
                exit(EXIT_FAILURE);
        }


        assert(buf.index < n_buffer);
        //.............................................                         
        process_image(user_buf[buf.index].start,user_buf[buf.index].length);

        if(-1 == ioctl(fd,VIDIOC_QBUF,&buf))
        {
                perror("Fail to ioctl 'VIDIOC_QBUF'");
                exit(EXIT_FAILURE);
        }

        return 1;
}

int mainloop(int fd)
{
        //5秒进行一次抓取                                                                        
        while(1)

       {
                for(;;)
                {
                        fd_set fds;
                        struct timeval tv;
                        int r;

                        FD_ZERO(&fds);
                        FD_SET(fd,&fds);

                        /*Timeout*/
                        tv.tv_sec = 2;
                        tv.tv_usec = 0;

                        r = select(fd + 1,&fds,NULL,NULL,&tv);

                        if(-1 == r)
                        {
                                if(EINTR == errno)
                                        continue;

                                perror("Fail to select");
                                exit(EXIT_FAILURE);
                        }

                        if(0 == r)
                        {
                                fprintf(stderr,"select Timeout\n");
                                exit(EXIT_FAILURE);
                        }

                        if(read_frame(fd))
                                break;
                }
                sleep(5);
        }

        return 0;
}


//关闭摄像头                                                                       
int CloseCamera(int fd)
{

        enum v4l2_buf_type type;
        unsigned int i;
        //停止抓取照片
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if(-1 == ioctl(fd,VIDIOC_STREAMOFF,&type))
        {
                perror("Fail to ioctl 'VIDIOC_STREAMOFF'");
                exit(EXIT_FAILURE);
        }

        //释放内存空间
        for(i = 0;i < n_buffer;i ++)
        {
                if(-1 == munmap(user_buf[i].start,user_buf[i].length))
                {
                        exit(EXIT_FAILURE);

                }
        }

        free(user_buf);


        if(-1 == close(fd))
        {
                perror("Fail to close fd");
                exit(EXIT_FAILURE);
        }

        return 0;
}

//argc 参数的个数，argv是字符串数组，存的是参数                                                                                
int main(int argc,char** argv)
{

        int fd;
        if (argc < 2) {
                printf("usage:video [picturename]\n");     //执行文件运行方式./video 参数    
                exit(0);
        }

        _pictureName=argv[1];
        printf("%s",_pictureName);

        fd = OpenCamera();

        SetCamera(fd);


        mainloop(fd);


        CloseCamera(fd);

        return 0;
}

