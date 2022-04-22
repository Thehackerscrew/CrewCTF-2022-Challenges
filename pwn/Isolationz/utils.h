#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>
#include <unistd.h>

typedef unsigned long int ulong;

#define log(...) do {\
    fprintf(stdout, __VA_ARGS__); \
}while(0);

#define err(...) do { \
    puts(__VA_ARGS__); \
    exit(-1); \
}while(0);

#define ANDVAL 0xffffff
#define ALIGNMENT 8
#define HEADER 0x10
#define MAX 0x1000
// All chunks are 8 byte aligned
#define ALIGN(size) (size + (ALIGNMENT - 1)) & ~0x7

// Pointer obfuscation
#define ENC(ptr, addr) (((ulong)addr ^ ((ulong)ptr >> 12)) & ANDVAL)
#define DEC(ptr, bit) (((*((ulong *)ptr+bit) ^ ((ulong)ptr >> 12)) & ANDVAL) | ((ulong)ptr& ~ANDVAL))
#define DECFD(ptr) DEC(ptr, 3)
#define DECBK(ptr) DEC(ptr, 2)

// For setting and getting sizes, addresses and bits
#define SETPREV(ptr, size) *(ulong *)ptr = size
#define GETPREV(ptr) *(ulong *)ptr
#define SETSIZE(ptr, size) *((ulong *)ptr + 1) = size
#define GETSIZE(ptr) *((ulong *)ptr + 1)
#define GETREALSIZE(ptr) (GETSIZE(ptr)&~0x7)
#define SETBIT(ptr, bit) SETSIZE(ptr, GETSIZE(ptr) | (1<<bit))
#define SETPREVBIT(ptr) SETBIT(ptr, 0)
#define SETNEXTBIT(ptr) SETBIT(ptr, 1)
#define SETCURRBIT(ptr) SETBIT(ptr, 2)
#define GETBIT(ptr, bit) ((GETSIZE(ptr) & (1 << bit)) >> bit)
#define GETPREVBIT(ptr) GETBIT(ptr, 0)
#define GETNEXTBIT(ptr) GETBIT(ptr, 1)
#define GETCURRBIT(ptr) GETBIT(ptr, 2)
#define UNSETBIT(ptr, bit) SETSIZE(ptr, GETSIZE(ptr) & ~(1<<bit))
#define UNSETPREVBIT(ptr) UNSETBIT(ptr, 0)
#define UNSETNEXTBIT(ptr) UNSETBIT(ptr, 1)
#define UNSETCURRBIT(ptr) UNSETBIT(ptr, 2)
#define GETBK(ptr) DECBK(ptr)
#define GETFD(ptr) DECFD(ptr)
#define SETBK(ptr, addr) *((ulong **)ptr + 2) = addr
#define SETFD(ptr, addr) *((ulong **)ptr + 3) = addr
#define SETENCBK(ptr, addr) SETBK(ptr, (ulong*)ENC(ptr, addr))
#define SETENCFD(ptr, addr) SETFD(ptr, (ulong*)ENC(ptr, addr))

// Freelist head
char *head;
ulong listLength;
char *top;
// We should not go above this
char *first;
// Chunk just above top
char *lastCall;
int hook = 1;