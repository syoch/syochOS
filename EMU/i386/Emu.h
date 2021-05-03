#ifndef _EMU_
#define _EMU_

#include "mnemonic.h"

char* (*mne[0x100])(char)={0};

void Emu_init(){
    printf("Emu     :Init    :initializing　Mnemonic To Invaid 000%%");
    
    //Mnemonic init
    unsigned short i;
    for (i = 0; i < 0x100; i++)
    {
        printf("\b\b\b\b%3d%%",100*i/0xff);
        mne[i] = mne_invalid;
    }
    putch('\n');

    //x86 CPU
    tol_init();

    //calc
    mne[0x83]=mne_calc_rv_std;
    
    //mov reg,val
    mne[0xb0]=mne_mov_rd_sht;
    mne[0xb1]=mne_mov_rd_sht;
    mne[0xb2]=mne_mov_rd_sht;
    mne[0xb3]=mne_mov_rd_sht;
    mne[0xb4]=mne_mov_rd_sht;
    mne[0xb5]=mne_mov_rd_sht;
    mne[0xb6]=mne_mov_rd_sht;
    mne[0xb7]=mne_mov_rd_sht;
    mne[0xb8]=mne_mov_rd_std;
    mne[0xb9]=mne_mov_rd_std;
    mne[0xba]=mne_mov_rd_std;
    mne[0xbb]=mne_mov_rd_std;
    mne[0xbc]=mne_mov_rd_std;
    mne[0xbd]=mne_mov_rd_std;
    mne[0xbe]=mne_mov_rd_std;
    mne[0xbf]=mne_mov_rd_std;
    //mov reg,[reg]
    mne[0x8a]=mne_mov_rp_sht;

    //jmp
    
    mne[0xeb]=mne_jmp_sht;
    
    //flag conterol
    mne[0xfa]=mne_cli;
    
    //stack
    mne[0x50]=mne_push_reg_std;
    mne[0x51]=mne_push_reg_std;
    mne[0x52]=mne_push_reg_std;
    mne[0x53]=mne_push_reg_std;
    mne[0x54]=mne_push_reg_std;
    mne[0x55]=mne_push_reg_std;
    mne[0x56]=mne_push_reg_std;
    mne[0x57]=mne_push_reg_std;
    mne[0x58]=mne_pop_reg_std;
    mne[0x59]=mne_pop_reg_std;
    mne[0x5a]=mne_pop_reg_std;
    mne[0x5b]=mne_pop_reg_std;
    mne[0x5c]=mne_pop_reg_std;
    mne[0x5d]=mne_pop_reg_std;
    mne[0x5e]=mne_pop_reg_std;
    mne[0x5f]=mne_pop_reg_std;
    
    //Incriment / Decriment
    mne[0x40]=mne_inc;
    mne[0x41]=mne_inc;
    mne[0x42]=mne_inc;
    mne[0x43]=mne_inc;
    mne[0x44]=mne_inc;
    mne[0x45]=mne_inc;
    mne[0x46]=mne_inc;
    mne[0x47]=mne_inc;

    mne[0x48]=mne_dec;
    mne[0x49]=mne_dec;
    mne[0x4a]=mne_dec;
    mne[0x4b]=mne_dec;
    mne[0x4c]=mne_dec;
    mne[0x4d]=mne_dec;
    mne[0x4e]=mne_dec;
    mne[0x4f]=mne_dec;

    //comp
    mne[0x3c]=mne_cmp_rv_ax;
    mne[0x3d]=mne_cmp_rv_ax;

    //jump
    mne[0x70]=mne_jump_if;
    mne[0x71]=mne_jump_if;
    mne[0x72]=mne_jump_if;
    mne[0x73]=mne_jump_if;
    mne[0x74]=mne_jump_if;
    mne[0x75]=mne_jump_if;
    mne[0x76]=mne_jump_if;
    mne[0x77]=mne_jump_if;
    mne[0x78]=mne_jump_if;
    mne[0x79]=mne_jump_if;
    mne[0x7A]=mne_jump_if;
    mne[0x7B]=mne_jump_if;
    mne[0x7C]=mne_jump_if;
    mne[0x7D]=mne_jump_if;
    mne[0x7E]=mne_jump_if;
    mne[0x7F]=mne_jump_if;

    //Float Control
    mne[0xe8]=mne_call_std;
    mne[0xc3]=mne_ret;
    mne[0xcd]=mne_int;
    mne[0xf4]=mne_hlt;
}

#endif