#ifndef _TEST_CMD_H

#define _TEST_CMD_H
#define TEST_MAGIC 'x' //定义幻数
#define TEST_MAX_NR 3 //定义命令的最大序数
#define TEST_CLEAR _IO(TEST_MAGIC, 1)
#define TEST_OFFSET _IO(TEST_MAGIC, 2)
#define TEST_KBUF _IO(TEST_MAGIC, 3)
#endif /*_TEST_CMD_H*/
