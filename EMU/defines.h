#ifndef _CONSTANTS_
#define _CONSTANTS_

//Contants
//Basic
#define TRUE 1
#define FALSE 0

//Flags
#define CPU_FLAG_INTERPLETE 1
#define INTERNALFLAG_EXIT_U ((char)0b00000001) //Exit By Unknown
#define INTERNALFLAG_EXIT_M ((char)0b00000010) //Exit By mnemonic error
//Modes 
#define REGREFMODE_8h ((char)0)
#define REGREFMODE_8l ((char)1)
#define REGREFMODE_16 ((char)2)
#define REGREFMODE_32 ((char)3)
#define REGREFMODE_64 ((char)4)
#define REGREFMODE_UN ((char)5)


//Type Define
//Base Type
typedef __int64 registerdata;

//8~64bit name contain Struct
typedef struct RegisterName
{
    char val64[8];
    char val32[8];
    char val16[8];
    char val8h[8];
    char val8l[8];
} regname;

//64bit Register
typedef struct Register{
    registerdata data;
    regname name;
} reg;

//Register Reference
typedef struct RegisterReference{
    char mode;
    char* name;
    registerdata* val;
} regref;

//Modrm Contain object struct
typedef struct ModrmObject
{
    //Addisional Values
    char *addmne;
    char addmneno;
    __int64 adddat;
    //Core Values
    char mod;
    regref r;
    regref m;
} modrm;

//Macros
#define Reg8h(data) ((data)>>8&(__int64)0xff)
#define Reg8l(data) ((data)&(__int64)0xff)
#define Reg16(data) ((data)&(__int64)0xffff)
#define Reg32(data) ((data)&(__int64)0xffffffff)
#define Reg8hp(data) (((char*)&data+1)+0)
#define Reg8lp(data) (((char*)&data)+0)
#define Reg16p(data) (((char*)&data)+0)
#define Reg32p(data) (((char*)&data)+0)
#define DataAdd(data,add) *(((char*)&data)+add)

#endif