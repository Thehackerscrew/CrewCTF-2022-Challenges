#include "chall.h"

int addData(req* ptr){
    int i = 0;
    item* curr = NULL;
    char* data = NULL;
    for(i=0; i<MAX; i++){
        if(!ptrArr[i])
            break;
    }
    if(i>=MAX){
        Info("No more space left");
        return -ENOMEM;
    }

    curr = kmalloc(sizeof(item), GFP_KERNEL);
    data = kmalloc(DATASIZE, GFP_KERNEL);
    if(!curr || !data){
        Info("kmalloc failed");
        return -ENOMEM;
    }
    memset(data, '\x00', DATASIZE);
    memset(curr, '\x00', sizeof(item));
    curr->data = data;
    // End of the data chunk
    curr->end = curr->data + DATASIZE;
    if(copy_from_user(data, ptr->ptr, DATASIZE)){
        Info("copy_from_user failed");
        return -EAGAIN;
    }

    // Now copy the header
    if(copy_from_user(curr->header, ptr->ptr+DATASIZE, HEADER)){
        Info("copy_from_user failed");
        return -EAGAIN;
    }

    ptrArr[i] = curr;
    // No read yet
    readPos[i] = 0;
    return i;
}

int readData(req* ptr){
    unsigned char size = ptr->size;
    unsigned char newSize = 0;
    unsigned int idx = ptr->idx;
    item* curr = NULL;
    if(!ptrArr[idx]){
        Info("No such item");
        return -ENOENT;
    }
    
    curr = ptrArr[idx];
    newSize = size + readPos[idx];
    // Maximum read
    if(curr->data+size > curr->end || newSize > DATASIZE){
        Info("Read limit exceeded");
        return -EOVERFLOW;
    }

    // Update read position
    readPos[idx]+= size;

    if(copy_to_user(ptr->ptr, curr->data, size)){
        Info("copy_to_user_failed");
        return -EAGAIN;
    }
    // Copy header back to user
    if(copy_to_user(ptr->ptr+DATASIZE, curr->header, HEADER)){
        Info("copy_to_user_failed");
        return -EAGAIN;
    }

    // Update to point to next read position
    curr->data = (char*)curr->data + size;
    return 0; 
}

int delData(unsigned int idx){
    item* curr = NULL;
    if(!ptrArr[idx]){
        Info("No such item");
        return -ENOENT;
    }
    curr = ptrArr[idx];
    if(curr->data-readPos[idx] < curr->end-DATASIZE){
        Info("Invalid pointer");
        return -EAGAIN;
    }
    // Free the original pointer
    kfree(curr->data-readPos[idx]);
    kfree(ptrArr[idx]);
    ptrArr[idx] = NULL;
    readPos[idx] = 0;
    return 0;
}

long int handle_ioctl(struct file* fd, unsigned int type, unsigned long arg){
    req* ptr = NULL;
    int ret = 0;
    mutex_lock(&mutex);
    ptr = kmalloc(sizeof(req), GFP_KERNEL);
    if(!ptr){
        Info("kmalloc failed");
        mutex_unlock(&mutex);
        return -ENOMEM;
    }
    if(copy_from_user(ptr, (const void*)arg, sizeof(req))){
        Info("copy_from_user failed");
        mutex_unlock(&mutex);
        return -EAGAIN;
    }
    switch (type)
    {
    case adddata: ret = addData(ptr);
        break;
    case readdata: ret = readData(ptr);
        break;
    case deldata: ret = delData(ptr->idx);
        break;
    default: Info("Invalid choice");
        mutex_unlock(&mutex);
        return -EINVAL;
    }
    kfree(ptr);
    mutex_unlock(&mutex);
    return ret;
}

static const struct file_operations file_ops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = handle_ioctl,
};

static struct miscdevice device = {
    MISC_DYNAMIC_MINOR, "chall" , &file_ops
};


static int __init init(void)
{
    int ret;
    mutex_init(&mutex);
    ret = misc_register(&device);
    if(ret < 0){
        mutex_destroy(&mutex);
        Info("Failed\n");
    }
    Info("Init\n");
    return 0;
}

static void __exit cleanup(void)
{
    Info("Fini\n");
    misc_deregister(&device);
}

module_init(init);
module_exit(cleanup);
