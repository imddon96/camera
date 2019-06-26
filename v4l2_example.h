#ifndef __LIBV4L2_H
#define __LIBV4L2_H

#ifdef __cplusplus
extern "C" {

#endif
#define TRUE            (1)
#define FALSE           (0)

#define FILE_VIDEO      "/dev/video0"
#define IMAGE           "./img/demo"

#define IMAGEWIDTH      160
#define IMAGEHEIGHT     120

#define FRAME_NUM       1  //4

#define LOOP_TIME       60

extern struct buffer
{
    void * start;
    unsigned int length;
    long long int timestamp;
} *buffers;
extern unsigned char* gInterfaceBuf;
extern int gLength;
extern int loop;
extern int v4l2_init(void);
extern int v4l2_mem_ops(void);
extern int v4l2_frame_process(void);
extern int v4l2_release(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
