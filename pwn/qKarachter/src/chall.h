#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/string.h>

MODULE_AUTHOR("");
MODULE_DESCRIPTION("chall");
MODULE_LICENSE("GPL");

static DEFINE_MUTEX(mutex);

#define adddata 0x1337
#define readdata 0x1338
#define deldata 0x1339
#define MAX 0x50
#define DATASIZE 0x80
#define HEADER 0x10

#define Info(...) printk(KERN_INFO __VA_ARGS__);

typedef struct
{
    unsigned char size;
    unsigned int idx;
    char* ptr; // In and Out ptr
}req;

typedef struct{
    char header[16];
    char* end;
    char* data;
}item;

item* ptrArr[MAX];
unsigned char readPos[MAX];
