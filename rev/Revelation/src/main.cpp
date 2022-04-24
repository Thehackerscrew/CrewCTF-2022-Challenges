#include "stdafx.h"

//#define DEBUG
//#define DEBUG_INFO
#define FUN_SIZE 16 
#define FUN_KEY 0x66
#define KEY_LEN 48
#include <cstdio>
#include <cstring>



#define S_SIZE 20
#define D_SIZE 100


enum OPCODE { END = 0x66, ADD, SUB, MUL, DIV, INC, DEC, XOR, AND, PUSH, PUSHD, POP, MOV, MOVD, MOV2D, LOOP, CMP, JL, JG, JZ, INCD, DECD, FUN };

struct REG {
	unsigned int r0;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
	unsigned int cf;   //Con flag
	unsigned char* db; //DD
	unsigned int* bp;  //BP
	unsigned int* sp;  //SP
	unsigned char* ip; //IP
};

class VM {
private:
	struct REG r;
	virtual unsigned int vmp_getd();
	virtual unsigned int vmp_gets();
	virtual void vmp_setd(unsigned int x);
	virtual void vmc_nop();
	virtual void vmc_add();
	virtual void vmc_sub();
	virtual void vmc_mul();
	virtual void vmc_div();
	virtual void vmc_inc();
	virtual void vmc_dec();
	virtual void vmc_xor();
	virtual void vmc_and();
	virtual void vmc_push();
	virtual void vmc_pushd();
	virtual void vmc_pop();
	virtual void vmc_mov();
	virtual void vmc_movd();
	virtual void vmc_mov2d();
	virtual void vmc_loop();
	virtual void vmc_cmp();
	virtual void vmc_jl();
	virtual void vmc_jg();
	virtual void vmc_jz();
	virtual void vmc_incd();
	virtual void vmc_decd();
	virtual void vmc_fun();
public:
#ifdef DEBUG
	virtual void vm_debug();
#endif
	virtual int vm_init(REG* init);
	virtual void vm_out(REG* out) const;
	virtual void vm_run();
};

#ifdef DEBUG
void VM::vm_debug() {
#ifdef DEBUG_INFO
	//__asm { int 3 }
	static int step = 0;
	
	step++;
#endif
}
#endif

unsigned int VM::vmp_getd() {
	unsigned char op = *(r.ip + 1);
	op = op >> 4;
	switch (op) {
	case 0x00:
		return r.r0;
	case 0x01:
		return r.r1;
	case 0x02:
		return r.r2;
	case 0x03:
		return r.r3;
	case 0x04:
		return r.cf;
	default:
		return 0;
	}
}

unsigned int VM::vmp_gets() {
	unsigned char op = *(r.ip + 1);
	op = op & 0x0f;
	switch (op) {
	case 0x00:
		return r.r0;
	case 0x01:
		return r.r1;
	case 0x02:
		return r.r2;
	case 0x03:
		return r.r3;
	case 0x04:
		return r.cf;
	default:
		return 0;
	}
}

void VM::vmp_setd(unsigned int x) {
	unsigned char op = *(r.ip + 1);
	op = op >> 4;
	switch (op) {
	case 0x00:
		r.r0 = x;
		break;
	case 0x01:
		r.r1 = x;
		break;
	case 0x02:
		r.r2 = x;
		break;
	case 0x03:
		r.r3 = x;
		break;
		//case 0x04:
		//r.db = (unsigned char*)x;
	}
}

int VM::vm_init(REG* init) {
#ifdef DEBUG
	
#endif
	r.r0 = init->r0;
	r.r1 = init->r1;
	r.r2 = init->r2;
	r.r3 = init->r3;
	r.cf = 0;
	r.db = init->db;
	r.bp = init->bp;
	r.sp = init->bp + (S_SIZE / 2);
	r.ip = init->ip;
	return 0;
}

void VM::vm_out(REG* out) const {
#ifdef DEBUG
	
#endif
	out->r0 = r.r0;
	out->r1 = r.r1;
	out->r2 = r.r2;
	out->r3 = r.r3;
	out->cf = r.cf;
	/*  out->db = r.db;
	out->bp = r.bp;
	out->sp = r.sp;
	out->ip = r.ip;*/
}

void VM::vmc_nop() {
	r.ip += 1;
#ifdef DEBUG
	
#endif
}

void VM::vmc_add() {
	vmp_setd(vmp_getd() + vmp_gets());
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_sub() {
	vmp_setd(vmp_getd() - vmp_gets());
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_mul() {
	vmp_setd(vmp_getd() * vmp_gets());
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_div() {
	vmp_setd(vmp_getd() / vmp_gets());
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_inc() {
	vmp_setd(vmp_getd() + 1);
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_dec() {
	vmp_setd(vmp_getd() - 1);
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_xor() {
	vmp_setd(vmp_getd() ^ vmp_gets());
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_and() {
	vmp_setd(vmp_getd() & vmp_gets());
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_push() {
	unsigned int buf = vmp_getd();
	r.sp -= 1;
	*(r.sp) = buf;
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_pushd() {
	unsigned int buf = 0;
	buf += (unsigned int)(*(r.ip + 1)) << 24;
	buf += (unsigned int)(*(r.ip + 2)) << 16;
	buf += (unsigned int)(*(r.ip + 3)) << 8;
	buf += (unsigned int)(*(r.ip + 4));
	r.sp -= 1;
	*(r.sp) = buf;
	r.ip += 5;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_pop() {
	vmp_setd(*(r.sp));
	r.sp += 1;
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_mov() {
	vmp_setd(vmp_gets());
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_movd() {
	char buf;
	buf = *(r.db);
	vmp_setd((unsigned int)buf);
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_mov2d() {
	unsigned char buf = (unsigned char)vmp_getd();
	*(r.db) = buf;
	r.ip += 2;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_loop() {
	unsigned int i = (unsigned int)(*(r.ip + 1));
	if (r.r3 != 0) {
		r.r3--;
		r.ip -= i;
	}
	else {
		r.ip += 2;
	}
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_cmp() {
	if (vmp_getd() == vmp_gets())
		r.cf = 0;
	if (vmp_getd() < vmp_gets())
		r.cf = -1;
	if (vmp_getd() > vmp_gets())
		r.cf = 1;
	r.ip += 2;
#ifdef DEBUG

		
	vm_debug();
#endif
}

void VM::vmc_jl() {
	unsigned int i;
	if (r.cf == -1) {
		i = (unsigned int)(*(r.ip + 1)) + 2;
		r.ip += i;
	}
	else {
		r.ip += 2;
	}
#ifdef DEBUG

	vm_debug();
#endif 
}

void VM::vmc_jg() {
	unsigned int i;
	if (r.cf == 1) {
		i = (unsigned int)(*(r.ip + 1)) + 2;
		r.ip += i;
	}
	else {
		r.ip += 2;
	}
#ifdef DEBUG

	vm_debug();
#endif
}

void VM::vmc_jz() {
	unsigned int i;
	if (r.cf == 0) {
		i = (unsigned int)(*(r.ip + 1)) + 2;
		r.ip += i;
	}
	else {
		r.ip += 2;
	}
#ifdef DEBUG

	vm_debug();
#endif  
}

void VM::vmc_incd() {
	r.db++;
	r.ip++;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vmc_decd() {
	r.db--;
	r.ip++;
#ifdef DEBUG

	vm_debug();
#endif
}

void VM::vmc_fun() {
	int i;
	for (i = 1; i < FUN_SIZE; ++i) {
		*(r.ip + i) ^= FUN_KEY;
	}
	r.ip += FUN_SIZE;
#ifdef DEBUG
	
	vm_debug();
#endif
}

void VM::vm_run() {
#ifdef DEBUG
	
	vm_debug();
#endif
	while (1) {
		unsigned char* addr = r.ip;

#ifdef DEBUG
		
#endif 
		switch (*addr) {
		case END:
			goto end;
		case ADD:
			vmc_add();
			break;
		case SUB:
			vmc_sub();
			break;
		case MUL:
			vmc_mul();
			break;
		case DIV:
			vmc_div();
			break;
		case INC:
			vmc_inc();
			break;
		case DEC:
			vmc_dec();
			break;
		case XOR:
			vmc_xor();
			break;
		case AND:
			vmc_and();
			break;
		case PUSH:
			vmc_push();
			break;
		case PUSHD:
			vmc_pushd();
			break;
		case POP:
			vmc_pop();
			break;
		case MOV:
			vmc_mov();
			break;
		case MOVD:
			vmc_movd();
			break;
		case MOV2D:
			vmc_mov2d();
			break;
		case LOOP:
			vmc_loop();
			break;
		case CMP:
			vmc_cmp();
			break;
		case JL:
			vmc_jl();
			break;
		case JG:
			vmc_jg();
			break;
		case JZ:
			vmc_jz();
			break;
		case INCD:
			vmc_incd();
			break;
		case DECD:
			vmc_decd();
			break;
		case FUN:
			vmc_fun();
			break;
		default:
			vmc_nop();
		}
	}
#ifdef DEBUG
	
#endif 
end:
	return;
}








unsigned char VM_CODE[] =
{
	0xDE, 0xAD, 0xC0, 0xDE,   //magic code
	FUN, 0x2E, 0x27, 0x25, 0x2D, 0x39, 0x32, 0x2E, 0x23, 0x39, 0x36, 0x2a, 0x27, 0x28, 0x23, 0x32,
	PUSHD,  0x00, 0x00, 0x00, 0x2f,   //key len
	LOOP,   0x05,
	POP,    0x30,

	MOVD,   0x00,   //loop begin
	XOR,    0x22,
	CMP,    0x02,
	JZ,     0x33,
	INCD,
	PUSHD,  0x00, 0x00, 0x00, 0x46,
	POP,    0x10,
	CMP,    0x01,
	JG,     0x27,   //error             x<0x46
	PUSHD,  0x00, 0x00, 0x00, 0x30,
	POP,    0x10,
	CMP,    0x01,
	JL,     0x16,   //error             x>0x30
	PUSHD,  0x00, 0x00, 0x00, 0x39,
	POP,    0x10,
	CMP,    0x01,
	JL,     0x0b,   //loop              x<0x39
	PUSHD,  0x00, 0x00, 0x00, 0x41,
	POP,    0x01,
	CMP,    0x01,
	JL,     0x06,   //error             x<0x41
	XOR,    0x00,   //jmp to loop
	CMP,    0x00,
	JZ,     0x05,
	XOR,    0x00,   //error
	INC,    0x00,
	END,
	LOOP,   0x40,   //loop end

	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x30,
	XOR,    0x11,

	DECD,
	MOVD,   0x00,
	PUSHD,  0x00, 0x00, 0x00, 0x30,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x0a,
	POP,    0x20,
	CMP,    0x02,
	JL,     0x09,
	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x10,
	POP,    0x20,
	MUL,    0x12,
	ADD,    0x10,
	LOOP,   0x2b,

	PUSHD,  0xf3, 0x37, 0x46, 0xe6,   //6e64733f
	POP,    0x20,
	CMP,    0x12,
	XOR,    0x00,
	JZ,     0x03,
	INC,    0x00,
	END,

	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x30,
	XOR,    0x11,

	DECD,
	MOVD,   0x00,
	PUSHD,  0x00, 0x00, 0x00, 0x30,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x0a,
	POP,    0x20,
	CMP,    0x02,
	JL,     0x09,
	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x10,
	POP,    0x20,
	MUL,    0x12,
	ADD,    0x10,
	LOOP,   0x2b,

	PUSHD,  0x54, 0x96, 0x27, 0x66,   //66726945
	POP,    0x20,
	CMP,    0x12,
	XOR,    0x00,
	JZ,     0x03,
	INC,    0x00,
	END,

	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x30,
	XOR,    0x11,

	DECD,
	MOVD,   0x00,
	PUSHD,  0x00, 0x00, 0x00, 0x30,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x0a,
	POP,    0x20,
	CMP,    0x02,
	JL,     0x09,
	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x10,
	POP,    0x20,
	MUL,    0x12,
	ADD,    0x10,
	LOOP,   0x2b,

	DEC,    0x10,
	PUSHD,  0x02, 0x54, 0x26, 0x01,   //20624520
	POP,    0x20,
	CMP,    0x12,
	XOR,    0x00,
	JZ,     0x03,
	INC,    0x00,
	END,

	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x30,
	XOR,    0x11,

	DECD,
	MOVD,   0x00,
	PUSHD,  0x00, 0x00, 0x00, 0x30,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x0a,
	POP,    0x20,
	CMP,    0x02,
	JL,     0x09,
	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x10,
	POP,    0x20,
	MUL,    0x12,
	ADD,    0x10,
	LOOP,   0x2b,

	INC,    0x10,
	PUSHD,  0x54, 0x77, 0x02, 0xe7,   //6e207745
	POP,    0x20,
	CMP,    0x12,
	XOR,    0x00,
	JZ,     0x03,
	INC,    0x00,
	END,

	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x30,
	XOR,    0x11,

	DECD,
	MOVD,   0x00,
	PUSHD,  0x00, 0x00, 0x00, 0x30,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x0a,
	POP,    0x20,
	CMP,    0x02,
	JL,     0x09,
	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x10,
	POP,    0x20,
	MUL,    0x12,
	ADD,    0x10,
	LOOP,   0x2b,

	PUSHD,  0x16, 0x36, 0xc2, 0xf6,   //6f2c6361
	POP,    0x20,
	CMP,    0x12,
	XOR,    0x00,
	JZ,     0x03,
	INC,    0x00,
	END,

	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x30,
	XOR,    0x11,

	DECD,
	MOVD,   0x00,
	PUSHD,  0x00, 0x00, 0x00, 0x30,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x0a,
	POP,    0x20,
	CMP,    0x02,
	JL,     0x09,
	PUSHD,  0x00, 0x00, 0x00, 0x07,
	POP,    0x20,
	SUB,    0x02,
	PUSHD,  0x00, 0x00, 0x00, 0x10,
	POP,    0x20,
	MUL,    0x12,
	ADD,    0x10,
	LOOP,   0x2b,

	PUSHD,  0x16, 0x86, 0x57, 0x47,   //74756861
	POP,    0x20,
	CMP,    0x12,
	XOR,    0x00,
	JZ,     0x02,
	INC,    0x00,
	END,
};

int main(int argc, char* argv[]) {

	printf("Don't fear the Darkness welcome its embrace\n");
	if (argc < 2) {
		printf("Use: ./revlation [key]\n");
		return 0;
	}
	if (strlen(argv[1]) > KEY_LEN) {
		return 0;
	}
	VM* vm = new VM;
	REG* x = new REG;
	x->r1 = 0;
	x->r2 = 0;
	x->r3 = 0;
	unsigned char* y = new unsigned char[D_SIZE];
	unsigned int* s = new unsigned int[S_SIZE];

	memcpy(y, argv[1], 50);
	x->db = y;
	x->bp = s;
	x->ip = VM_CODE;
	vm->vm_init(x);
	vm->vm_run();
	vm->vm_out(x);
	if (x->r0 == 0) {
	
		printf("Here's your The Pieces of Eden crew{%s}\n",argv[1]);
	}
	delete[] s;
	delete[] y;
	delete x;
	delete vm;
	return 0;
}