#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/ioctl.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>  //copy_to_user cop_from_user
#include <linux/fs.h>  //注册函数 包含了 kdev_t.h MAJOR/MINOR/MKDEV 和 types.h  dev.t
#include<linux/moduleparam.h>
#include "yw_chr_locked.h"
#define DEV_SIZE 100


//                    <<设备报告错误信息,改变波特率,弹出光驱 弹出介质 修改设置，锁门>> 这些操作通过 ioctl方法来实现。>>对硬件控制
//1.定义命令（幻数），序号，传送方向，参数的大小
// #define TEST_MAGIC  0x81  //TEST_MAGIC的选择 查看   Documentation/userspace-api/ioctl-number.rst  罗列了ioctl中的所有幻数 没有用过的用                                           
//#define TEST_SIZE 0x10
//#define TEST_MAGIC 'x'//定义幻数 "幻数"是一个字母，数据长度也是8位，用一个特定的字母来标明设备类型，这和用一个数字是一样的


/*2.一般会有两种的传参方法：
1)整数，省力又省心，直接使用就可以了。
2)指针，通过指针的就传什么类型都可以了*/  
//#define TEST_CLEAR _IO(TEST_MAGIC, 0)// _IO(type，nr) 通过值设置   无数据传输
//#define TEST_READ _IOR(TEST_MAGIC, 1, char)//   通过指针返回    _IOR(type，nr，datatype)  值为1   从驱动中读数据 nr
//nr：用这个数来给自己的命令编号，占8bit(_IOC_NRBITS)
//#define TEST_WRITE _IOW(TEST_MAGIC, 2, char)//  通过指针设置    _IOWR(type，nr，datatype) 值为2  写数据到驱动
//#define TEST_MAXNR 2 //  定义命令的最大序数  为代码增加判断   _IOC_READ|_IOC_WRITE：双向数据传输。
/*正常不会混用 不是值传递 就是指针传递*/






                                  //0. cat /dev/mychardev  出现死循环打印log 无法进行rmmod操作  的bug 解决完成  
/*read之后 需要修改位置指针。否则会不循断循环读数。 解决完后只打印一次 更新一下偏移量     也就是更新为   偏移量加上count的个数 
    uint8_t *data = "Hello from the kernel world!";//cat /dev/device 会循环打印log
    size_t datalen = strlen(data)-(*offset);//数据的总量-偏移量
            //数据长度
    if (count > datalen) {   
            count = datalen; //将剩下的数据保存在count
    }
    //int size =count<(datalen-(*offset))?count:(datalen-(*offset));
    if (copy_to_user(buf, data, count)) { //data 指向的是打印了"Hello from the kernel world!"log的地址，所以无法read新write到内核空间中的新内容                             //size
            return -EFAULT;    
    }else{
    (*offset) += count;//read之后 需要修改位置指针。否则会不循断循环读数  更新一下偏移量     也就是更新为   偏移量加上count的个数
    }
               return count; */ 
    


                                    //1.Create your own character device with read/write permission (666)
/*1.1struct class结构体中  devnode：Callback to provide the devtmpfs.  
char *(*devnode)(struct device *dev, umode_t *mode);   设备节点的相对路径名 Callback to provide the devtmpfs
umode_t *mode 访问权限的分配，目录项的类型（文件、目录
1.2 udevadm info -a -n /dev/mychardev查看设备信息  SUBSYSTEM=="mychardev" 获取判断条件
在etc/udev/rules.d/目录下 使用root权限创建一个规则   创建一个文件 10-my-rules.rules 
通过vim编辑器   #SUBSYSTEM=="mychardev",GROUP="g"      //修改用户组  udv会判断SUBSYSTEM是不是等于mychardev 是的话执行赋值操作
#SUBSYSTEM=="mychardev",MODE="0666"     //修改权限   udv会判断SUBSYSTEM是不是等于mychardev  是的话将这个值赋给MODE进行保存完成      */



                                   //2.Driver with own private data for storage
/*container_of() 通过结构体成员获取整个字符设备结构体本身的地址  
此处我定义的结构体 成员包括 ->  
c（字符指针，用于保存字符串） n（保存字符串的个数） mycdev（注册设备号相关的结构体）
//当驱动需要持续读写文件的话，需要考虑并发的情况，需要给buf的读写加锁。
copy_to_user   read函数用 从内核到用户
copy_from_user write函数用 从用户到内核
struct inode   表示文件 我用到了成员i_cdev  保存字符设备结构的地址（struct cdev ）
struct filp    表示打开的文件描述符的结构体  用到了其成员 private_date*/


                                //3.Test application can write xxxx to driver and then read xxxx from driver




static int dev_major = 0;//自己分配的主设备号      minor次设备号
module_param(dev_major, int, S_IRUGO); //主设备号值  //类型（支持整形，字符指针和bool invbool）  //权限位(sys文件系统)
//指定模块的参数  需要#include<linux/moduleparam.h>和#include<linux/stat.h>  被#include <linux/module.h>包含了所以无需再加入
//#define MYCDEV_NR_DEVS 2
struct mychardev_char_dev  //实际的字符设备结构  
{
    struct cdev mycdev;
    char *c; //保存一个字符串    //读取这个设备就是将这个设备中的字符串给到用户空间 
                                //写这个设备就是将用户空间的字符串保存到这个字符设备当中           
    int n;//保存字符串的个数
    //unsigned int size;
    //char *buf;
    //char buf[100];
    
};

//static size_t maxdatalen = (size_t)MAX_BUFFER, numcopied;//最大缓冲区  编号
//static uint8_t databuf[MAX_BUFFER]="Hello from the kernel world!";//用来作为数据缓冲区
static char *mychardev_devnode(struct device *dev,umode_t *mode);
static int mychardev_open(struct inode *inode, struct file *file);
static int mychardev_release(struct inode *inode, struct file *file);
static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);
static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
//static char *mychardev_devnode(struct device *dev,umode_t *mode);
//static ssize_t show_my_device(struct device *dev,struct device_attribute *attr, char *buf);
//static ssize_t set_my_device(struct device *dev,struct device_attribute *attr,const char *buf, size_t len);  
static const struct file_operations mychardev_fops = {
    .owner          = THIS_MODULE,
    .open           = mychardev_open,
    .release        = mychardev_release,
    .read           = mychardev_read,
    .write          = mychardev_write,
    .unlocked_ioctl = mychardev_ioctl,//还有一个 compat_ioctl  用于32位程序运行于64位操作系统
};
static struct class *mychardev_class = NULL;//创建驱动设备文件相关的结构体变量
struct mychardev_char_dev *mychardev_dev;
//struct device *devp;
//static struct cdev mycdev;//注册设备号相关的结构体
//static mycdev_nr_devs=MYCDEV_NR_DEVS;//次设备号 可以拥有多个
//static DEVICE_ATTR(YE_device_test,436,show_my_device,set_my_device);
//dev_t dev;//高12位是主设备号  低20位是次设备号  主设备号和次设备号结合起来的设备号
//module_param(mycdev_nr_devs,int,S_IRUGO);
/*static  char mybuf[100]="yes";
//cat命令时,将会调用该函数
static ssize_t show_my_device(struct device *dev,struct device_attribute *attr, char *buf)        
{
    return sprintf(buf, "%s\n", mybuf);
}
//echo命令时,将会调用该函数
static ssize_t set_my_device(struct device *dev,struct device_attribute *attr,const char *buf, size_t len)        
{
    sprintf(mybuf, "%s", buf);
    return len;
}
//定义一个名字为my_device_test的设备属性文件
static DEVICE_ATTR(YE_device_test,436,show_my_device,set_my_device);*/

/*static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{    //Ioctl函数的实现通常是根据命令执行的一个switch语句。但是，当命令号不能匹配任何一个设备所支持的命令时，通常返回-EINVAL/-ENOTTY（“非法参数”）
    //cmd就是一个数，如果应用层传来的数值在驱动中有对应的操作
    int ret = 0;
     //1.传入指针需要检查安全性。memcpy函数尽量不要在内核中使用。
    // 2.copy_to_user.copy_from_user.get_user.put_user函数会再拷贝数据前检测指针的安全性。
     //3.如果在ioctl函数开头使用了accsee_ok检验数据，接下来的代码可以使用__put_user或__get_user这些不需要检测的函数
    
    struct mychardev_char_dev *mychardev_dev = file->private_data;//获得保存在dev地址
	switch (cmd) { //业务操作
            case TEST_CLEAR:
	memset(mychardev_dev->c, '0', TEST_SIZE);
	printk(KERN_INFO "test is set to zero\n");
		break;
	case TEST_READ://读
		ret = copy_to_user((struct test_dev *)arg, mychardev_dev->c, 4);//指针是从用户程序传来，所以必须检查安全性
                                     //地址                 //数据
        if(ret)
            {
            ret = - EFAULT;
                return ret;
            }
            printk(KERN_INFO "test:read str: %s\n", mychardev_dev->c);
		break;
	case TEST_WRITE://写
		ret = copy_from_user(mychardev_dev->c, (struct  mychardev_char_dev *)arg, 4);
            printk("test: write str %s \n", mychardev_dev->c);
		break;
	default:
		ret = -EINVAL;
	}
    return ret;
}*/
static long mychardev_ioctl (struct file *file, unsigned int cmd, unsigned long arg)
{
    struct mychardev_char_dev *mychardev_dev = file->private_data;
    int temp, err = 0;
    long retval = 0;
/*既然这么费劲定义了命令，当然要检验命令是否有效*/
    if(_IOC_TYPE(cmd) != MY_IOC_MAGIC)//检查幻数 是否合法
        return - EINVAL;
    if(_IOC_NR(cmd) > MY_IOC_MAXNR)//检查命令编号
        return - EINVAL; 
    if (_IOC_DIR(cmd)&_IOC_READ)//处理用户空间与内核空间数据交互       判断读是否ok
        err = ! access_ok((void __user *)arg,_IOC_SIZE(cmd));//用户空间地址 传入数据大小
    if (err)//0时不执行  1时执行
    return - EFAULT;
    switch(cmd){
    case MY_IOC_RESET:
        printk(KERN_INFO"ioctl reset\n");
        kfree(mychardev_dev->c);//空间释放        
        mychardev_dev->n = 0;//长度设置为0
        break;
    case MY_IOCP_GET_LENS:
        printk(KERN_INFO"ioctl get lens through pointer\n");
        retval=__put_user(mychardev_dev->n,(int __user *)arg);//传的数据长度    用户空间的地址
        break;
    case MY_IOCV_GET_LENS:// 值传递
        printk(KERN_INFO"ioctl get lens through value\n");
        return mychardev_dev->n;//值传递 返回这个值
        break;
    case MY_IOCP_SET_LENS:// 指针设置
        printk(KERN_INFO"ioctl get lens through pointer\n");
        if(!capable (CAP_SYS_ADMIN))// 判断是否拥有权限作修改  具备管理员权限才能修改
            return -EFAULT;
        retval=get_user(temp,(int __user *)arg); //从用户空间拿数据       保存数据的位置        通过用户空间传入的数据
        if(mychardev_dev->n > temp)         
            mychardev_dev->n=temp;
        printk(KERN_INFO" %d\n",mychardev_dev->n);
        break;
    case MY_IOCV_SET_LENS://值方式设置
        printk(KERN_INFO"ioctl get lens through value\n");
        if(!capable (CAP_SYS_ADMIN))
            return -EFAULT;
        mychardev_dev->n = min(mychardev_dev->n,(int)arg);//  第二个形参 直接是值
        printk(KERN_INFO" %d\n",mychardev_dev->n);
        break;
    default://前面对cmd命令进行了检查
        break;
    }
    return retval;
}
static char *mychardev_devnode(struct device *dev,umode_t *mode)
{
    if(!mode)
        return NULL;
    if(dev->devt == MKDEV(dev_major,0))
        *mode = 0666;
    return NULL;
}
static int __init mycdev_init(void)
{
    int err;
    struct device *devp;
    dev_t dev;//32位变量    //dev_t地址
    err = alloc_chrdev_region(&dev, 0, 1, "mychardev");
    //第一步：函数向内核申请下来的设备号 次设备号的起始 申请次设备号的个数 执行 cat /proc/devices显示的名称
    dev_major = MAJOR(dev); //提取主设备号
    mychardev_dev = kzalloc(sizeof(struct mychardev_char_dev) , GFP_KERNEL);
    mychardev_class = class_create(THIS_MODULE, "mychardev");
    mychardev_class->devnode = mychardev_devnode;
    //第三步：创建驱动的设备文件
    //for(i=0;i<mycdev_nr_devs;i++){     //mycdev_nr_devs是个数
    cdev_init(&mychardev_dev->mycdev, &mychardev_fops);//第二步：初始化字符设备驱动
    mychardev_dev->mycdev.owner = THIS_MODULE;
    //2.1初始化cdev
    cdev_add(&mychardev_dev->mycdev, MKDEV(dev_major, 0), 1);//2.2添加cdev//}inode
             //待添加到内核的字符设备结构  dev: 设备号  count: 添加的设备个数
    devp=device_create(mychardev_class, NULL, MKDEV(dev_major, 0), NULL, "mychardev"); //mychardev就是将来在/dev中创建的设备文件名
    /*if(sysfs_create_file(&(devp->kobj), &dev_attr_YE_device_test.attr)){    //在mytest_device设备目录下创建一个my_device_test属性文件
        return -1;
    }*/   
    printk("Driver installed\n");     
    return 0;
}

static void __exit mycdev_exit(void)
{    
    device_destroy(mychardev_class, MKDEV(dev_major, 0));//销毁创建的驱动文件
    cdev_del(&mychardev_dev->mycdev); //注销驱动
    class_unregister(mychardev_class);//释放设备号
    class_destroy(mychardev_class);//销毁创建的类
    kfree(mychardev_dev);//释放指针指向之前分配好的内存地址
    mychardev_dev=NULL;//指向NULL   做安全处理
    unregister_chrdev_region(MKDEV(dev_major, 0), 1);//设备号 要卸载的个数
    printk("Driver uninstalled\n");//是删除设备号列表中的元素
}
static int mychardev_open(struct inode *inode, struct file *file)
{ 
    struct mychardev_char_dev *mychardev_dev;
    printk("MYCHARDEV: open mychardev_dev%d %d\n", iminor(inode), MINOR(inode->i_cdev->dev));
   //由结构指针mychardev_dev接收  container_of 通过成员的地址（mycdev） 获取整个结构体类型地址（struct mychardev_char_dev）并保存在inode->i_cdev变量中
    mychardev_dev=container_of(inode->i_cdev,struct mychardev_char_dev,mycdev);   
    //将地址mychardev_dev 保存在文件描述符 file->private_data中 read 和write 进行读取作为后续操作                           
    file->private_data = mychardev_dev;
    return 0;
}
static int mychardev_release(struct inode *inode, struct file *file)
{
    printk("MYCHARDEV: Device close\n");
    return 0;
}                                             
static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{       
    size_t retval=0;
    int result;   
    struct mychardev_char_dev *mychardev_dev=file->private_data;
    //获得字符设备的地址  通过文件描述符中file->private_data
    printk(" read mychardev_dev%p\n",mychardev_dev);        
    //当我想限制读取的长度时  我可以这样如下设置
    if(*offset >= mychardev_dev->n)//指针偏移量是否大于数据的总量      
    goto out;//   到了文件末尾   返回到0
    if(*offset + count > mychardev_dev->n)//判断所需要读取的个数加上偏移量是否超过数据的总量
    count = mychardev_dev->n - *offset;//将剩下的数据保存在count
    //if (count > mychardev_dev->n-*offset) {
    //         count =mychardev_dev->n-*offset;
    // }
    result=copy_to_user(buf, mychardev_dev->c, count);//保存在用户缓存区  buf中
                       //目标地址   //内核空间  //字节数
    if (result) {                              
    retval= -EFAULT;//传输错误  返回
    }
    *offset += count;//传输成功的话 更新一下偏移量     也就是更新为   偏移量加上count的个数
      //result=count;
    return count;
    //return  result;  //终端中进行cat 时，系统默认当前终端为独立的一个进程，读取未结束，会一直/反复的读取这里面内容，要让cat退出，必须保证后续的read能够返回0读到EOF。
out:
    return retval;
}
                                                 //包含已有的数据的缓冲区
static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    int numcopied;
    struct mychardev_char_dev *mychardev_dev=file->private_data;
    //获得字符设备的地址  通过文件描述符中file->private_data
    int retval= -ENOMEM;
    int i;
    printk(" write mychardev_dev%p\n",mychardev_dev); 
    kfree(mychardev_dev->c);//清除之前的数据
    mychardev_dev->c=NULL;//为	NOLL
    mychardev_dev->n=0;//字符个数清空
    mychardev_dev->c = kzalloc(count,GFP_KERNEL);//根据新传进来的count 进行分配空间
    printk("addr %p\n",mychardev_dev);
    numcopied = copy_from_user(mychardev_dev->c,buf,count);
                        //保存的地址 新分配的存储空间  用户空间   字节        
    if ( numcopied  ) {     
        retval = -ENOMEM;
       // goto fail_copy;
        //printk("Could't copy %zd bytes from the user\n", numcopied);
    }    
        for(i=0;i<count;i++)//成功打印以下
        printk("Copied %c  from the user", mychardev_dev->c[i]);
    mychardev_dev->n =count;//将成功传入的字符个数保存在设备n当中
    //printk("Data from the user: %s\n", count);
    return count;
//fail_copy:
   // kfree(mychardev_dev->c);//失败了就释放
}
module_init(mycdev_init);
module_exit(mycdev_exit);
//模組聲明
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lewis Chen");
MODULE_DESCRIPTION("Alloc char devices");
