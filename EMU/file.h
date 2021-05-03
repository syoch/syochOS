#ifndef _FILE_
#define _FILE_

#include <tiny_stdlib.h>
#include "mem.h"

FILE *fp;

void file_init(char* filename)
{
    printf("File    :Init    :Opening %s\n",filename);
    fp = fopen(filename, "rb"); //File Init
    fseek(fp,(long)0,SEEK_SET);
    mem_init();
}
void file_end(){
    printf("File    :Finish  :Start\n");
    fclose(fp);
}
char read1byte()
{
    return fgetc(fp); //Read as char
}
int isEOF()
{
    return feof(fp) != 0;
}
void file_goto(unsigned long long int to){
    fseek(fp,to,SEEK_SET);
}
void file_goto_absolute(unsigned long long int to){
    fseek(fp,to,SEEK_CUR);
}
long file_pos(){
    return ftell(fp)-1;
}
void readBootloader(){
    __int64 WritingAddress=0x7C00;
    printf("File    :Load    :Loading BootLoader 000b");
    for(int i=0;i<512;i++){
        mem_set8(WritingAddress,read1byte());
        WritingAddress++;
        printf("\b\b\b\b%3d%%",100*i/511);
    }
    putch('\n');
}
#endif