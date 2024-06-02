#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define zr r[31]
#define Cur(x) ((cur >> (x)) & 1)
#define cur(l, r) ((cur >> (l)) & ((1ULL << ((r) - (l) + 1)) - 1))
#define MEM_SIZE 0x200000 // 2MB of memory
uint8_t memory[MEM_SIZE];

void init_memory()
{
    memset(memory, 0, MEM_SIZE);
}

uint64_t load_memory_64(uint64_t address)
{
    if (address + 7 >= MEM_SIZE)
    {
        return 0;
    }
    uint64_t value = 0;
    for (int i = 0; i < 8; i++)
    {
        value |= ((uint64_t)memory[address + i]) << (8 * i);
    }
    return value;
}
void store_memory_64(uint64_t address, uint64_t data)
{
    if (address + 7 >= MEM_SIZE)
    {
        return;
    }
    for (int i = 0; i < 8; i++)
    {
        memory[address + i] = (data >> (8 * i)) & 0xFF;
    }
}
uint32_t load_memory_32(uint64_t address)
{
    if (address + 3 >= MEM_SIZE)
    {
        return 0;
    }
    uint32_t value = 0;
    for (int i = 0; i < 4; i++)
    {
        value |= ((uint32_t)memory[address + i]) << (8 * i);
    }
    return value;
}

void store_memory_32(uint64_t address, uint32_t data)
{
    if (address + 3 >= MEM_SIZE)
    {
        return;
    }
    for (int i = 0; i < 4; i++)
    {
        memory[address + i] = (data >> (8 * i)) & 0xFF;
    }
}

typedef uint32_t instr;
typedef uint32_t seg;

union reg
{
    int64_t x;
    int32_t w;
    uint64_t ux;
    uint32_t uw;
} r[32], pc, sp;

struct
{
    bool n, z, c, v;
} pstate = {0, 1, 0, 0};

void Single_data_transfer(instr cur)
{
    seg rt = cur(0, 4);
    seg xn = cur(5, 9);
    seg sf = Cur(30);
    seg load = Cur(22);
    seg target_address;
    if (Cur(31) == 0)
    { // check type of transform
        target_address = pc.x + cur(5, 23) * 4;
        load = 1;
    }
    else if (Cur(24))
    { // check unsigned
        seg imm12 = cur(10, 21);
        if (Cur(31) == 1)
        {
            target_address = r[xn].uw + imm12 * 4;
        }
        else
        {
            target_address = r[xn].ux + imm12 * 8;
        }
    }
    else if (Cur(21) == 1)
    { // register offset
        seg xm = cur(16, 20);
        if (Cur(31) == 1)
        {
            target_address = r[xn].uw + xm;
        }
        else
        {
            target_address = r[xn].ux + xm;
        }
    }
    else
    { // check pre/post
        seg simm9 = cur(12, 20);
        if (Cur(31) == 1)
        {
            target_address = r[xn].uw + ((Cur(11) == 1) ? simm9 : 0);
            r[xn].uw += simm9;
        }
        else
        {
            target_address = r[xn].ux + ((Cur(11) == 1) ? simm9 : 0);
            r[xn].ux += simm9;
        }
    }
    if (load == 1)
    { // load / store
        if (sf == 0)
        {
            r[rt].uw = load_memory_32(target_address);
        }
        else
        {
            r[rt].ux = load_memory_64(target_address);
        }
    }
    else
    {
        if (sf == 0)
        {
            store_memory_32(target_address, r[xn].uw);
        }
        else
        {
            store_memory_64(target_address, r[xn].ux);
        }
    }
}

void branch(instr cur)
{
    seg kind = cur(21, 30);
    if (kind == 0)
    { // unconditional
        seg simm26 = cur(0, 25);
        int64_t offset = ((int64_t)simm26 << 38) >> 38;
        offset *= 4;
        pc.x += offset;
    }
    else if (kind == 1)
    { // conditional
        seg simm19 = cur(5, 23);
        int64_t offset = ((int64_t)simm19 << 38) >> 38;
        offset *= 4;
        seg cond = cur(0, 3);
        bool check = false;
        switch (cond)
        {
        case 0:
            check = pstate.z == 1;
            break;
        case 1:
            check = pstate.z == 0;
            break;
        case 10:
            check = pstate.n == pstate.v;
            break;
        case 11:
            check = pstate.n != pstate.v;
            break;
        case 12:
            check = pstate.z == 0 && pstate.n == pstate.v;
            break;
        case 13:
            check = !(pstate.z == 0 && pstate.n == pstate.v);
            break;
        case 14:
            check = true;
            break;
        }
        if(check) {
            pc.x += offset;
        }
    }
    else if (kind == 3) // register
    { 
        seg xn = cur(5,9);
        pc = r[xn];
    }
}

int main()
{
    return EXIT_SUCCESS;
}