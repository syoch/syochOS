#ifndef _MNEMONIC_
#define _MNEMONIC_

#include <tiny_stdlib.h>
#include "tool.h"
#include "../defines.h"

char* mne_invalid(char ch)
{
    static char msg[32];
    strcpy(msg,"Currentry Supported i386 opcode");
    printf("MNEMONIC:ERROR   :%I64x@%2x Currentry Supported i386 opcode\n",tol_GetCodePos(),ch);
    internalflags|=INTERNALFLAG_EXIT_M;
    return msg;
}

char* mne_cli(char ch)
{
    static char msg[32];
    cpu_flags &= ~CPU_FLAG_INTERPLETE;
    sprintf(msg,"cli");
    return msg;
}

char* mne_hlt(char ch){
    static char msg[32];
    internalflags|=INTERNALFLAG_EXIT_M;
    sprintf(msg,"hlt");
    return msg;
}

char* mne_jmp_sht(char ch)
{
    static char msg[32];

    unsigned long long int Goto = tol_read_sht();
    if(Goto<0){
        regs[CS].data-=Goto;
    }else{
        regs[CS].data+=Goto;
    }
    
    sprintf(msg,"jmp 0x%I64x",Goto);
    return msg;
}

char* mne_mov_rd_std(char ch)
{
    static char msg[32];

    regref tar;
    tol_ind2reg_hib((ch & 0x0f)-8, &tar);
    *tar.val = tol_read_std();

    sprintf(msg,"mov %s,0x%I64x", tol_Regref2String(&tar), *tar.val);
    return msg;
}
char* mne_mov_rd_sht(char ch){
    static char msg[32];

    regref tar;
    tol_ind2reg_std((ch & 0x0f), &tar);
    if(tar.mode==REGREFMODE_8h){
        *tar.val &=~ 0xff00;
        *tar.val |= tol_read_sht()<<8;
    }else if(tar.mode==REGREFMODE_8l){
        *tar.val &=~ 0x00ff;
        *tar.val |= tol_read_sht();
    }

    sprintf(msg,"mov %s,0x%I64x", tol_Regref2String(&tar), *tar.val);
    return msg;
}

char* mne_call_std(char ch)
{
    static char msg[32];

    unsigned long long Goto = tol_read_std();
    Goto+=tol_GetCodePos();
    regs[SP].data -= CurrentBit / 8;
    if(CurrentBit==16){
        mem_set16(regs[SP].data,regs[CS].data);
    }else if(CurrentBit==32){
        mem_set32(regs[SP].data,regs[CS].data);
    }else if(CurrentBit==64){
        mem_set64(regs[SP].data,regs[CS].data);
    }
    regs[CS].data = Goto;
    sprintf(msg,"call 0x%I64x", Goto);
    return msg;
}

char* mne_ret(char ch)
{
    static char msg[32];

    regs[CS].data = mem_get(regs[SP].data,CurrentBit / 8);
    regs[SP].data += CurrentBit / 8;
    sprintf(msg,"ret");
    return msg;
}

char* mne_mov_rp_sht(char ch)
{
    static char msg[32];
    modrm modrm;
    unsigned char mdrm = tol_read1byteByCS();

    tol_setModrm(ch, mdrm, &modrm);
    __int64 null;
    //TODO: mov r,*m
    void* dest=   (modrm.r.mode==REGREFMODE_8l) ? (void*)Reg8lp(*modrm.r.val)
                : (modrm.r.mode==REGREFMODE_8h) ? (void*)Reg8hp(*modrm.r.val)
                : (modrm.r.mode==REGREFMODE_16) ? (void*)Reg16p(*modrm.r.val)
                : (modrm.r.mode==REGREFMODE_32) ? (void*)Reg32p(*modrm.r.val)
                : (modrm.r.mode==REGREFMODE_64) ? (void*)modrm.r.val
                : (void*)&null;
    __int64 addr=   (modrm.m.mode==REGREFMODE_8l) ? (__int64)Reg8l(*modrm.m.val)
                  : (modrm.m.mode==REGREFMODE_8h) ? (__int64)Reg8h(*modrm.m.val)
                  : (modrm.m.mode==REGREFMODE_16) ? (__int64)Reg16(*modrm.m.val)
                  : (modrm.m.mode==REGREFMODE_32) ? (__int64)Reg32(*modrm.m.val)
                  : (modrm.m.mode==REGREFMODE_64) ? (__int64)*modrm.m.val
                  : (__int64)0;
    char size=modrm.r.mode==REGREFMODE_8h ? 1
            : modrm.r.mode==REGREFMODE_8l ? 1
            : modrm.r.mode==REGREFMODE_16 ? 2
            : modrm.r.mode==REGREFMODE_32 ? 4
            : modrm.r.mode==REGREFMODE_64 ? 8
            : 0;
    if(size==1){
        *((unsigned char*)dest)=(unsigned char)mem_get(addr,size);
    }else if(size==2){
        *((unsigned short int*)dest)=(unsigned short int)mem_get(addr,size);
    }else if(size==4){
        *((unsigned long int*)dest)=(unsigned long int)mem_get(addr,size);
    }else{
        *((unsigned long long int*)dest)=(unsigned long long int)mem_get(addr,size);
    }
    sprintf(msg,"mov %s,[%s]",
        tol_Regref2String(&modrm.r), tol_Regref2String(&modrm.m)    );
    return msg;
}

char* mne_push_reg_std(char ch)
{
    static char msg[32];
    regref from;

    tol_ind2reg_hib(ch - 0x50, &from);
    regs[SP].data -= CurrentBit / 8;
    if(CurrentBit==16){
        mem_set16(regs[SP].data,Reg16(*from.val));
    }else if(CurrentBit==32){
        mem_set32(regs[SP].data,Reg32(*from.val));
    }else if(CurrentBit==64){
        mem_set16(regs[SP].data,*from.val);
    }

    sprintf(msg,"push %s", from.name);
    return msg;

}
char* mne_pop_reg_std(char ch)
{
    static char msg[32];
    regref from;

    tol_ind2reg_hib(ch - 0x58, &from);
    regs[SP].data += CurrentBit / 8;
    
    if(CurrentBit==16){
        *Reg16p(from.val)=(unsigned short)mem_get(Reg16(regs[SP].data),2);
    }else if(CurrentBit==32){
        *Reg32p(from.val)=(unsigned long)mem_get(Reg32(regs[SP].data),4);
    }else if(CurrentBit==64){
        *from.val=(unsigned long long)mem_get(regs[SP].data,8);
    }

    sprintf(msg,"pop %s", from.name);
    return msg;
}

char* mne_inc(char ch)
{
    static char msg[32];
    regref from;

    tol_ind2reg_hib(ch - 0x40, &from);
    (*from.val)++;
    
    sprintf(msg,"inc %s", from.name);
    return msg;
}

char* mne_dec(char ch)
{
    static char msg[32];
    regref from;

    tol_ind2reg_hib(ch - 0x48, &from);
    (*from.val)--;
    
    sprintf(msg,"dec %s", from.name);
    return msg;
}

char* mne_cmp_rv_ax(char ch)
{
    static char msg[32];
    char comp = tol_read1byteByCS();
    __int64 val;
    if (ch == 0x3c)
        val = Reg8l(regs[AX].data);
    else if (ch == 0x3d)
        val = Reg32(regs[AX].data);
    tol_comp(val,comp);
    if (ch == 0x3c)
        sprintf(msg,"cmp al,0x%x", comp);
    else if (ch == 0x3d)
        sprintf(msg,"cmp eax,0x%x", comp);
    return msg;
}

char* mne_calc_rv_std(char ch){
    static char msg[32];
    modrm modrm;

    tol_setModrm(ch,tol_read1byteByCS(),&modrm);
    if(modrm.addmneno==7){
        tol_comp(*modrm.m.val,modrm.adddat);
    }

    sprintf(msg,"calc:%s %s 0x%I64x",modrm.addmne,modrm.m.name,modrm.adddat);
    return msg;

}

char* mne_jump_if(char ch)
{
    static char msg[32];
    unsigned char type = ch - 0x70;
    //unsigned char goflag=0;
    __int64 Goto = tol_GetCodePos() + tol_read_sht() + CurrentBit / 16;

    char* mnemonic = type==0x0 ? "jo  "
                   : type==0x1 ? "jno "
                   : type==0x2 ? "jb  /jc  /jnae"
                   : type==0x3 ? "jnb /jnc /jae "
                   : type==0x4 ? "jz  /je  " 
                   : type==0x5 ? "jnz /jne "
                   : type==0x6 ? "jna /jbe "
                   : type==0x7 ? "jnbe/ja  "
                   : type==0x8 ? "js  "
                   : type==0x9 ? "jns "
                   : type==0xa ? "jp  /jpe "
                   : type==0xb ? "jnp /jpo "
                   : type==0xc ? "jnge/jl  "
                   : type==0xd ? "jge /jnl "
                   : type==0xe ? "jle /jng "
                   : type==0xf ? "jnle/jg  "
                   : "???"
                   ;
    if(type==0x4){
        if(eflag&1<<FLAG_ZF){
            regs[CS].data=Goto;
            eflag&=~1<<FLAG_ZF;
        }
    }

    sprintf(msg,"%s 0x%I64x",mnemonic, Goto);
    return msg;
}
void _mne_int10(char* dest){
    unsigned char id=Reg8h(regs[AX].data);
    if(id==0x0E){
        //Color -> bl
        putchar(Reg8l(regs[AX].data));
        strcpy(dest,"");
    }else{
        strcpy(dest,"Not Current Supported Bios Call id");
        printf("MNEMONIC:ERROR   :%I64x@%2x Currentry Supported Bios Call id\n",tol_GetCodePos(),id);
        internalflags|=INTERNALFLAG_EXIT_M;
    }
    return;
}
void _mne_int13(char* dest){
    unsigned char id=Reg8h(regs[AX].data);
    if(id==0x00){
        eflag&=~FLAG_CF;
        strcpy(dest,"");
    }else{
        strcpy(dest,"Not Current Supported Bios Call id");
        printf("MNEMONIC:ERROR   :%I64x@%2x Currentry Supported Bios Call id\n",tol_GetCodePos(),id);
        internalflags|=INTERNALFLAG_EXIT_M;
    }
    return;
}

void _mne_int(char* dest,unsigned char code){
    if(code==0x10){
        _mne_int10(dest);
    }else if(code==0x13){
        _mne_int13(dest);
    }else{
        strcpy(dest,"Not Current Supported Bios Call");
        printf("MNEMONIC:ERROR   :%I64x@%2x Currentry Supported Bios Call\n",tol_GetCodePos(),code);
        internalflags|=INTERNALFLAG_EXIT_M;
    }
}
char* mne_int(char ch){
    static char msg[32];

    unsigned char code=tol_read1byteByCS();
    sprintf(msg,"int 0x%2x        ",code);
    _mne_int(msg+9,code);
    return msg;
}
#endif