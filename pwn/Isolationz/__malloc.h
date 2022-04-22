#include "utils.h"

void __init()
{
    top = mmap(NULL, MAX, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    SETSIZE(top, MAX);
    hook = 1;
    head = NULL;
    lastCall = NULL;
    listLength = 0;
}

int isPrevFree()
{
    if (GETCURRBIT(lastCall))
        return 0;
    return 1;
}

// Check if we can handle
void checkSize(int size)
{
    if (top)
    {
        if(GETREALSIZE(top)>MAX){
            err("[!] Something went wrong");
        }
        if (size >= GETREALSIZE(top))
        {
            err("[!] Ran out of memory");
        }
    }
}

char* unLink(char* ptr){
    char* curr = head;

    if((char*)GETBK(GETFD(ptr))!=ptr || (char*)GETFD(GETBK(ptr))!=ptr){
        err("[!] Unlink pointer corruption");
    }

    if(ptr==curr){
        head = (char*)GETFD(curr);
        SETENCBK(GETFD(curr), GETBK(curr));
        SETENCFD(GETBK(curr), GETFD(curr));
        SETFD(curr, NULL);
        SETBK(curr, NULL);
    }
    else{
        curr = (char*)GETFD(curr);
        while(ptr!=curr && curr!=head) curr = (char*)GETFD(curr);
        if(curr!=head){
            SETENCBK(GETFD(curr), GETBK(curr));
            SETENCFD(GETBK(curr), GETFD(curr));
            SETFD(curr, NULL);
            SETBK(curr, NULL);
        }
    }
    listLength--;

    // List is empty
    if(!listLength){
        head = NULL;
    }
    return curr;

    err("[!] Chunk not found in list");
}

void linkChunk(char* ptr){
    if(head){
        SETENCBK(ptr, GETBK(head));
        SETENCFD(ptr, head);
        SETENCFD(GETBK(head), ptr);
        SETENCBK(head, ptr);
        head = ptr;
    }
    else{
        head = ptr;
        SETENCBK(ptr, ptr);
        SETENCFD(ptr, ptr);
    }
    listLength++;
}

void splitChunk(char* ptr, ulong size){
    char* next = ptr + size;
    char* nextNext = NULL;
    SETSIZE(next, (GETREALSIZE(ptr)-size));
    SETPREV(next, size);
    SETSIZE(ptr, (GETSIZE(ptr) - GETREALSIZE(ptr) + size));
    UNSETNEXTBIT(ptr);
    UNSETCURRBIT(next);
    SETPREVBIT(next);
    SETNEXTBIT(next);
    if(ptr!=lastCall){
        nextNext = next + GETREALSIZE(next);
        SETPREV(nextNext, GETREALSIZE(next));
    }
    else{
        lastCall = next;
    }
    linkChunk(next);
}

char* __malloc(int size)
{
    ulong actual = ALIGN(size + HEADER);
    char *chunk = NULL;

    if(hook){
        __init();
        hook = 0;
    }

    // Either from freelist or from top chunk
    if (head)
    {
        char *curr = head;
        ulong idx = 1;
        while (GETREALSIZE(curr) < actual){
            if(idx==listLength){
                break;
            }
            curr = (char*)GETFD(curr);
            idx++;
        }

        // We found a chunk that can fit
        if (GETREALSIZE(curr) >= actual)
        {
            ulong freeChunkSize = GETREALSIZE(curr);
            // No splitting needed
            if(freeChunkSize==actual){
                unLink(curr);
                // Set bits of prev and next chunk to denote inuse
                SETCURRBIT(curr);
                if(curr!=first){
                    SETNEXTBIT((curr - GETPREV(curr)));
                }
                if(curr!=lastCall){
                    SETPREVBIT((curr + GETREALSIZE(curr)));
                }
                return curr + HEADER;
            }
            // Split and put the remaining back in the list
            else{
                unLink(curr);
                splitChunk(curr, actual);
                SETCURRBIT(curr);
                if(curr!=first){
                    SETNEXTBIT((curr - GETPREV(curr)));
                }
                return curr + HEADER;
            }
        }
    }
    // Make sure top chunk has enough space
    checkSize(actual);

    // Use top chunk
    ulong topSize = GETREALSIZE(top);
    chunk = top;
    top += actual;
    SETSIZE(top, topSize - actual);
    SETSIZE(chunk, actual);
    SETNEXTBIT(chunk);
    SETCURRBIT(chunk);
    if(lastCall){
        SETPREV(chunk, GETREALSIZE(lastCall));
        if (!isPrevFree())
        {
            SETPREVBIT(chunk);
        }
    }
    // First ever allocation
    else{
        SETPREV(chunk, 0);
        SETPREVBIT(chunk);
        first = chunk;
    }
    lastCall = chunk;
    return chunk + HEADER;
}

void __free(char *ptr)
{
    char * actual = ptr - HEADER;
    if(!GETCURRBIT(actual)){
        err("[!] Double free");
    }
    UNSETCURRBIT(actual);
    ulong oldSize = GETREALSIZE(actual);
    // Forward and Backward merge if chunks are free
    // Both prev and next are free
    if(!GETNEXTBIT(actual) && !GETPREVBIT(actual)){
        char* next = actual + oldSize;
        char* prev = (actual - GETPREV(actual));
        ulong newSize = GETREALSIZE(next) + GETREALSIZE(prev) + oldSize;
        unLink(next);
        unLink(prev);
        SETSIZE(prev, newSize);
        SETNEXTBIT(prev);
        SETPREVBIT(prev);
        if(next!=lastCall){
            SETPREV((next + GETREALSIZE(next)), newSize);
        }
        SETSIZE(actual, 0);
        SETSIZE(next, 0);
        SETPREV(actual, 0);
        SETPREV(next, 0);
        linkChunk(prev);
        // Chunk before top changes
        if(next==lastCall){
            lastCall = prev;
        }
    }
    // Only next is free
    else if(!GETNEXTBIT(actual)){
        char* next = actual + oldSize;
        ulong newSize = oldSize + GETREALSIZE(next);
        unLink(next);
        SETSIZE(actual, newSize);
        SETNEXTBIT(actual);
        SETPREVBIT(actual);
        if(next!=lastCall){
            SETPREV((next + GETREALSIZE(next)), newSize);
        }
        if(actual!=first){
            UNSETNEXTBIT((actual - GETPREV(actual)));
        }
        SETSIZE(next, 0);
        SETPREV(next, 0);
        linkChunk(actual);
        if(next==lastCall){
            lastCall = actual;
        }
    }
    // Only prev is free
    else if(!GETPREVBIT(actual)){
        char* prev = actual - GETPREV(actual);
        ulong prevSize = GETREALSIZE(prev);
        ulong newRealSize = GETREALSIZE(prev) + oldSize;
        // Bits are preserved
        ulong newSize = GETSIZE(prev) +oldSize;
        unLink(prev);
        SETSIZE(prev, newSize);
        if(actual!=lastCall){
            char* next = actual + oldSize;
            SETPREV(next, newRealSize);
            UNSETPREVBIT(next);
        }
        SETSIZE(actual, 0);
        SETPREV(actual, 0);
        linkChunk(prev);
        if(actual==lastCall){
            lastCall = prev;
        }
    }
    // No merge
    else{
        if(actual!=first){
            UNSETNEXTBIT((actual - GETPREV(actual)));
        }
        if(actual!=lastCall){
            UNSETPREVBIT((actual + oldSize));
        }
        linkChunk(actual);
    }
    return;
}