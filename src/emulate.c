#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define zr r[31]
#define Cur(x) ((cur >> (x)) & 1)
#define cur(l, r) ((cur >> (l)) & ((1ULL << ((r) - (l) + 1)) - 1))
#define MEM_SIZE 0x200000  // 2MB of memory
uint8_t memory[MEM_SIZE];

void init_memory() {
    memset(memory, 0, MEM_SIZE);
}

uint32_t load_memory(uint64_t address, int bit) {
    int size = bit / 8;
    if (address + size-1 >= MEM_SIZE) {
        return 0;
    }
    uint32_t value = 0;
    for (int i = 0; i < size; i++) {
        value |= ((uint32_t)memory[address + i]) << (8 * i);
    }
    return value;
}

void store_memory(uint64_t address, uint32_t data, int bit) {
    int size = bit / 8;
    if (address + size-1 >= MEM_SIZE) {
        return;
    }

    for (int i = 0; i < size; i++) {
        memory[address + i] = (data >> (8 * i)) & 0xFF;
    }
}


typedef uint32_t instr;
typedef uint32_t seg;

union reg {
    int64_t x;
    int32_t w;
    uint64_t ux;
    uint32_t uw;
} r[32], pc, sp;

enum {
    N = -1, Z = 0, C = 1, V =2
} pstate = Z;

void Single_data_transfer(instr cur) {
    seg rt = cur(0,4);
    seg xn = cur(5, 9);
    seg sf = Cur(30);
    seg load = Cur(22);
    seg target_address; 
    if(Cur(31) == 0) { // check type of transform
        target_address = pc.x + cur(5,23) * 4;
        load = 1;
    }
    else if(Cur(24)) { // check unsigned
        seg imm12 = cur(10, 21);
        if(Cur(31) == 1) {
            target_address = r[xn].uw + imm12 * 4;
        } 
        else {
            target_address = r[xn].ux + imm12 * 8;
        }
    }
    else if(Cur(21) == 1) { // register offset
        seg xm = cur(16,20);
        if(Cur(31) == 1) {
            target_address = r[xn].uw + xm;
        } 
        else { 
            target_address = r[xn].ux + xm;
        }
    }
    else { //check pre/post
        seg simm9 = cur(12, 20);
        if(Cur(31) == 1) {
            target_address = r[xn].uw + ((Cur(11) == 1) ? simm9 : 0);
            r[xn].uw += simm9;
        } 
        else {
            target_address = r[xn].ux + ((Cur(11) == 1) ? simm9 : 0);
            r[xn].ux += simm9;
        }
    }
    if(load == 1) { // load / store
        if(sf == 0) {
            r[rt].uw = load_memory(target_address, 32);
        }
        else {
            r[rt].ux = load_memory(target_address, 64);
        }
    }
    else {
        if(sf == 0) {
            store_memory(target_address, r[xn].uw, 32);
        }
        else {
            store_memory(target_address, r[xn].ux, 64);
        }
    }
}
int main() {
    return EXIT_SUCCESS;
}