 Linux-Character-device.
linux字符设备学习code- 一劳永逸 
code.c 是Character device代码 
通过file_operations工具集进行read和write，还可以通过ioctl以值传递和指针传递的形式传参
操作介绍
首先下载到同一目录下 
打开终端 在此目录下make一键编译
sudo insmod code.ko（加载内核模块）
cat /dev/mychardev（不用另加权限直接查看mychardev设备节点缓存区的数据）
echo 12345 > /dev/mychardev （write数据）
cat /dev/mychardev（不用另加权限再次查看成功写入）
dmesg（查看内核log）
sudo rrmod code（卸载模块）
ioctl操作：
./test_ioctl3 4 4
 dmesg
 ./test_ioctl3 2
 ./test_ioctl3 3 3
 ./test_ioctl3 2
 ./test_ioctl3 1
