#ifndef _YW_CHR_LOCKED_H_
#define _YW_CHR_LOCKED_H_
#define MY_IOC_MAGIC 'x' //定义幻数
//#define MY_MAX_NR 3 //定义命令的最大序数
#define MY_IOC_RESET       _IO(MY_IOC_MAGIC,0)//清空分配的空间 0 
#define MY_IOCP_GET_LENS   _IOR(MY_IOC_MAGIC,1,int)//通过ioctl第三个参数以指针的形式返回数据 通过指针获取字符串的长度, 1
#define MY_IOCV_GET_LENS   _IO(MY_IOC_MAGIC,2)//通过ioctl返回值返回字符串的长度 2
#define MY_IOCP_SET_LENS   _IOW(MY_IOC_MAGIC,3,int)//通过指针 设置字符串的长度3通过ioctl第三个参数以指针的形式将要设置的值传递进去
#define MY_IOCV_SET_LENS   _IO(MY_IOC_MAGIC,4)//通过值设置字符串的长度 4 直接传入



#define MY_IOC_MAXNR 4
#endif /*_YW_CHR_LOCKED_H_*/
