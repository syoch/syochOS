#ifndef _TOOL_H_
#define _TOOL_H_

#include "../file.h"
#include "../defines.h"
#include "../mem.h"

char CurrentBit = 16;
char cpu_flags = 0b00000001;
int eflag = 0;

reg regs[8 + 6];

#define AX 0
#define BX 1
#define CX 2
#define DX 3
#define SI 4
#define DI 5
#define BP 6
#define SP 7
//Segment Register index
#define CS 8

#define FLAG_CF 0
#define FLAG_EF 1
#define FLAG_PF 2
#define FLAG_AF 4
#define FLAG_ZF 6
#define FLAG_SF 7
#define FLAG_TF 8
#define FLAG_IF 9
#define FLAG_DF 10
#define FLAG_OF 11
#define FLAG_IOPL_H 12
#define FLAG_IOPL_L 13
#define FLAG_NT 14
#define FLAG_RF 16
#define FLAG_VM 17
#define FLAG_AC 18
#define FLAG_VIF 19
#define FLAG_VIP 20
#define FLAG_ID 21

void tol_init()
{
    printf("CPU Tool:Init    :Initalizing Intel x86 CPU\n");
    char *str[8+6] = {"ax", "cx", "dx", "bx",
                    "si", "di", "bp", "sp",
                    "ss", "cs", "ds", "es",
                    "fs", "gs"};
    for (int i = 0; i < 8; i++)
    {
        regs[i].name.val64[0] = 'r';
        regs[i].name.val64[1] = str[i][0];
        regs[i].name.val64[2] = str[i][1];
        regs[i].name.val64[3] = '\0';

        regs[i].name.val32[0] = 'e';
        regs[i].name.val32[1] = str[i][0];
        regs[i].name.val32[2] = str[i][1];
        regs[i].name.val32[3] = '\0';

        regs[i].name.val16[0] = str[i][0];
        regs[i].name.val16[1] = str[i][1];
        regs[i].name.val16[2] = '\0';

        regs[i].name.val8h[0] = str[i][0];
        regs[i].name.val8h[1] = 'h';
        regs[i].name.val8h[2] = '\0';

        regs[i].name.val8l[0] = str[i][0];
        regs[i].name.val8l[1] = 'l';
        regs[i].name.val8l[2] = '\0';
    }
    regs[SP].data = -1;
    
    mem_init();
}

void tol_ind2reg_std(unsigned char ind, regref *dest)
{
    if (ind >> 2 == 1)
    {
        dest->mode=REGREFMODE_8h;
        dest->name = regs[(ind)&3].name.val8h;
    }
    else
    {
        dest->mode=REGREFMODE_8l;
        dest->name = regs[(ind)&3].name.val8l;
    }
    dest->val = &regs[(ind)&3].data;
}
void tol_ind2reg_adv(unsigned char ind, regref *dest)
{
    if (ind < 4)
    {
        dest->mode=REGREFMODE_UN;
        dest->name = "???";
    }
    else
    {
        dest->mode=REGREFMODE_16;
        dest->name = regs[(ind)].name.val16;
    }
    dest->val = &regs[(ind)].data;
}
void tol_ind2reg_hib(unsigned char ind, regref *dest)
{
    if (ind > 3)
    {
        if (ind <= 5)
        {
            ind += 2;
        }
        else if (ind <= 7)
        {
            ind -= 2;
        }
    }
    dest->name = regs[(ind)].name.val16;
    dest->mode=REGREFMODE_16;
    dest->val = &regs[(ind)].data;
}

__int64 tol_GetCodePos(){
    return regs[CS].data;
}

#define tol_read1byteByCS() __tol_read1byteByCS(__func__)
char __tol_read1byteByCS(const char* from){
    char ch;
    ch=(char)_mem_get(from,regs[CS].data,1);
    regs[CS].data++;
    return ch;
}

void tol_setModrm(unsigned char op, unsigned char mdrm, modrm *modrm)
{
    unsigned char mod = (mdrm & 0b11000000) >> 6;
    unsigned char reg = (mdrm & 0b00111000) >> 3;
    unsigned char mem = (mdrm & 0b00000111) >> 0;
    unsigned char ignoreadddat = FALSE;
    unsigned char ReadSize = 0;
    modrm->mod = mod;

    modrm->adddat = 0;
    modrm->addmne = "???";

    if ((op & 0xfc) == 0x80)
    {
        unsigned char regindex;
        modrm->addmne =
            reg == 0b000 ? (char *)"add"
          : reg == 0b001 ? (char *)"or"
          : reg == 0b010 ? (char *)"adc"
          : reg == 0b011 ? (char *)"sbb"
          : reg == 0b100 ? (char *)"and"
          : reg == 0b101 ? (char *)"sub"
          : reg == 0b110 ? (char *)"xor"
          : reg == 0b111 ? (char *)"cmp"
          : (char *)"???";
        modrm->addmneno =
            reg == 0b000 ? 0
          : reg == 0b001 ? 1
          : reg == 0b010 ? 2
          : reg == 0b011 ? 3
          : reg == 0b100 ? 4
          : reg == 0b101 ? 5
          : reg == 0b110 ? 6
          : reg == 0b111 ? 7
          : 8;
        regindex =
            mem == 0b000 ? 0b000
          : mem == 0b001 ? 0b001
          : mem == 0b010 ? 0b010
          : mem == 0b011 ? 0b011
          : mem == 0b100 ? 0b110
          : mem == 0b101 ? 0b111
          : mem == 0b110 ? 0b100
          : mem == 0b111 ? 0b101
          : 0b000;
        tol_ind2reg_hib(regindex, &modrm->m);
    }
    else if (op == 0x8a)
    {
        tol_ind2reg_std(reg, &modrm->r);
        tol_ind2reg_adv(mem, &modrm->m);
    }
    else
    {
        tol_ind2reg_std(reg, &modrm->r);
        tol_ind2reg_std(mem, &modrm->m);
    }
    //Setting ReadSize
    ReadSize = op == 0x83 ? CurrentBit / 16
                          : 0;
    //Processing 'adddat' field
    if (ignoreadddat == FALSE)
    {
        if (mod == 1)
        {
            DataAdd(modrm->adddat,0) = tol_read1byteByCS();
        }
        else if (mod == 2)
        {
            DataAdd(modrm->adddat,1) = tol_read1byteByCS();
            DataAdd(modrm->adddat,0) = tol_read1byteByCS();
        }
    }
    if (ReadSize != 0)
    {
        switch (ReadSize)
        {
        case 8:
            DataAdd(modrm->adddat,7) = tol_read1byteByCS();
        case 7:
            DataAdd(modrm->adddat,6) = tol_read1byteByCS();
        case 6:
            DataAdd(modrm->adddat,5) = tol_read1byteByCS();
        case 5:
            DataAdd(modrm->adddat,4) = tol_read1byteByCS();
        case 4:
            DataAdd(modrm->adddat,3) = tol_read1byteByCS();
        case 3:
            DataAdd(modrm->adddat,2) = tol_read1byteByCS();
        case 2:
            DataAdd(modrm->adddat,1) = tol_read1byteByCS();
        case 1:
            DataAdd(modrm->adddat,0) = tol_read1byteByCS();
        }
    }
}
__int64 tol_read(int d)
{
    __int64 data=0;
    int bit = CurrentBit / d;
    if (bit == 8)
    {
        data=(__int64)tol_read1byteByCS();
    }
    else if (bit == 16)
    {
        data|=((unsigned char)tol_read1byteByCS());
        data|=((unsigned char)tol_read1byteByCS())<<8;
    }
    else if (bit == 32)
    {
        data=-1;
    }
    else if (bit == 64)
    {
        data=-1;
    }
    return data;
}

//ShotCuts for tol_read
#define tol_read_std() tol_read(1)
#define tol_read_sht() tol_read(2)

void tol_read_reg(reg *dest)
{
    dest->data = tol_read_std();
}

__int64 tol_reg2int(reg *from)
{
    __int64 val = 0;
    if (CurrentBit == 16)
        val = Reg16(from->data);
    if (CurrentBit == 32)
        val = Reg32(from->data);
    if (CurrentBit == 64)
        val = from->data;
    return val;
}

char *tol_Register2String(reg *form)
{
    if (CurrentBit == 16)
    {
        return form->name.val16;
    }
    else if (CurrentBit == 32)
    {
        return form->name.val32;
    }
    else if (CurrentBit == 64)
    {
        return form->name.val64;
    }
    else
    {
        return "T.T";
    }
}

char *tol_Regref2String(regref *form)
{
    return form->name;
}

void tol_comp(__int64 val, __int64 comp)
{
    if (val == 0)
    {
        eflag |= 1 << FLAG_ZF;
    }
    else
    {
        eflag &= ~(1 << FLAG_ZF);
    }
    if (val == comp)
    {
        eflag |= 1 << FLAG_ZF;
    }
    else
    {
        eflag &= ~(1 << FLAG_ZF);
    }
    if (val < 0)
    {
        eflag |= 1 << FLAG_SF;
    }
    else
    {
        eflag &= ~(1 << FLAG_SF);
    }
}
#endif