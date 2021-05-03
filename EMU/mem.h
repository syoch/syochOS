#ifndef _MEM_
#define _MEM_

#include <dict.h>

dict *virtualmemory = (dict *)NULL;

#define _mem_internaltool_sChar2Hex(a) ((a) == 0x00 ? '0' : (a) == 0x01 ? '1' : (a) == 0x02 ? '2' : (a) == 0x03 ? '3' : (a) == 0x04 ? '4' : (a) == 0x05 ? '5' : (a) == 0x06 ? '6' : (a) == 0x07 ? '7' : (a) == 0x08 ? '8' : (a) == 0x09 ? '9' : (a) == 0x0A ? 'A' : (a) == 0x0B ? 'B' : (a) == 0x0C ? 'C' : (a) == 0x0D ? 'D' : (a) == 0x0E ? 'E' : (a) == 0x0F ? 'F' : '?')
char *_mem_internaltool_dChar2Hex(char a)
{
    static char returnvalue[2];
    returnvalue[0] = _mem_internaltool_sChar2Hex((a & 0xf0) >> 8);
    returnvalue[1] = _mem_internaltool_sChar2Hex((a & 0x0f) >> 0);
    return returnvalue;
}
unsigned char *mem_tol_Integer2Hex(__int64 adr)
{
    static unsigned char stringaddress[8];
    stringaddress[0] = (adr >> 0x00) & 0xff;
    stringaddress[1] = (adr >> 0x08) & 0xff;
    stringaddress[2] = (adr >> 0x10) & 0xff;
    stringaddress[3] = (adr >> 0x18) & 0xff;
    stringaddress[4] = (adr >> 0x20) & 0xff;
    stringaddress[5] = (adr >> 0x28) & 0xff;
    stringaddress[6] = (adr >> 0x30) & 0xff;
    stringaddress[7] = (adr >> 0x38) & 0xff;

    return stringaddress;
}
void mem_init()
{
    if (virtualmemory == NULL)
    {
        printf("Memory  :Init    :Initalizing\n");
        virtualmemory = Dict_Make();
    }
}

void mem_free()
{
    if (virtualmemory != NULL)
    {
        printf("Memory  :Finish  :Finishing up\n");
        Dict_delete(virtualmemory);
    }
}

void mem_set64(__int64 addr, __int64 val)
{
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 0), (val & 0xff00000000000000) >> 0x0038);
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 1), (val & 0x00ff000000000000) >> 0x0030);
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 2), (val & 0x0000ff0000000000) >> 0x0028);
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 3), (val & 0x000000ff00000000) >> 0x0020);
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 4), (val & 0x00000000ff000000) >> 0x0018);
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 5), (val & 0x0000000000ff0000) >> 0x0010);
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 6), (val & 0x000000000000ff00) >> 0x0008);
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 7), (val & 0x00000000000000ff) >> 0x0000);
}
void mem_set32(__int64 addr, unsigned long int val)
{
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 0), (val & 0xff000000) >> 0x0018);
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 1), (val & 0x00ff0000) >> 0x0010);
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 2), (val & 0x0000ff00) >> 0x0008);
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 3), (val & 0x000000ff) >> 0x0000);
}
void mem_set16(__int64 addr, unsigned short int val)
{
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 0), (val & 0xff00) >> 0x0008);
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr + 1), (val & 0x00ff) >> 0x0000);
}
void mem_set8(__int64 addr, unsigned char val)
{
    Dict_Adddata(virtualmemory, mem_tol_Integer2Hex(addr), val);
}

#define mem_get(a, s) _mem_get(__func__, a, s)
__int64 _mem_get(const char *from, __int64 addr, int size)
{
    __int64 val = 0;
    int i;
    for (i = 0; i < size; i++)
    {
        val |= (__int64)Dict_GetData(virtualmemory, mem_tol_Integer2Hex(addr + i)) & 0x000000ff;
        val <<= 8;
    }
    val >>= 8;
    //if(strcmp(from,"tol_read")&&
    //   strcmp(from,"main")){
    //    printf("Memory  :Read    :%I64x ~ %2db = %I64x By %s\n",addr,size,val,from);
    //}

    return val;
}

void mem_listup()
{
    int i,j;
    for (i = 0; i < virtualmemory->datasize; i++)
    {
        print("Memory  :Listup  :");
        for (j = 0; j < 8; j++){
            printf("%2x",virtualmemory->key[i][j]);
        }
        printf("=%02x", virtualmemory->val[i] & 0x000000ff);
        putch('\n');
    }
}
#endif