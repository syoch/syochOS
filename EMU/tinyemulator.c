//#define DEBUG 0

#include <tiny_stdlib.h>
#include "utils.h"
#include "i386/Emu.h"
#include "file.h"

int main(int argc,char **args)
{
    __int64 addr;
    char* msg;
    unsigned char opcode = 0;
    
    //Init
    file_init(args[1]);
    Emu_init();
    readBootloader();
    regs[CS].data=0x00007C00;
    printf("-----start of Program Output-----\n");
    while (1){
        addr=regs[CS].data;
        opcode=tol_read1byteByCS();
        msg=mne[opcode](opcode);

        #ifdef DEBUG //if debugmode => Call Data
            printf("Main    :Called  :%2x%2x:%08x %08x %02x %s\n",
                internalflags,
                cpu_flags,
                (unsigned long int)regs[SP].data&0xffffffff,
                (unsigned long int)addr&0xffffffff,
                opcode,
                msg
            );
        #endif

        if(internalflags!=0){
            if(internalflags&INTERNALFLAG_EXIT_M){
                printf("-----end of Program Output-----\n");
                printf("Main    :Exit    :Exit Flag Seted By Mnemonic\n");
                break;
            }
        }
        //Sleep(1000/10);
    }
    end();
    printf("end of Program\n");
    return 0;
}
