#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <sys/time.h>


#include "v4l2_example.h"


int fd;
struct v4l2_buffer buf;

struct buffer *buffers;

void v4l2_print(){
    printf("yes");
}

int v4l2_init()
{
    struct v4l2_capability cap;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_format fmt;
    struct v4l2_streamparm stream_para;

    //打开摄像头设备
    if ((fd = open(FILE_VIDEO, O_RDWR)) == -1) {
        printf("Error opening V4L interface\n");
        return FALSE;
    }

    //查询设备属性
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
        printf("Error opening device %s: unable to query device.\n",FILE_VIDEO);
        return FALSE;
    }else {
        printf("driver:\t\t%s\n",cap.driver);
        printf("card:\t\t%s\n",cap.card);
        printf("bus_info:\t%s\n",cap.bus_info);
        printf("version:\t%d\n",cap.version);
        printf("capabilities:\t%x\n",cap.capabilities);
        
        if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE) {
            printf("Device %s: supports capture.\n",FILE_VIDEO);
        }

        if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING) {
            printf("Device %s: supports streaming.\n",FILE_VIDEO);
        }
    }
    //显示所有支持帧格式
    fmtdesc.index=0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    printf("Support format:\n");
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1) {
        printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
        fmtdesc.index++;
    }

    //检查是否支持某帧格式
    struct v4l2_format fmt_test;
    fmt_test.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt_test.fmt.pix.pixelformat=V4L2_PIX_FMT_RGB32;
    if(ioctl(fd,VIDIOC_TRY_FMT,&fmt_test)==-1) {
        printf("not support format RGB32!\n");      
    }else{
        printf("support format RGB32\n");
    }

    //查看及设置当前格式
    printf("set fmt...\n");
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32; //jpg格式
    //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//yuv格式

    fmt.fmt.pix.height = IMAGEHEIGHT;
    fmt.fmt.pix.width = IMAGEWIDTH;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    printf("fmt.type:\t\t%d\n",fmt.type);
    printf("pix.pixelformat:\t%c%c%c%c\n",fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF,(fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
    printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);
    printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);
    printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);

    if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1){
        printf("Unable to set format\n");
        return FALSE;
    }

    printf("get fmt...\n"); 
    if(ioctl(fd, VIDIOC_G_FMT, &fmt) == -1) {
        printf("Unable to get format\n");
        return FALSE;
    }else{
        printf("fmt.type:\t\t%d\n",fmt.type);
        printf("pix.pixelformat:\t%c%c%c%c\n",fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF,(fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
        printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);
        printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);
        printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);
    }

    //设置及查看帧速率，这里只能是30帧，就是1秒采集30张图
    memset(&stream_para, 0, sizeof(struct v4l2_streamparm));
    stream_para.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    stream_para.parm.capture.timeperframe.denominator = 30;
    stream_para.parm.capture.timeperframe.numerator = 1;

    if(ioctl(fd, VIDIOC_S_PARM, &stream_para) == -1) {
        printf("Unable to set frame rate\n");
        return FALSE;
    }
    if(ioctl(fd, VIDIOC_G_PARM, &stream_para) == -1) {
        printf("Unable to get frame rate\n");
        return FALSE;       
    }else{
        printf("numerator:%d\ndenominator:%d\n",stream_para.parm.capture.timeperframe.numerator,stream_para.parm.capture.timeperframe.denominator);
    }
    return TRUE;
}

int v4l2_mem_ops() {

    unsigned int n_buffers;
    struct v4l2_requestbuffers req;
    
    //申请帧缓冲
    req.count=FRAME_NUM;
    req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory=V4L2_MEMORY_MMAP;
    if(ioctl(fd,VIDIOC_REQBUFS,&req)==-1) {
        printf("request for buffers error\n");
        return FALSE;
    }

    // 申请用户空间的地址列
    buffers = malloc(req.count*sizeof (*buffers));
    if (!buffers) {
        printf ("out of memory!\n");
        return FALSE;
    }
    
    // 进行内存映射
    for (n_buffers = 0; n_buffers < FRAME_NUM; n_buffers++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        //查询
        if (ioctl (fd, VIDIOC_QUERYBUF, &buf) == -1) {
            printf("query buffer error\n");
            return FALSE;
        }

        //映射
        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL,buf.length,PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[n_buffers].start == MAP_FAILED) {
            printf("buffer map error\n");
            return FALSE;
        }
    }
    return TRUE;    
}


static char start = 0;
int v4l2_frame_process()
{
    unsigned int n_buffers;
    enum v4l2_buf_type type;
    //入队和开启采集
    if (!start) {
        for (n_buffers = 0; n_buffers < FRAME_NUM; n_buffers++) {
            buf.index = n_buffers;
            ioctl(fd, VIDIOC_QBUF, &buf);
        }
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(fd, VIDIOC_STREAMON, &type);
        start = 1;
    }
    for(n_buffers = 0; n_buffers < FRAME_NUM; n_buffers++){
        //出队
        buf.index = n_buffers;
        ioctl(fd, VIDIOC_DQBUF, &buf);
        ioctl(fd, VIDIOC_QBUF, &buf);
    }
    return TRUE;    
}


int v4l2_release() {
    unsigned int n_buffers;
    enum v4l2_buf_type type;

    //关闭流
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMON, &type);
    
    //关闭内存映射
    for(n_buffers=0;n_buffers<FRAME_NUM;n_buffers++) {
        munmap(buffers[n_buffers].start,buffers[n_buffers].length);
    }
    
    //释放自己申请的内存
    free(buffers);
    
    //关闭设备
    close(fd);
    return TRUE;
}
