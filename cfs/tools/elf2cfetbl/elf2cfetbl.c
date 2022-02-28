/*
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
**
**  Purpose:
**    This file implements the ELF file to Standard cFE Table file format tool
**
**  Notes:
**
**
*/

/*
** Required header files.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <limits.h>
#include "ELF_Structures.h"
#include "cfe_tbl_filedef.h"
#include "elf2cfetbl_version.h"

#define MAX_SECTION_HDR_NAME_LEN (128)
#define TBL_DEF_SYMBOL_NAME      "CFE_TBL_FileDef"
#define SUCCESS                  (0)
#define FAILED                   (1)

/* macro to construct 32 bit value from 4 chars */
#define U32FROM4CHARS(_C1, _C2, _C3, _C4) \
    ((uint32)(_C1) << 24 | (uint32)(_C2) << 16 | (uint32)(_C3) << 8 | (uint32)(_C4))

typedef struct
{
    int32 Value;
    char  String[50];
} ElfStrMap;

/**
 *    Function Prototypes
 */
int32 ProcessCmdLineOptions(int argc, char *argv[]);
int32 GetSrcFilename(void);
int32 GetDstFilename(void);
int32 OpenSrcFile(void);
int32 OpenDstFile(void);
int32 GetElfHeader(void);
void  SwapElfHeader(void);
int32 GetSectionHeader(int32 SectionIndex, union Elf_Shdr *SectionHeader);
void  SwapSectionHeader(union Elf_Shdr *SectionHeader);
int32 GetSymbol(int32 SymbolIndex, union Elf_Sym *Symbol);
void  SwapSymbol(union Elf_Sym *Symbol);
int32 GetStringFromMap(char *Result, ElfStrMap *Map, int32 Key);
void  SwapUInt16(uint16 *ValueToSwap);
void  SwapUInt32(uint32 *ValueToSwap);
void  SwapUInt64(uint64 *ValueToSwap);
int32 AllocateSectionHeaders(void);
void  DeallocateSectionHeaders(void);
int32 AllocateSymbols(void);
void  DeallocateSymbols(void);
void  FreeMemoryAllocations(void);
int32 GetTblDefInfo(void);
int32 OutputDataToTargetFile(void);
void  OutputVersionInfo(void);
void  OutputHelpInfo(void);
int32 LocateAndReadUserObject(void);

void PrintSymbol32(union Elf_Sym *Symbol);
void PrintSymbol64(union Elf_Sym *Symbol);
void PrintSectionHeader32(union Elf_Shdr *SectionHeader);
void PrintSectionHeader64(union Elf_Shdr *SectionHeader);
void PrintElfHeader32(union Elf_Ehdr ElfHeaderLcl);
void PrintElfHeader64(union Elf_Ehdr ElfHeaderLcl);

/**
 *    Global Variables
 */
char SrcFilename[PATH_MAX] = {""};
char DstFilename[PATH_MAX] = {""};
char TableName[38]         = {""};
char Description[32]       = {""};
char LineOfText[300]       = {""};

bool Verbose                     = false;
bool ReportVersion               = false;
bool OutputHelp                  = false;
bool ByteSwapRequired            = false;
bool ScIDSpecified               = false;
bool ProcIDSpecified             = false;
bool AppIDSpecified              = false;
bool ScEpochSpecified            = false;
bool FileEpochSpecified          = false;
bool TableNameOverride           = false;
bool DescriptionOverride         = false;
bool ThisMachineIsLittleEndian   = true;
bool TargetMachineIsLittleEndian = true;
bool EnableTimeTagInHeader       = false;
bool TargetWordsizeIs32Bit       = true;

bool TableDataIsAllZeros = false;

FILE *SrcFileDesc = NULL;
FILE *DstFileDesc = NULL;

CFE_FS_Header_t    FileHeader;
CFE_TBL_File_Hdr_t TableHeader;

union Elf_Ehdr   ElfHeader;
union Elf_Shdr **SectionHeaderPtrs                  = NULL;
union Elf_Shdr   SectionHeaderStringTable           = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
int64            SectionHeaderStringTableDataOffset = 0;
char **          SectionNamePtrs                    = NULL;

struct stat SrcFileStats;

uint64_t          StringTableDataOffset = 0;
int32             SymbolTableDataOffset = 0;
uint64_t          NumSymbols            = 0;
uint64_t          SymbolTableEntrySize  = 0;
union Elf_Sym **  SymbolPtrs            = NULL;
char **           SymbolNames;
int32             TblDefSymbolIndex = -1;
CFE_TBL_FileDef_t TblFileDef;
int32             UserObjSymbolIndex = -1;
uint32            SpacecraftID       = 0;
uint32            ProcessorID        = 0;
uint32            ApplicationID      = 0;
time_t            EpochTime          = 0;

typedef struct
{
    uint32 Year;
    uint32 Month;
    uint32 Day;
    uint32 Hour;
    uint32 Minute;
    uint32 Second;
} SpecifiedEpoch_t;

SpecifiedEpoch_t ScEpoch   = {1970, 1, 1, 0, 0, 0};
SpecifiedEpoch_t FileEpoch = {1970, 1, 1, 0, 0, 0};
time_t           EpochDelta;
time_t           SrcFileTimeInScEpoch;

/**
 *    ELF Characteristic Maps
 */
ElfStrMap e_type_Map[] = {
    {ET_NONE, "ET_NONE (0)"}, {ET_REL, "ET_REL (1)"},   {ET_EXEC, "ET_EXEC (2)"},
    {ET_DYN, "ET_DYN (3)"},   {ET_CORE, "ET_CORE (4)"}, {0, "* Unknown Elf File Type (%d) *"},
};

ElfStrMap e_machine_Map[] = {
    {EM_NONE, "EM_NONE         ( 0)"},
    {EM_M32, "EM_M32          ( 1)"},
    {EM_SPARC, "EM_SPARC        ( 2)"},
    {EM_386, "EM_386          ( 3)"},
    {EM_68K, "EM_68K          ( 4)"},
    {EM_88K, "EM_88K          ( 5)"},
    {EM_860, "EM_860          ( 7)"},
    {EM_MIPS, "EM_MIPS         ( 8)"},
    {EM_S370, "EM_S370         ( 9)"},
    {EM_MIPS_RS3_LE, "EM_MIPS_RS3_LE  (10)"},
    {EM_PARISC, "EM_PARISC       (15)"},
    {EM_VPP500, "EM_VPP500       (17)"},
    {EM_SPARC32PLUS, "EM_SPARC32PLUS  (18)"},
    {EM_960, "EM_960          (19)"},
    {EM_PPC, "EM_PPC          (20)"},
    {EM_PPC64, "EM_PPC64        (21)"},
    {EM_S390, "EM_S390         (22)"},
    {EM_SPU, "EM_SPU          (23)"},
    {EM_V800, "EM_V800         (36)"},
    {EM_FR20, "EM_FR20         (37)"},
    {EM_RH32, "EM_RH32         (38)"},
    {EM_RCE, "EM_RCE          (39)"},
    {EM_ARM, "EM_ARM          (40)"},
    {EM_ALPHA, "EM_ALPHA        (41)"},
    {EM_SH, "EM_SH           (42)"},
    {EM_SPARCV9, "EM_SPARCV9      (43)"},
    {EM_TRICORE, "EM_TRICORE      (44)"},
    {EM_ARC, "EM_ARC          (45)"},
    {EM_H8_300, "EM_H8_300       (46)"},
    {EM_H8_300H, "EM_H8_300H      (47)"},
    {EM_H8S, "EM_H8S          (48)"},
    {EM_H8_500, "EM_H8_500       (49)"},
    {EM_IA_64, "EM_IA_64        (50)"},
    {EM_MIPS_X, "EM_MIPS_X       (51)"},
    {EM_COLDFIRE, "EM_COLDFIRE     (52)"},
    {EM_68HC12, "EM_68HC12       (53)"},
    {EM_MMA, "EM_MMA          (54)"},
    {EM_PCP, "EM_PCP          (55)"},
    {EM_NCPU, "EM_NCPU         (56)"},
    {EM_NDR1, "EM_NDR1         (57)"},
    {EM_STARCORE, "EM_STARCORE     (58)"},
    {EM_ME16, "EM_ME16         (59)"},
    {EM_ST100, "EM_ST100        (60)"},
    {EM_TINYJ, "EM_TINYJ        (61)"},
    {EM_X86_64, "EM_X86_64       (62)"},
    {EM_PDSP, "EM_PDSP         (63)"},
    {EM_PDP10, "EM_PDP10        (64)"},
    {EM_PDP11, "EM_PDP11        (65)"},
    {EM_FX66, "EM_FX66         (66)"},
    {EM_ST9PLUS, "EM_ST9PLUS      (67)"},
    {EM_ST7, "EM_ST7          (68)"},
    {EM_68HC16, "EM_68HC16       (69)"},
    {EM_68HC11, "EM_68HC11       (70)"},
    {EM_68HC08, "EM_68HC08       (71)"},
    {EM_68HC05, "EM_68HC05       (72)"},
    {EM_SVX, "EM_SVX          (73)"},
    {EM_ST19, "EM_ST19         (74)"},
    {EM_VAX, "EM_VAX          (75)"},
    {EM_CRIS, "EM_CRIS         (76)"},
    {EM_JAVELIN, "EM_JAVELIN      (77)"},
    {EM_FIREPATH, "EM_FIREPATH     (78)"},
    {EM_ZSP, "EM_ZSP          (79)"},
    {EM_MMIX, "EM_MMIX         (80)"},
    {EM_HUANY, "EM_HUANY        (81)"},
    {EM_PRISM, "EM_PRISM        (82)"},
    {EM_AVR, "EM_AVR          (83)"},
    {EM_FR30, "EM_FR30         (84)"},
    {EM_D10V, "EM_D10V         (85)"},
    {EM_D30V, "EM_D30V         (86)"},
    {EM_V850, "EM_V850         (87)"},
    {EM_M32R, "EM_M32R         (88)"},
    {EM_MN10300, "EM_MN10300      (89)"},
    {EM_MN10200, "EM_MN10200      (90)"},
    {EM_PJ, "EM_PJ           (91)"},
    {EM_OPENRISC, "EM_OPENRISC     (92)"},
    {EM_ARC_COMPACT, "EM_ARC_COMPACT  (93)"},
    {EM_XTENSA, "EM_XTENSA       (94)"},
    {EM_VIDEOCORE, "EM_VIDEOCORE    (95)"},
    {EM_TMM_GPP, "EM_TMM_GPP      (96)"},
    {EM_NS32K, "EM_NS32K        (97)"},
    {EM_TPC, "EM_TPC          (98)"},
    {EM_SNP1K, "EM_SNP1K        (99)"},
    {EM_ST200, "EM_ST200       (100)"},
    {EM_IP2K, "EM_IP2K        (101)"},
    {EM_MAX, "EM_MAX         (102)"},
    {EM_CR, "EM_CR          (103)"},
    {EM_F2MC16, "EM_F2MC16      (104)"},
    {EM_MSP430, "EM_MSP430      (105)"},
    {EM_BLACKFIN, "EM_BLACKFIN    (106)"},
    {EM_SE_C33, "EM_SE_C33      (107)"},
    {EM_SEP, "EM_SEP         (108)"},
    {EM_ARCA, "EM_ARCA        (109)"},
    {EM_UNICORE, "EM_UNICORE     (110)"},
    {EM_EXCESS, "EM_EXCESS      (111)"},
    {EM_DXP, "EM_DXP         (112)"},
    {EM_ALTERA_NIOS2, "EM_ALTERA_NIOS2 (113)"},
    {EM_CRX, "EM_CRX         (114)"},
    {EM_XGATE, "EM_XGATE       (115)"},
    {EM_C166, "EM_C166        (116)"},
    {EM_M16C, "EM_M16C        (117)"},
    {EM_DSPIC30F, "EM_DSPIC30F    (118)"},
    {EM_CE, "EM_CE          (119)"},
    {EM_M32C, "EM_M32C        (120)"},
    {EM_TSK3000, "EM_TSK3000     (131)"},
    {EM_RS08, "EM_RS08        (132)"},
    {EM_SHARC, "EM_SHARC       (133)"},
    {EM_ECOG2, "EM_ECOG2       (134)"},
    {EM_SCORE7, "EM_SCORE7      (135)"},
    {EM_DSP24, "EM_DSP24       (136)"},
    {EM_VIDEOCORE3, "EM_VIDEOCORE3  (137)"},
    {EM_LATTICEMICO32, "EM_LATTICEMICO32(138)"},
    {EM_SE_C17, "EM_SE_C17      (139)"},
    {EM_TI_C6000, "EM_TI_C6000    (140)"},
    {EM_TI_C2000, "EM_TI_C2000    (141)"},
    {EM_TI_C5500, "EM_TI_C5500    (142)"},
    {EM_TI_ARP32, "EM_TI_ARP32    (143)"},
    {EM_TI_PRU, "EM_TI_PRU      (144)"},
    {EM_MMDSP_PLUS, "EM_MMDSP_PLUS  (160)"},
    {EM_CYPRESS_M8C, "EM_CYPRESS_M8C (161)"},
    {EM_R32C, "EM_R32C        (162)"},
    {EM_TRIMEDIA, "EM_TRIMEDIA    (163)"},
    {EM_QDSP6, "EM_QDSP6       (164)"},
    {EM_8051, "EM_8051        (165)"},
    {EM_STXP7X, "EM_STXP7X      (166)"},
    {EM_NDS32, "EM_NDS32       (167)"},
    {EM_ECOG1, "EM_ECOG1       (168)"},
    {EM_ECOG1X, "EM_ECOG1X      (168)"},
    {EM_MAXQ30, "EM_MAXQ30      (169)"},
    {EM_XIMO16, "EM_XIMO16      (170)"},
    {EM_MANIK, "EM_MANIK       (171)"},
    {EM_RX, "EM_RX          (173)"},
    {EM_METAG, "EM_METAG       (174)"},
    {EM_MCST_ELBRUS, "EM_MCST_ELBRUS (175)"},
    {EM_ECOG16, "EM_ECOG16      (176)"},
    {EM_CR16, "EM_CR16        (177)"},
    {EM_ETPU, "EM_ETPU        (178)"},
    {EM_SLE9X, "EM_SLE9X       (179)"},
    {EM_L10M, "EM_L10M        (180)"},
    {EM_K10M, "EM_K10M        (181)"},
    {EM_AARCH64, "EM_AARCH64     (183)"},
    {EM_AVR32, "EM_AVR32       (185)"},
    {EM_STM8, "EM_STM8        (186)"},
    {EM_TILE64, "EM_TILE64      (187)"},
    {EM_TILEPRO, "EM_TILEPRO     (188)"},
    {EM_MICROBLAZE, "EM_MICROBLAZE  (189)"},
    {EM_CUDA, "EM_CUDA        (190)"},
    {EM_TILEGX, "EM_TILEGX      (191)"},
    {EM_CLOUDSHIELD, "EM_CLOUDSHIELD (192)"},
    {EM_COREA_1ST, "EM_COREA_1ST   (193)"},
    {EM_COREA_2ND, "EM_COREA_2ND   (194)"},
    {EM_ARC_COMPACT2, "EM_ARC_COMPACT2 (195)"},
    {EM_OPEN8, "EM_OPEN8       (196)"},
    {EM_RL78, "EM_RL78        (197)"},
    {EM_VIDEOCORE5, "EM_VIDEOCORE5  (198)"},
    {EM_78KOR, "EM_78KOR       (199)"},
    {EM_56800EX, "EM_56800EX     (200)"},
    {EM_BA1, "EM_BA1         (201)"},
    {EM_BA2, "EM_BA2         (202)"},
    {EM_XCORE, "EM_XCORE       (203)"},
    {EM_MCHP_PIC, "EM_MCHP_PIC    (204)"},
    {EM_INTEL205, "EM_INTEL205    (205)"},
    {EM_INTEL206, "EM_INTEL206    (206)"},
    {EM_INTEL207, "EM_INTEL207    (207)"},
    {EM_INTEL208, "EM_INTEL208    (208)"},
    {EM_INTEL209, "EM_INTEL209    (209)"},
    {EM_KM32, "EM_KM32        (210)"},
    {EM_KMX32, "EM_KMX32       (211)"},
    {EM_KMX16, "EM_KMX16       (212)"},
    {EM_KMX8, "EM_KMX8        (213)"},
    {EM_KVARC, "EM_KVARC       (214)"},
    {EM_CDP, "EM_CDP         (215)"},
    {EM_COGE, "EM_COGE        (216)"},
    {EM_COOL, "EM_COOL        (217)"},
    {EM_NORC, "EM_NORC        (218)"},
    {EM_CSR_KALIMBA, "EM_CSR_KALIMBA (219)"},
    {EM_Z80, "EM_Z80         (220)"},
    {EM_VISIUM, "EM_VISIUM      (221)"},
    {EM_FT32, "EM_FT32        (222)"},
    {EM_MOXIE, "EM_MOXIE       (223)"},
    {EM_AMDGPU, "EM_AMDGPU      (224)"},
    {EM_RISCV, "EM_RISCV       (243)"},
    {0, "* Unknown Machine Type (%d) *"},
};

// Elf Header helper functions
uint8_t get_e_ident(const union Elf_Ehdr *ElfHeaderLcl, int index)
{
    if (TargetWordsizeIs32Bit)
    {
        return ElfHeaderLcl->Ehdr32.e_ident[index];
    }
    else
    {
        return ElfHeaderLcl->Ehdr64.e_ident[index];
    }
}

uint16_t get_e_type(const union Elf_Ehdr *ElfHeaderLcl)
{
    if (TargetWordsizeIs32Bit)
    {
        return ElfHeaderLcl->Ehdr32.e_type;
    }
    else
    {
        return ElfHeaderLcl->Ehdr64.e_type;
    }
}

uint16_t get_e_machine(const union Elf_Ehdr *ElfHeaderLcl)
{
    if (TargetWordsizeIs32Bit)
    {
        return ElfHeaderLcl->Ehdr32.e_machine;
    }
    else
    {
        return ElfHeaderLcl->Ehdr64.e_machine;
    }
}

uint32_t get_e_version(const union Elf_Ehdr *ElfHeaderLcl)
{
    if (TargetWordsizeIs32Bit)
    {
        return ElfHeaderLcl->Ehdr32.e_version;
    }
    else
    {
        return ElfHeaderLcl->Ehdr64.e_version;
    }
}

uint16_t get_e_shstrndx(const union Elf_Ehdr *ElfHeaderLcl)
{
    if (TargetWordsizeIs32Bit)
    {
        return ElfHeaderLcl->Ehdr32.e_shstrndx;
    }
    else
    {
        return ElfHeaderLcl->Ehdr64.e_shstrndx;
    }
}

uint16_t get_e_shnum(const union Elf_Ehdr *ElfHeaderLcl)
{
    if (TargetWordsizeIs32Bit)
    {
        return ElfHeaderLcl->Ehdr32.e_shnum;
    }
    else
    {
        return ElfHeaderLcl->Ehdr64.e_shnum;
    }
}

// Elf Section Header helper functions

uint32_t get_sh_name(const union Elf_Shdr *SectionHeader)
{
    if (TargetWordsizeIs32Bit)
    {
        return SectionHeader->Shdr32.sh_name;
    }
    else
    {
        return SectionHeader->Shdr64.sh_name;
    }
}

uint32_t get_sh_type(const union Elf_Shdr *SectionHeader)
{
    if (TargetWordsizeIs32Bit)
    {
        return SectionHeader->Shdr32.sh_type;
    }
    else
    {
        return SectionHeader->Shdr64.sh_type;
    }
}

void print_sh_flags(const union Elf_Shdr *SectionHeader)
{
    char VerboseStr[60];

    sprintf(VerboseStr, "/");
    if (TargetWordsizeIs32Bit)
    {
        if ((SectionHeader->Shdr32.sh_flags & SHF_WRITE) == SHF_WRITE)
        {
            sprintf(VerboseStr, "SHF_WRITE/");
        }

        if ((SectionHeader->Shdr32.sh_flags & SHF_ALLOC) == SHF_ALLOC)
        {
            strcat(VerboseStr, "SHF_ALLOC/");
        }

        if ((SectionHeader->Shdr32.sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR)
        {
            strcat(VerboseStr, "SHF_EXECINSTR/");
        }

        printf("   sh_flags      = %s\n", VerboseStr);
    }
    else
    {
        if ((SectionHeader->Shdr64.sh_flags & SHF_WRITE) == SHF_WRITE)
        {
            sprintf(VerboseStr, "SHF_WRITE/");
        }

        if ((SectionHeader->Shdr64.sh_flags & SHF_ALLOC) == SHF_ALLOC)
        {
            strcat(VerboseStr, "SHF_ALLOC/");
        }

        if ((SectionHeader->Shdr64.sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR)
        {
            strcat(VerboseStr, "SHF_EXECINSTR/");
        }

        printf("   sh_flags      = %s\n", VerboseStr);
    }
}

uint64_t get_sh_offset(const union Elf_Shdr *SectionHeader)
{
    if (TargetWordsizeIs32Bit)
    {
        return SectionHeader->Shdr32.sh_offset;
    }
    else
    {
        return SectionHeader->Shdr64.sh_offset;
    }
}

uint64_t get_sh_size(const union Elf_Shdr *SectionHeader)
{
    if (TargetWordsizeIs32Bit)
    {
        return SectionHeader->Shdr32.sh_size;
    }
    else
    {
        return SectionHeader->Shdr64.sh_size;
    }
}

uint64_t get_sh_entsize(const union Elf_Shdr *SectionHeader)
{
    if (TargetWordsizeIs32Bit)
    {
        return SectionHeader->Shdr32.sh_entsize;
    }
    else
    {
        return SectionHeader->Shdr64.sh_entsize;
    }
}

// Elf_Sym helper functions

uint32_t get_st_name(const union Elf_Sym *Symbol)
{
    if (TargetWordsizeIs32Bit)
    {
        return Symbol->Sym32.st_name;
    }
    else
    {
        return Symbol->Sym64.st_name;
    }
}

uint64_t get_st_value(const union Elf_Sym *Symbol)
{
    if (TargetWordsizeIs32Bit)
    {
        return Symbol->Sym32.st_value;
    }
    else
    {
        return Symbol->Sym64.st_value;
    }
}

uint64_t get_st_size(const union Elf_Sym *Symbol)
{
    if (TargetWordsizeIs32Bit)
    {
        return Symbol->Sym32.st_size;
    }
    else
    {
        return Symbol->Sym64.st_size;
    }
}

void set_st_size(union Elf_Sym *Symbol, uint64_t new_value)
{
    if (TargetWordsizeIs32Bit)
    {
        Symbol->Sym32.st_size = (uint32_t)new_value;
        if (Symbol->Sym32.st_size != new_value)
        {
            printf("ERROR: Sym32.st_size can not hold %lu\n", (long unsigned int)new_value);
        }
    }
    else
    {
        Symbol->Sym64.st_size = new_value;
    }
}

uint16_t get_st_shndx(const union Elf_Sym *Symbol)
{
    if (TargetWordsizeIs32Bit)
    {
        return Symbol->Sym32.st_shndx;
    }
    else
    {
        return Symbol->Sym64.st_shndx;
    }
}

/**
 *
 */

int main(int argc, char *argv[])
{
    int32 Status = SUCCESS;
    int32 i      = 0;

    Status = ProcessCmdLineOptions(argc, argv);
    if (Status != SUCCESS)
        return Status;

    if (ReportVersion)
        OutputVersionInfo();

    Status = GetSrcFilename();
    if (OutputHelp)
        OutputHelpInfo();
    if (Status != SUCCESS)
        return Status;

    Status = OpenSrcFile();
    if (Status != SUCCESS)
        return Status;

    Status = GetElfHeader();
    if (Status != SUCCESS)
        return Status;

    /* Get the string section header first */
    Status = GetSectionHeader(get_e_shstrndx(&ElfHeader), &SectionHeaderStringTable);
    if (Status != SUCCESS)
        return Status;

    if (TargetWordsizeIs32Bit)
    {
        SectionHeaderStringTableDataOffset = SectionHeaderStringTable.Shdr32.sh_offset;
    }
    else
    {
        SectionHeaderStringTableDataOffset = SectionHeaderStringTable.Shdr64.sh_offset;
    }

    /* Allocate memory for all of the ELF object file section headers */
    Status = AllocateSectionHeaders();
    if (Status != SUCCESS)
    {
        FreeMemoryAllocations();
        return Status;
    }

    /* Read in each section header from input file */
    for (i = 0; i < get_e_shnum(&ElfHeader); i++)
    {
        Status = GetSectionHeader(i, SectionHeaderPtrs[i]);
        if (Status != SUCCESS)
        {
            FreeMemoryAllocations();
            return Status;
        }
    }

    if (StringTableDataOffset == 0)
    {
        printf("Error! Unable to locate ELF string table for symbol names\n");
        return EXIT_FAILURE;
    }

    /* Allocate memory for all of the symbol table entries */
    Status = AllocateSymbols();
    if (Status != SUCCESS)
    {
        FreeMemoryAllocations();
        return Status;
    }

    /* Read in each symbol table entry */
    for (i = 0; i < NumSymbols; i++)
    {
        Status = GetSymbol(i, SymbolPtrs[i]);
        if (Status != SUCCESS)
        {
            FreeMemoryAllocations();
            return Status;
        }
    }

    if (TblDefSymbolIndex == -1)
    {
        printf("Error! Unable to locate '%s' object in '%s'.\n", TBL_DEF_SYMBOL_NAME, SrcFilename);
        FreeMemoryAllocations();
        return EXIT_FAILURE;
    }

    /* Read in the definition of the table file */
    Status = GetTblDefInfo();
    if (Status != SUCCESS)
    {
        FreeMemoryAllocations();
        return Status;
    }

    Status = GetDstFilename();
    if (Status != SUCCESS)
        return Status;

    Status = OpenDstFile();
    if (Status != SUCCESS)
        return Status;

    Status = LocateAndReadUserObject();
    if (Status != SUCCESS)
    {
        FreeMemoryAllocations();
        return Status;
    }

    Status = OutputDataToTargetFile();

    FreeMemoryAllocations();

    return SUCCESS;
}

/**
 *
 */

int32 AllocateSectionHeaders(void)
{
    int32 Status = SUCCESS;
    int32 i      = 0;

    if (get_e_shnum(&ElfHeader) == 0)
    {
        printf("Error! Failed to locate any Section Headers in '%s'!\n", SrcFilename);
        Status = FAILED;
    }
    else
    {
        SectionHeaderPtrs = (union Elf_Shdr **)malloc(sizeof(union Elf_Shdr *) * get_e_shnum(&ElfHeader));
        if (SectionHeaderPtrs == NULL)
        {
            printf("Error! Insufficient memory for number of Sections in '%s'!\n", SrcFilename);
            Status = FAILED;
        }

        SectionNamePtrs = (char **)malloc(sizeof(char *) * get_e_shnum(&ElfHeader));
        if (SectionNamePtrs == NULL)
        {
            printf("Error! Insufficient memory for number of Sections in '%s'!\n", SrcFilename);
            Status = FAILED;
        }

        if (Status == SUCCESS)
        {
            /* Initialize all of the pointers to NULL */
            for (i = 0; i < get_e_shnum(&ElfHeader); i++)
            {
                SectionHeaderPtrs[i] = NULL;
                SectionNamePtrs[i]   = NULL;
            }

            /* Allocate memory for each header */
            for (i = 0; i < get_e_shnum(&ElfHeader); i++)
            {
                SectionHeaderPtrs[i] = (union Elf_Shdr *)malloc(sizeof(union Elf_Shdr));
                if (SectionHeaderPtrs[i] == NULL)
                {
                    printf("Error! Insufficient memory to store Section Headers\n");
                    Status = FAILED;
                }

                SectionNamePtrs[i] = (char *)malloc(MAX_SECTION_HDR_NAME_LEN);
                if (SectionNamePtrs[i] == NULL)
                {
                    printf("Error! Insufficient memory to store Section Names\n");
                    Status = FAILED;
                }
            }
        }
    }

    return Status;
}

/**
 *
 */

void DeallocateSectionHeaders(void)
{
    int32 i = 0;

    if (SectionHeaderPtrs != NULL)
    {
        while ((i < get_e_shnum(&ElfHeader)) && (SectionHeaderPtrs[i] != NULL))
        {
            free(SectionHeaderPtrs[i]);
            i++;
        }
        free(SectionHeaderPtrs);
    }

    i = 0;

    if (SectionNamePtrs != NULL)
    {
        while ((i < get_e_shnum(&ElfHeader)) && (SectionNamePtrs[i] != NULL))
        {
            free(SectionNamePtrs[i]);
            i++;
        }
        free(SectionNamePtrs);
    }
}

/**
 *
 */

int32 AllocateSymbols(void)
{
    int32 Status = SUCCESS;
    int32 i      = 0;

    if (NumSymbols == 0)
    {
        printf("Error! Failed to locate any Symbols in '%s'!\n", SrcFilename);
        Status = FAILED;
    }
    else
    {
        SymbolPtrs = malloc(sizeof(union Elf_Sym *) * NumSymbols);

        if (SymbolPtrs == NULL)
        {
            printf("Error! Insufficient memory for number of Symbols in '%s'!\n", SrcFilename);
            Status = FAILED;
        }

        SymbolNames = malloc(sizeof(char *) * NumSymbols);
        if (SymbolNames == NULL)
        {
            printf("Error! Insufficient memory for number of Symbols in '%s'!\n", SrcFilename);
            Status = FAILED;
        }

        for (i = 0; i < NumSymbols; i++)
        {
            SymbolPtrs[i]  = NULL;
            SymbolNames[i] = NULL;
        }

        if (Status == SUCCESS)
        {
            /* Allocate memory for each symbol */
            for (i = 0; i < NumSymbols; i++)
            {
                SymbolPtrs[i] = (union Elf_Sym *)malloc(sizeof(union Elf_Sym));
                if (SymbolPtrs[i] == NULL)
                {
                    printf("Error! Insufficient memory to store Symbol Headers\n");
                    Status = FAILED;
                }
            }
        }
    }

    return Status;
}

/**
 *
 */

void DeallocateSymbols(void)
{
    int32 i = 0;

    if (SymbolPtrs != NULL)
    {
        while ((i < NumSymbols) && (SymbolPtrs[i] != NULL))
        {
            free(SymbolPtrs[i]);

            if (SymbolNames[i] != NULL)
            {
                free(SymbolNames[i]);
            }

            i++;
        }
        free(SymbolPtrs);
    }
}

/**
 *
 */

void FreeMemoryAllocations(void)
{
    DeallocateSymbols();
    DeallocateSectionHeaders();

    if (SrcFileDesc != NULL)
    {
        fclose(SrcFileDesc);
    }

    if (DstFileDesc != NULL)
    {
        fclose(DstFileDesc);
    }
}

/**
 *
 */

int32 ProcessCmdLineOptions(int ArgumentCount, char *Arguments[])
{
    int32     Status              = SUCCESS;
    bool      InputFileSpecified  = false;
    bool      OutputFileSpecified = false;
    int       i                   = 1;
    char *    EndPtr;
    uint32    MaxDay;
    struct tm FileEpochTm;
    struct tm ScEpochTm;
    time_t    FileEpochInSecs;
    time_t    ScEpochInSecs;

    while ((i < ArgumentCount) && (Status == SUCCESS))
    {
        if ((Arguments[i][0] == '-') && (Arguments[i][1] == 't'))
        {
            /* Extract the Table Name Override */
            strncpy(TableName, &Arguments[i][2], sizeof(TableName) - 1);
            TableName[sizeof(TableName) - 1] = 0;
            TableNameOverride                = true;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'd'))
        {
            /* Extract the Description Override */
            strncpy(Description, &Arguments[i][2], sizeof(Description) - 1);
            Description[sizeof(Description) - 1] = 0;
            DescriptionOverride                  = true;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 's'))
        {
            SpacecraftID = strtoul(&Arguments[i][2], &EndPtr, 0);
            if (EndPtr != &Arguments[i][2])
            {
                ScIDSpecified = true;
            }
            else
            {
                printf("Error!, Spacecraft ID of '%s' cannot be interpreted as an integer.\n", &Arguments[i][2]);
                Status = false;
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'S'))
        {
            if (strlen(&Arguments[i][2]) == 4)
            {
                SpacecraftID  = U32FROM4CHARS(Arguments[i][2], Arguments[i][3], Arguments[i][4], Arguments[i][5]);
                ScIDSpecified = true;
            }
            else
            {
                printf("Error!, Spacecraft ID of '%s' does not have exactly 4 characters.\n", &Arguments[i][2]);
                Status = false;
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'a'))
        {
            ApplicationID = strtoul(&Arguments[i][2], &EndPtr, 0);
            if (EndPtr != &Arguments[i][2])
            {
                AppIDSpecified = true;
            }
            else
            {
                printf("Error!, Application ID of '%s' cannot be interpreted as an integer.\n", &Arguments[i][2]);
                Status = false;
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'p'))
        {
            ProcIDSpecified = true;
            ProcessorID     = strtoul(&Arguments[i][2], &EndPtr, 0);
            if (EndPtr != &Arguments[i][2])
            {
                ProcIDSpecified = true;
            }
            else
            {
                printf("Error!, Processor ID of '%s' cannot be interpreted as an integer.\n", &Arguments[i][2]);
                Status = false;
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'P'))
        {
            if (strlen(&Arguments[i][2]) == 4)
            {
                ProcessorID     = U32FROM4CHARS(Arguments[i][2], Arguments[i][3], Arguments[i][4], Arguments[i][5]);
                ProcIDSpecified = true;
            }
            else
            {
                printf("Error!, Processor ID of '%s' does not have exactly 4 characters.\n", &Arguments[i][2]);
                Status = false;
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'h'))
        {
            OutputHelp = true;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'v'))
        {
            Verbose = true;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'V'))
        {
            ReportVersion = true;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'n'))
        {
            /* This option is ignored for compatibility */
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'T'))
        {
            EnableTimeTagInHeader = true;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'e'))
        {
            ScEpoch.Year = strtoul(&Arguments[i][2], &EndPtr, 0);
            if (EndPtr != &Arguments[i][6])
            {
                fprintf(stderr, "Error! Spacecraft Epoch Year is not of the form 'YYYY:'\n");
                Status = false;
            }
            else
            {
                ScEpoch.Month = strtoul(&Arguments[i][7], &EndPtr, 0);
                if ((EndPtr != &Arguments[i][9]) || (ScEpoch.Month == 0) || (ScEpoch.Month > 12))
                {
                    fprintf(
                        stderr,
                        "Error! Spacecraft Epoch Month is not of the form 'MM:' where MM is in the range of 1-12\n");
                    Status = false;
                }
                else
                {
                    MaxDay = 31;
                    if ((ScEpoch.Month == 4) || (ScEpoch.Month == 6) || (ScEpoch.Month == 9) || (ScEpoch.Month == 11))
                    {
                        MaxDay = 30;
                    }
                    else if (ScEpoch.Month == 2)
                    {
                        if ((ScEpoch.Year % 4) == 0)
                        {
                            if ((ScEpoch.Year % 100) == 0)
                            {
                                if ((ScEpoch.Year % 400) == 0)
                                {
                                    MaxDay = 29;
                                }
                                else
                                {
                                    MaxDay = 28;
                                }
                            }
                            else
                            {
                                MaxDay = 29;
                            }
                        }
                        else
                        {
                            MaxDay = 28;
                        }
                    }
                    ScEpoch.Day = strtoul(&Arguments[i][10], &EndPtr, 0);
                    if ((EndPtr != &Arguments[i][12]) || (ScEpoch.Day == 0) || (ScEpoch.Day > MaxDay))
                    {
                        fprintf(
                            stderr,
                            "Error! Spacecraft Epoch Day is not of the form 'DD:' where DD is in the range of 1-%d\n",
                            MaxDay);
                        Status = false;
                    }
                    else
                    {
                        ScEpoch.Hour = strtoul(&Arguments[i][13], &EndPtr, 0);
                        if ((EndPtr != &Arguments[i][15]) || (ScEpoch.Hour > 23))
                        {
                            fprintf(stderr, "Error! Spacecraft Epoch Hour is not of the form 'hh:' where hh is in the "
                                            "range of 0-23\n");
                            Status = false;
                        }
                        else
                        {
                            ScEpoch.Minute = strtoul(&Arguments[i][16], &EndPtr, 0);
                            if ((EndPtr != &Arguments[i][18]) || (ScEpoch.Minute > 59))
                            {
                                fprintf(stderr, "Error! Spacecraft Epoch Minute is not of the form 'mm:' where mm is "
                                                "in the range of 0-59\n");
                                Status = false;
                            }
                            else
                            {
                                ScEpoch.Second = strtoul(&Arguments[i][19], &EndPtr, 0);
                                if ((EndPtr != &Arguments[i][21]) || (ScEpoch.Second > 59))
                                {
                                    fprintf(stderr, "Error! Spacecraft Epoch Second is not of the form 'ss' where ss "
                                                    "is in the range of 0-59\n");
                                    Status = false;
                                }
                                else
                                {
                                    ScEpochSpecified = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'f'))
        {
            FileEpoch.Year = strtoul(&Arguments[i][2], &EndPtr, 0);
            if (EndPtr != &Arguments[i][6])
            {
                fprintf(stderr, "Error! File Epoch Year is not of the form 'YYYY:'\n");
                Status = false;
            }
            else
            {
                FileEpoch.Month = strtoul(&Arguments[i][7], &EndPtr, 0);
                if ((EndPtr != &Arguments[i][9]) || (FileEpoch.Month == 0) || (FileEpoch.Month > 12))
                {
                    fprintf(stderr,
                            "Error! File Epoch Month is not of the form 'MM:' where MM is in the range of 1-12\n");
                    Status = false;
                }
                else
                {
                    MaxDay = 31;
                    if ((FileEpoch.Month == 4) || (FileEpoch.Month == 6) || (FileEpoch.Month == 9) ||
                        (FileEpoch.Month == 11))
                    {
                        MaxDay = 30;
                    }
                    else if (FileEpoch.Month == 2)
                    {
                        if ((FileEpoch.Year % 4) == 0)
                        {
                            if ((FileEpoch.Year % 100) == 0)
                            {
                                if ((FileEpoch.Year % 400) == 0)
                                {
                                    MaxDay = 29;
                                }
                                else
                                {
                                    MaxDay = 28;
                                }
                            }
                            else
                            {
                                MaxDay = 29;
                            }
                        }
                        else
                        {
                            MaxDay = 28;
                        }
                    }
                    FileEpoch.Day = strtoul(&Arguments[i][10], &EndPtr, 0);
                    if ((EndPtr != &Arguments[i][12]) || (FileEpoch.Day == 0) || (FileEpoch.Day > MaxDay))
                    {
                        fprintf(stderr,
                                "Error! File Epoch Day is not of the form 'DD:' where DD is in the range of 1-%d\n",
                                MaxDay);
                        Status = false;
                    }
                    else
                    {
                        FileEpoch.Hour = strtoul(&Arguments[i][13], &EndPtr, 0);
                        if ((EndPtr != &Arguments[i][15]) || (FileEpoch.Hour > 23))
                        {
                            fprintf(
                                stderr,
                                "Error! File Epoch Hour is not of the form 'hh:' where hh is in the range of 0-23\n");
                            Status = false;
                        }
                        else
                        {
                            FileEpoch.Minute = strtoul(&Arguments[i][16], &EndPtr, 0);
                            if ((EndPtr != &Arguments[i][18]) || (FileEpoch.Minute > 59))
                            {
                                fprintf(stderr, "Error! File Epoch Minute is not of the form 'mm:' where mm is in the "
                                                "range of 0-59\n");
                                Status = false;
                            }
                            else
                            {
                                FileEpoch.Second = strtoul(&Arguments[i][19], &EndPtr, 0);
                                if ((EndPtr != &Arguments[i][21]) || (FileEpoch.Second > 59))
                                {
                                    fprintf(stderr, "Error! File Epoch Second is not of the form 'ss' where ss is in "
                                                    "the range of 0-59\n");
                                    Status = false;
                                }
                                else
                                {
                                    FileEpochSpecified = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (!InputFileSpecified)
        {
            strncpy(SrcFilename, Arguments[i], PATH_MAX - 1);
            SrcFilename[PATH_MAX - 1] = '\0';
            InputFileSpecified        = true;
        }
        else if (!OutputFileSpecified)
        {
            strncpy(DstFilename, Arguments[i], PATH_MAX - 1);
            DstFilename[PATH_MAX - 1] = '\0';
            OutputFileSpecified       = true;
        }
        else
        {
            printf("\nError! Unknown Command Line Option '%s'\n", Arguments[i]);
            Status = FAILED;
        }
        i++;
    }
    FileEpochTm.tm_sec   = FileEpoch.Second;
    FileEpochTm.tm_min   = FileEpoch.Minute;
    FileEpochTm.tm_hour  = FileEpoch.Hour;
    FileEpochTm.tm_mday  = FileEpoch.Day;
    FileEpochTm.tm_mon   = FileEpoch.Month - 1;
    FileEpochTm.tm_year  = FileEpoch.Year - 1900;
    FileEpochTm.tm_isdst = -1;

    FileEpochInSecs = mktime(&FileEpochTm);

    ScEpochTm.tm_sec   = ScEpoch.Second;
    ScEpochTm.tm_min   = ScEpoch.Minute;
    ScEpochTm.tm_hour  = ScEpoch.Hour;
    ScEpochTm.tm_mday  = ScEpoch.Day;
    ScEpochTm.tm_mon   = ScEpoch.Month - 1;
    ScEpochTm.tm_year  = ScEpoch.Year - 1900;
    ScEpochTm.tm_isdst = -1;

    ScEpochInSecs = mktime(&ScEpochTm);

    EpochDelta = FileEpochInSecs - ScEpochInSecs;

    return Status;
}

/**
 *
 */

void OutputVersionInfo(void)
{
    printf("\n%s\n", ELF2CFETBL_VERSION_STRING);
}

/**
 *
 */

void OutputHelpInfo(void)
{
    printf("\nElf Object File to cFE Table Image File Conversion Tool (elf2cfetbl)\n\n");
    printf("elf2cfetbl [-tTblName] [-d\"Description\"] [-h] [-v] [-V] [-s#] [-p#] [-n] \n");
    printf("           [-T] [-eYYYY:MM:DD:hh:mm:ss] [-fYYYY:MM:DD:hh:mm:ss] SrcFilename [DestDirectory]\n");
    printf("   where:\n");
    printf("   -tTblName             replaces the table name specified in the object file with 'TblName'\n");
    printf("   -d\"Description\"       replaces the description specified in the object file with 'Description'\n");
    printf("   -h                    produces this output\n");
    printf("   -v                    produces verbose output showing the breakdown of the object file in detail\n");
    printf("   -V                    shows the version of this utility\n");
    printf("   -s#                   specifies a Spacecraft ID to be put into file header.\n");
    printf("                         # can be specified as decimal, octal (starting with a zero), or hex (starting "
           "with '0x')\n");
    printf("   -Scccc                specifies a Spacecraft ID as a 4 byte string to be put into the table file file "
           "header.\n");
    printf("                         cccc represents the 4 ASCII characters that will be encoded into the 32 bit "
           "Spacecraft ID field.\n");
    printf("                              examples: -SMMS1 or -SQQ#2\n");
    printf("   -p#                   specifies a Processor ID to be put into file header.\n");
    printf("                         # can be specified as decimal, octal (starting with a zero), or hex (starting "
           "with '0x')\n");
    printf(
        "   -Pcccc                specifies a Processor ID as a 4 byte string to be put into the table file header.\n");
    printf("                         cccc represents the 4 ASCII characters that will be encoded into the 32 bit "
           "Processor ID field.\n");
    printf("                              examples: -PMMS1 or -PQQ#2\n");
    printf("   -a#                   specifies an Application ID to be put into file header.\n");
    printf("                         # can be specified as decimal, octal (starting with a zero), or hex (starting "
           "with '0x')\n");
    printf("   -T                    enables insertion of the SrcFilename's file creation time into the standard cFE "
           "File Header.\n");
    printf("                         This option must be specified for either the '-e' and/or '-f' options below to "
           "have any effect.\n");
    printf("                         By default, the time tag fields are set to zero.\n");
    printf("   -eYYYY:MM:DD:hh:mm:ss specifies the spacecraft epoch time.  The SrcFilename's file creation time will "
           "be converted to\n");
    printf("                         seconds since the specified epoch time and stored in the standard cFE File "
           "Header.\n");
    printf("                         where:   YYYY=year, MM=month (01-12), DD=day (01-31), \n");
    printf("                                  hh=hour (00-23), mm=minute (00-59), ss=seconds (00-59)\n");
    printf("                         If no epoch is specified, the default epoch is either 1970:01:01:00:00:00 or the "
           "epoch specified\n");
    printf("                         by the user using the '-f' option described below\n");
    printf("                         This option requires the '-T' option, defined above, to be specified to have any "
           "effect\n");
    printf("   -fYYYY:MM:DD:hh:mm:ss specifies the file system epoch time.  The SrcFilename's file creation time is "
           "obtained from the\n");
    printf("                         file system as seconds since an epoch.  On most systems the file system epoch is "
           "defined as\n");
    printf("                         1970:01:01:00:00:00.  If the user is running this application on a machine with a "
           "different epoch,\n");
    printf("                         then the file system epoch should be defined with this option.\n");
    printf("                         where:   YYYY=year, MM=month (01-12), DD=day (01-31), \n");
    printf("                                  hh=hour (00-23), mm=minute (00-59), ss=seconds (00-59)\n");
    printf("                         If no epoch is specified, the default epoch is 1970:01:01:00:00:00\n");
    printf("                         This option requires the '-T' option, defined above, to be specified to have any "
           "effect\n");
    printf("   SrcFilename           specifies the object file to be converted\n");
    printf("   DestDirectory         specifies the directory in which the cFE Table Image file is to be created.\n");
    printf("                         If a directory is not specified './' is assumed.\n");
    printf("\n");
    printf("EXAMPLES:\n");
    printf("   elf2cfetbl MyObjectFile ../../TblDefaultImgDir/\n");
    printf("   elf2cfetbl -s12 -p0x0D -a016 -e2000:01:01:00:00:00 MyObjectFile ../../TblDefaultImgDir/\n");
    printf("\n");
    printf("NOTE: The name of the target file is specified within the source file as part of the CFE_TBL_FILEDEF "
           "macro.\n");
    printf("      If the macro has not been included in the source file, the utility will fail to convert the object "
           "file.\n");
}

/**
 *
 */

int32 GetSrcFilename(void)
{
    int32 Status = SUCCESS;

    if (strlen(SrcFilename) == 0)
    {
        OutputHelp = true;
        Status     = FAILED;
    }

    return Status;
}

/**
 *
 */

int32 GetDstFilename(void)
{
    int32 Status = SUCCESS;

    if (strlen(DstFilename) == 0)
    {
        strcpy(DstFilename, "./");
    }

    strcat(DstFilename, TblFileDef.TgtFilename);

    if (Verbose)
        printf("Target Filename: %s\n", DstFilename);

    return Status;
}

/**
 *
 */

int32 OpenSrcFile(void)
{
    int  RtnCode;
    char TimeBuff[50];

    /* Check to see if input file can be found and opened */
    SrcFileDesc = fopen(SrcFilename, "r");

    if (SrcFileDesc == NULL)
    {
        printf("'%s' was not opened\n", SrcFilename);
        return FAILED;
    }

    /* Obtain time of object file's last modification */
    RtnCode = stat(SrcFilename, &SrcFileStats);
    if (RtnCode == 0)
    {
        SrcFileTimeInScEpoch = SrcFileStats.st_mtime + EpochDelta;

        if (Verbose)
        {
            printf("Original Source File Modification Time: %s\n", ctime_r(&SrcFileStats.st_mtime, TimeBuff));
            printf("Source File Modification Time in Seconds since S/C Epoch: %ld (0x%08lX)\n", SrcFileTimeInScEpoch,
                   SrcFileTimeInScEpoch);
        }
    }
    else
    {
        if (Verbose)
            printf("Unable to get modification time from %s", SrcFilename);
        SrcFileTimeInScEpoch = 0;
    }

    return SUCCESS;
}

int32 OpenDstFile(void)
{
    struct stat dststat;

    /* Check to see if output file can be opened and written */
    DstFileDesc = fopen(DstFilename, "w");

    if (DstFileDesc == NULL)
    {
        printf("'%s' was not opened\n", DstFilename);
        return FAILED;
    }

    /* Fix file if too permissive (CWE-732) */
    if (stat(DstFilename, &dststat) == 0)
    {
        if (Verbose)
            printf("%s: Destination file permissions after open = 0x%X\n", DstFilename, dststat.st_mode);
        chmod(DstFilename, dststat.st_mode & ~(S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH));
        stat(DstFilename, &dststat);
        if (Verbose)
            printf("%s: Destination file permissions after chmod = 0x%X\n", DstFilename, dststat.st_mode);
    }

    return SUCCESS;
}

/**
 *
 */

int32 GetElfHeader(void)
{
    int32  Status      = SUCCESS;
    size_t NumHdrsRead = 0;
    char   VerboseStr[60];
    int32  EndiannessCheck = 0x01020304;

    if (((char *)&EndiannessCheck)[0] == 0x01)
    {
        ThisMachineIsLittleEndian = false;
    }
    else if (((char *)&EndiannessCheck)[0] != 0x04)
    {
        printf("Unable to determine endianness of this machine! (0x%02x, 0x%02x, 0x%02x, 0x%02x)\n",
               ((char *)&EndiannessCheck)[0], ((char *)&EndiannessCheck)[1], ((char *)&EndiannessCheck)[2],
               ((char *)&EndiannessCheck)[3]);
        return FAILED;
    }

    /* Begin by reading e_ident characters */
    NumHdrsRead = fread(&ElfHeader, EI_NIDENT, 1, SrcFileDesc);

    if (NumHdrsRead != 1)
    {
        printf("Experienced error attempting to read e_ident of ELF Header from file '%s'\n", SrcFilename);
        return FAILED;
    }

    if (Verbose)
        printf("ELF Header:\n");
    if (Verbose)
        printf("   e_ident[EI_MAG0..3] = 0x%02x,%c%c%c\n", get_e_ident(&ElfHeader, EI_MAG0),
               get_e_ident(&ElfHeader, EI_MAG1), get_e_ident(&ElfHeader, EI_MAG2), get_e_ident(&ElfHeader, EI_MAG3));

    /* Verify the ELF file magic number */
    if (get_e_ident(&ElfHeader, EI_MAG0) != ELFMAG0)
        Status = FAILED;
    if (get_e_ident(&ElfHeader, EI_MAG1) != ELFMAG1)
        Status = FAILED;
    if (get_e_ident(&ElfHeader, EI_MAG2) != ELFMAG2)
        Status = FAILED;
    if (get_e_ident(&ElfHeader, EI_MAG3) != ELFMAG3)
        Status = FAILED;

    if (Status == FAILED)
    {
        printf("Source File '%s' does not have an ELF Magic Number\n", SrcFilename);
        printf("If this object file was compiled on a PC under cygwin, then it is probably in COFF format.\n");
        printf("To convert it to an elf file, use the following command:\n");
        printf("./objcopy -O elf32-little %s %s.elf\n", SrcFilename, SrcFilename);
        printf("then try running this utility again on the %s.elf file\n", SrcFilename);
        return Status;
    }

    /* Verify the processor class type */
    switch (get_e_ident(&ElfHeader, EI_CLASS))
    {
        case ELFCLASSNONE:
            sprintf(VerboseStr, "ELFCLASSNONE (0)");
            Status = FAILED;
            break;

        case ELFCLASS32:
            sprintf(VerboseStr, "ELFCLASS32 (1)");
            if (Verbose)
                printf("Target table is 32 bit\n");
            TargetWordsizeIs32Bit = true;
            break;

        case ELFCLASS64:
            sprintf(VerboseStr, "ELFCLASS64 (2)");
            if (Verbose)
                printf("Target table is 64 bit\n");
            TargetWordsizeIs32Bit = false;
            break;

        default:
            sprintf(VerboseStr, "Invalid Class (%d)", get_e_ident(&ElfHeader, EI_CLASS));
            Status = FAILED;
            break;
    }

    if (Status == FAILED)
    {
        printf("Source file '%s' contains objects of class type '%s' which is unsupported by this utility\n",
               SrcFilename, VerboseStr);
        return Status;
    }

    if (Verbose)
        printf("   e_ident[EI_CLASS] = %s\n", VerboseStr);

    /* Verify Data Encoding type */
    switch (get_e_ident(&ElfHeader, EI_DATA))
    {
        case ELFDATANONE:
            sprintf(VerboseStr, "ELFDATANONE");
            Status = FAILED;
            break;

        case ELFDATA2LSB:
            sprintf(VerboseStr, "ELFDATA2LSB (Little-Endian)");
            TargetMachineIsLittleEndian = true;
            if (ThisMachineIsLittleEndian == false)
            {
                ByteSwapRequired = true;
            }
            break;

        case ELFDATA2MSB:
            sprintf(VerboseStr, "ELFDATA2MSB (Big-Endian)");
            TargetMachineIsLittleEndian = false;
            if (ThisMachineIsLittleEndian == true)
            {
                ByteSwapRequired = true;
            }
            break;

        default:
            sprintf(VerboseStr, "Unknown Data Encoding Type (%d)", get_e_ident(&ElfHeader, EI_DATA));
            Status = FAILED;
            break;
    }

    if (Status == FAILED)
    {
        printf("Source file '%s' contains data encoded with '%s'\n", SrcFilename, VerboseStr);
        return Status;
    }

    if (Verbose)
        printf("   e_ident[EI_DATA] = %s\n", VerboseStr);

    /* Verify ELF Header Version */
    if (get_e_ident(&ElfHeader, EI_VERSION) != EV_CURRENT)
    {
        printf("Source file '%s' is improper ELF header version (%d)\n", SrcFilename,
               get_e_ident(&ElfHeader, EI_VERSION));
        return FAILED;
    }

    if (Verbose)
        printf("   e_ident[EI_VERSION] = %d\n", get_e_ident(&ElfHeader, EI_VERSION));

    /* Now that e_ident is processed (with word size), read rest of the header */
    if (TargetWordsizeIs32Bit)
    {
        NumHdrsRead = fread(&(ElfHeader.Ehdr32.e_type), sizeof(Elf32_Ehdr) - EI_NIDENT, 1, SrcFileDesc);
    }
    else
    {
        NumHdrsRead = fread(&(ElfHeader.Ehdr64.e_type), sizeof(Elf64_Ehdr) - EI_NIDENT, 1, SrcFileDesc);
    }

    if (NumHdrsRead != 1)
    {
        printf("Experienced error attempting to read remaining ELF Header from file '%s'\n", SrcFilename);
        return FAILED;
    }

    if (ByteSwapRequired == true)
    {
        SwapElfHeader();
    }

    /* Verify ELF Type */
    Status = GetStringFromMap(&VerboseStr[0], e_type_Map, (int32)get_e_type(&ElfHeader));

    if (Status == FAILED)
    {
        printf("Error in source file '%s' - %s\n", SrcFilename, VerboseStr);
        return FAILED;
    }

    if (Verbose)
        printf("   e_type = %s\n", VerboseStr);

    /* Verify machine type */
    Status = GetStringFromMap(&VerboseStr[0], e_machine_Map, (int32)get_e_machine(&ElfHeader));

    if (Status == FAILED)
    {
        printf("Error in source file '%s' - %s\n", SrcFilename, VerboseStr);
        return FAILED;
    }

    if (Verbose)
        printf("   e_machine = %s\n", VerboseStr);

    /* Verify ELF Object File Version */
    if (get_e_version(&ElfHeader) != EV_CURRENT)
    {
        printf("Error in source file '%s' - Improper ELF object version (%d)\n", SrcFilename,
               get_e_version(&ElfHeader));
        return FAILED;
    }

    if (TargetWordsizeIs32Bit)
    {
        PrintElfHeader32(ElfHeader);
    }
    else
    {
        PrintElfHeader64(ElfHeader);
    }
    return Status;
}

/**
 *
 */

int32 GetSectionHeader(int32 SectionIndex, union Elf_Shdr *SectionHeader)
{
    int32  Status      = SUCCESS;
    size_t NumHdrsRead = 0;
    char   VerboseStr[60];
    int64  SeekOffset;
    int32  Shentsize;
    int32  i = 0;

    if (TargetWordsizeIs32Bit)
    {
        SeekOffset = ElfHeader.Ehdr32.e_shoff;
        Shentsize  = ElfHeader.Ehdr32.e_shentsize;
    }
    else
    {
        SeekOffset = ElfHeader.Ehdr64.e_shoff;
        Shentsize  = ElfHeader.Ehdr64.e_shentsize;
    }

    if (SectionIndex > 0)
    {
        SeekOffset = SeekOffset + (SectionIndex * Shentsize);
    }

    Status = fseek(SrcFileDesc, SeekOffset, SEEK_SET);

    if (Status != 0)
    {
        printf("Error locating Section Header #%d in file '%s'\n", SectionIndex, SrcFilename);
        return FAILED;
    }

    if (TargetWordsizeIs32Bit)
    {
        NumHdrsRead = fread(SectionHeader, sizeof(Elf32_Shdr), 1, SrcFileDesc);
    }
    else
    {
        NumHdrsRead = fread(SectionHeader, sizeof(Elf64_Shdr), 1, SrcFileDesc);
    }

    if (NumHdrsRead != 1)
    {
        printf("Experienced error attempting to read Section Header #%d from file '%s'\n", SectionIndex, SrcFilename);
        return FAILED;
    }

    if (ByteSwapRequired == true)
        SwapSectionHeader(SectionHeader);

    if ((SectionHeaderStringTableDataOffset != 0) && (get_sh_name(SectionHeader) != 0))
    {
        if (Verbose)
            printf("Section Header #%d:\n", SectionIndex);

        SeekOffset = SectionHeaderStringTableDataOffset + get_sh_name(SectionHeader);
        if (Verbose)
            printf("   sh_name       = 0x%08x - ", get_sh_name(SectionHeader));
        fseek(SrcFileDesc, SeekOffset, SEEK_SET);

        while ((VerboseStr[i] = fgetc(SrcFileDesc)) != '\0')
        {
            i++;
        }
        if (Verbose)
            printf("%s\n", VerboseStr);

        /* Save the name for later reference */
        strncpy(SectionNamePtrs[SectionIndex], VerboseStr, (MAX_SECTION_HDR_NAME_LEN - 1));
        SectionNamePtrs[SectionIndex][(MAX_SECTION_HDR_NAME_LEN - 1)] = '\0';

        switch (get_sh_type(SectionHeader))
        {
            case SHT_NULL:
                sprintf(VerboseStr, "SHT_NULL (0)");
                break;

            case SHT_PROGBITS:
                sprintf(VerboseStr, "SHT_PROGBITS (1)");
                break;

            case SHT_SYMTAB:
                if (TargetWordsizeIs32Bit)
                {
                    SymbolTableDataOffset = SectionHeader->Shdr32.sh_offset + sizeof(Elf32_Sym);
                }
                else
                {
                    SymbolTableDataOffset = SectionHeader->Shdr64.sh_offset + sizeof(Elf64_Sym);
                }
                SymbolTableEntrySize = get_sh_entsize(SectionHeader);
                if (SymbolTableEntrySize == 0)
                {
                    NumSymbols = 0;
                }
                else
                {
                    NumSymbols = (get_sh_size(SectionHeader) / SymbolTableEntrySize) - 1;
                }
                sprintf(VerboseStr, "SHT_SYMTAB (2) - # Symbols = %lu", (long unsigned int)NumSymbols);
                break;

            case SHT_STRTAB:
                sprintf(VerboseStr, "SHT_STRTAB (3)");
                /*
                 * If the section name is ".strtab" then preferentially use this section for symbol name data
                 * Otherwise use the first section which is NOT the section header string table (.shstrtab)
                 *
                 * Not all compilers generate a separate strtab for section header names; some put everything
                 * into one string table.
                 */
                if (strcmp(SectionNamePtrs[SectionIndex], ".strtab") == 0 ||
                    (StringTableDataOffset == 0 && SectionIndex != get_e_shstrndx(&ElfHeader)))
                {
                    StringTableDataOffset = get_sh_offset(SectionHeader);
                }
                break;

            case SHT_RELA:
                sprintf(VerboseStr, "SHT_RELA (4)");
                break;

            case SHT_HASH:
                sprintf(VerboseStr, "SHT_HASH (5)");
                break;

            case SHT_DYNAMIC:
                sprintf(VerboseStr, "SHT_DYNAMIC (6)");
                break;

            case SHT_NOTE:
                sprintf(VerboseStr, "SHT_NOTE (7)");
                break;

            case SHT_NOBITS:
                sprintf(VerboseStr, "SHT_NOBITS (8)");
                break;

            case SHT_REL:
                sprintf(VerboseStr, "SHT_REL (9)");
                break;

            case SHT_SHLIB:
                sprintf(VerboseStr, "SHT_SHLIB (10)");
                break;

            case SHT_DYNSYM:
                sprintf(VerboseStr, "SHT_DYNSYM (11)");
                break;

            case SHT_INIT_ARRAY:
                sprintf(VerboseStr, "SHT_INIT_ARRAY (14)");
                break;

            case SHT_FINI_ARRAY:
                sprintf(VerboseStr, "SHT_FINI_ARRAY (15)");
                break;

            case SHT_PREINIT_ARRAY:
                sprintf(VerboseStr, "SHT_PREINIT_ARRAY (16)");
                break;

            case SHT_GROUP:
                sprintf(VerboseStr, "SHT_GROUP (17)");
                break;

            case SHT_SYMTAB_SHNDX:
                sprintf(VerboseStr, "SHT_SYMTAB_SHNDX (18)");
                break;

            default:
                sprintf(VerboseStr, "Unknown (%d)", get_sh_type(SectionHeader));
                break;
        }

        if (Verbose)
            printf("   sh_type       = %s\n", VerboseStr);

        if (Verbose)
            print_sh_flags(SectionHeader);

        if (TargetWordsizeIs32Bit)
        {
            PrintSectionHeader32(SectionHeader);
        }
        else
        {
            PrintSectionHeader64(SectionHeader);
        }
    }

    return Status;
}

/**
 *
 */

int32 GetSymbol(int32 SymbolIndex, union Elf_Sym *Symbol)
{
    int32    Status            = SUCCESS;
    int32    NumSymRead        = 0;
    uint64_t calculated_offset = SymbolTableDataOffset + (SymbolIndex * SymbolTableEntrySize);
    int32_t  SeekOffset        = (int32_t)calculated_offset;
    char     VerboseStr[60];
    int32    i = 0;
    if (SeekOffset != calculated_offset)
    {
        printf("Error: SeekOffset may not be %lu\n", (long unsigned int)calculated_offset);
        Status = FAILED;
    }
    else
    {
        Status = fseek(SrcFileDesc, SeekOffset, SEEK_SET);
    }

    if (Status != 0)
    {
        printf("Error locating Symbol #%d in file '%s'\n", SymbolIndex, SrcFilename);
        return FAILED;
    }

    if (TargetWordsizeIs32Bit)
    {
        NumSymRead = fread(Symbol, sizeof(Elf32_Sym), 1, SrcFileDesc);
    }
    else
    {
        NumSymRead = fread(Symbol, sizeof(Elf64_Sym), 1, SrcFileDesc);
    }

    if (NumSymRead != 1)
    {
        printf("Experienced error attempting to read Symbol #%d from file '%s'\n", SymbolIndex, SrcFilename);
        return FAILED;
    }

    if (ByteSwapRequired)
        SwapSymbol(Symbol);

    if (Verbose)
        printf("Symbol #%d:\n", (SymbolIndex + 1));

    SeekOffset = StringTableDataOffset + get_st_name(Symbol);
    if (Verbose)
        printf("   st_name  = 0x%08x - ", get_st_name(Symbol));
    fseek(SrcFileDesc, SeekOffset, SEEK_SET);

    while ((i < sizeof(VerboseStr)) && ((VerboseStr[i] = fgetc(SrcFileDesc)) != '\0'))
    {
        i++;
    }

    VerboseStr[i] = '\0'; /* Just in case i=sizeof(VerboseStr) */

    SymbolNames[SymbolIndex] = malloc(i + 1);
    strcpy(SymbolNames[SymbolIndex], VerboseStr);

    if ((strcmp(VerboseStr, TBL_DEF_SYMBOL_NAME) == 0) || (strcmp(&VerboseStr[1], TBL_DEF_SYMBOL_NAME) == 0))
    {
        if (Verbose)
            printf("*** %s ***\n", SymbolNames[SymbolIndex]);
        TblDefSymbolIndex = SymbolIndex;
    }
    else
    {
        if (Verbose)
            printf("%s\n", SymbolNames[SymbolIndex]);
    }

    if (TargetWordsizeIs32Bit)
    {
        PrintSymbol32(Symbol);
    }
    else
    {
        PrintSymbol64(Symbol);
    }

    return Status;
}

void PrintSymbol32(union Elf_Sym *Symbol)
{
    if (Verbose)
        printf("   st_value = 0x%x\n", Symbol->Sym32.st_value);
    if (Verbose)
        printf("   st_size  = 0x%08x\n", Symbol->Sym32.st_size);
    if (Verbose)
        printf("   st_info  = 0x%02x\n", Symbol->Sym32.st_info);
    if (Verbose)
        printf("   st_other = 0x%02x\n", Symbol->Sym32.st_other);
    if (Verbose)
        printf("   st_shndx = 0x%04x\n", Symbol->Sym32.st_shndx);
}

void PrintSymbol64(union Elf_Sym *Symbol)
{
    if (Verbose)
        printf("   st_value = 0x%lx\n", (long unsigned int)Symbol->Sym64.st_value);
    if (Verbose)
        printf("   st_size  = 0x%08lx\n", (long unsigned int)Symbol->Sym64.st_size);
    if (Verbose)
        printf("   st_info  = 0x%02x\n", Symbol->Sym64.st_info);
    if (Verbose)
        printf("   st_other = 0x%02x\n", Symbol->Sym64.st_other);
    if (Verbose)
        printf("   st_shndx = 0x%04x\n", Symbol->Sym64.st_shndx);
}

void PrintSectionHeader32(union Elf_Shdr *SectionHeader)
{
    if (Verbose)
        printf("   sh_addr       = 0x%x\n", SectionHeader->Shdr32.sh_addr);
    if (Verbose)
        printf("   sh_offset     = 0x%08x\n", SectionHeader->Shdr32.sh_offset);
    if (Verbose)
        printf("   sh_size       = 0x%08x\n", SectionHeader->Shdr32.sh_size);
    if (Verbose)
        printf("   sh_link       = 0x%08x\n", SectionHeader->Shdr32.sh_link);
    if (Verbose)
        printf("   sh_info       = 0x%08x\n", SectionHeader->Shdr32.sh_info);
    if (Verbose)
        printf("   sh_addralign  = 0x%08x\n", SectionHeader->Shdr32.sh_addralign);
    if (Verbose)
        printf("   sh_entsize    = 0x%08x\n", SectionHeader->Shdr32.sh_entsize);
}

void PrintSectionHeader64(union Elf_Shdr *SectionHeader)
{
    if (Verbose)
        printf("   sh_addr       = 0x%lx\n", (long unsigned int)SectionHeader->Shdr64.sh_addr);
    if (Verbose)
        printf("   sh_offset     = 0x%08lx\n", (long unsigned int)SectionHeader->Shdr64.sh_offset);
    if (Verbose)
        printf("   sh_size       = 0x%08lx\n", (long unsigned int)SectionHeader->Shdr64.sh_size);
    if (Verbose)
        printf("   sh_link       = 0x%08x\n", SectionHeader->Shdr64.sh_link);
    if (Verbose)
        printf("   sh_info       = 0x%08x\n", SectionHeader->Shdr64.sh_info);
    if (Verbose)
        printf("   sh_addralign  = 0x%08lx\n", (long unsigned int)SectionHeader->Shdr64.sh_addralign);
    if (Verbose)
        printf("   sh_entsize    = 0x%08lx\n", (long unsigned int)SectionHeader->Shdr64.sh_entsize);
}

void PrintElfHeader32(union Elf_Ehdr ElfHeaderLcl)
{
    if (Verbose)
        printf("   e_version = %d\n", get_e_version(&ElfHeaderLcl));
    if (Verbose)
        printf("   e_entry = 0x%x\n", ElfHeaderLcl.Ehdr32.e_entry);
    if (Verbose)
        printf("   e_phoff = 0x%08x (%u)\n", ElfHeaderLcl.Ehdr32.e_phoff, ElfHeaderLcl.Ehdr32.e_phoff);
    if (Verbose)
        printf("   e_shoff = 0x%08x (%u)\n", ElfHeaderLcl.Ehdr32.e_shoff, ElfHeaderLcl.Ehdr32.e_shoff);
    if (Verbose)
        printf("   e_flags = 0x%08x\n", ElfHeaderLcl.Ehdr32.e_flags);
    if (Verbose)
        printf("   e_ehsize = %d\n", ElfHeaderLcl.Ehdr32.e_ehsize);
    if (Verbose)
        printf("   e_phentsize = %d\n", ElfHeaderLcl.Ehdr32.e_phentsize);
    if (Verbose)
        printf("   e_phnum = %d\n", ElfHeaderLcl.Ehdr32.e_phnum);
    if (Verbose)
        printf("   e_shentsize = %d\n", ElfHeaderLcl.Ehdr32.e_shentsize);
    if (Verbose)
        printf("   e_shnum = %d\n", get_e_shnum(&ElfHeaderLcl));
    if (Verbose)
        printf("   e_shstrndx = %d\n", get_e_shstrndx(&ElfHeaderLcl));
}

void PrintElfHeader64(union Elf_Ehdr ElfHeaderLcl)
{
    if (Verbose)
        printf("   e_version = %d\n", get_e_version(&ElfHeaderLcl));
    if (Verbose)
        printf("   e_entry = 0x%lx\n", (long unsigned int)ElfHeaderLcl.Ehdr64.e_entry);
    if (Verbose)
        printf("   e_phoff = 0x%08lx (%lu)\n", (long unsigned int)ElfHeaderLcl.Ehdr64.e_phoff,
               (long unsigned int)ElfHeaderLcl.Ehdr64.e_phoff);
    if (Verbose)
        printf("   e_shoff = 0x%08lx (%lu)\n", (long unsigned int)ElfHeaderLcl.Ehdr64.e_shoff,
               (long unsigned int)ElfHeaderLcl.Ehdr64.e_shoff);
    if (Verbose)
        printf("   e_flags = 0x%08x\n", ElfHeaderLcl.Ehdr64.e_flags);
    if (Verbose)
        printf("   e_ehsize = %d\n", ElfHeaderLcl.Ehdr64.e_ehsize);
    if (Verbose)
        printf("   e_phentsize = %d\n", ElfHeaderLcl.Ehdr64.e_phentsize);
    if (Verbose)
        printf("   e_phnum = %d\n", ElfHeaderLcl.Ehdr64.e_phnum);
    if (Verbose)
        printf("   e_shentsize = %d\n", ElfHeaderLcl.Ehdr64.e_shentsize);
    if (Verbose)
        printf("   e_shnum = %d\n", get_e_shnum(&ElfHeaderLcl));
    if (Verbose)
        printf("   e_shstrndx = %d\n", get_e_shstrndx(&ElfHeaderLcl));
}

void SwapElfHeader(void)
{
    if (TargetWordsizeIs32Bit)
    {
        SwapUInt16(&ElfHeader.Ehdr32.e_type);
        SwapUInt16(&ElfHeader.Ehdr32.e_machine);
        SwapUInt32((uint32 *)&ElfHeader.Ehdr32.e_version);
        SwapUInt32((uint32 *)&ElfHeader.Ehdr32.e_entry);
        SwapUInt32((uint32 *)&ElfHeader.Ehdr32.e_phoff);
        SwapUInt32((uint32 *)&ElfHeader.Ehdr32.e_shoff);
        SwapUInt32((uint32 *)&ElfHeader.Ehdr32.e_flags);
        SwapUInt16(&ElfHeader.Ehdr32.e_ehsize);
        SwapUInt16(&ElfHeader.Ehdr32.e_phentsize);
        SwapUInt16(&ElfHeader.Ehdr32.e_phnum);
        SwapUInt16(&ElfHeader.Ehdr32.e_shentsize);
        SwapUInt16(&ElfHeader.Ehdr32.e_shnum);
        SwapUInt16(&ElfHeader.Ehdr32.e_shstrndx);
    }
    else
    {
        SwapUInt16(&ElfHeader.Ehdr64.e_type);
        SwapUInt16(&ElfHeader.Ehdr64.e_machine);
        SwapUInt32((uint32 *)&ElfHeader.Ehdr64.e_version);
        SwapUInt64((uint64 *)&ElfHeader.Ehdr64.e_entry);
        SwapUInt64(&ElfHeader.Ehdr64.e_phoff);
        SwapUInt64(&ElfHeader.Ehdr64.e_shoff);
        SwapUInt32((uint32 *)&ElfHeader.Ehdr64.e_flags);
        SwapUInt16(&ElfHeader.Ehdr64.e_ehsize);
        SwapUInt16(&ElfHeader.Ehdr64.e_phentsize);
        SwapUInt16(&ElfHeader.Ehdr64.e_phnum);
        SwapUInt16(&ElfHeader.Ehdr64.e_shentsize);
        SwapUInt16(&ElfHeader.Ehdr64.e_shnum);
        SwapUInt16(&ElfHeader.Ehdr64.e_shstrndx);
    }
}

/**
 *
 */

void SwapSectionHeader(union Elf_Shdr *SectionHeader)
{
    if (TargetWordsizeIs32Bit)
    {
        SwapUInt32((uint32 *)&(SectionHeader->Shdr32.sh_name));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr32.sh_type));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr32.sh_flags));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr32.sh_addr));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr32.sh_offset));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr32.sh_size));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr32.sh_addralign));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr32.sh_entsize));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr32.sh_link));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr32.sh_info));
    }
    else
    {
        SwapUInt32((uint32 *)&(SectionHeader->Shdr64.sh_name));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr64.sh_type));
        SwapUInt64((uint64 *)&(SectionHeader->Shdr64.sh_flags));
        SwapUInt64((uint64 *)&(SectionHeader->Shdr64.sh_addr));
        SwapUInt64((uint64 *)&(SectionHeader->Shdr64.sh_offset));
        SwapUInt64((uint64 *)&(SectionHeader->Shdr64.sh_size));
        SwapUInt64((uint64 *)&(SectionHeader->Shdr64.sh_addralign));
        SwapUInt64((uint64 *)&(SectionHeader->Shdr64.sh_entsize));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr64.sh_link));
        SwapUInt32((uint32 *)&(SectionHeader->Shdr64.sh_info));
    }
}

/**
 *
 */

void SwapSymbol(union Elf_Sym *Symbol)
{
    if (TargetWordsizeIs32Bit)
    {
        SwapUInt32((uint32 *)&(Symbol->Sym32.st_name));
        SwapUInt32((uint32 *)&(Symbol->Sym32.st_value));
        SwapUInt32((uint32 *)&(Symbol->Sym32.st_size));
        SwapUInt16((uint16 *)&(Symbol->Sym32.st_shndx));
    }
    else
    {
        SwapUInt32((uint32 *)&(Symbol->Sym64.st_name));
        SwapUInt64((uint64 *)&(Symbol->Sym64.st_value));
        SwapUInt64((uint64 *)&(Symbol->Sym64.st_size));
        SwapUInt16((uint16 *)&(Symbol->Sym64.st_shndx));
    }
}

/**
 *
 */

void SwapUInt16(uint16 *ValueToSwap)
{
    uint8 *BytePtr  = (uint8 *)ValueToSwap;
    uint8  TempByte = BytePtr[1];
    BytePtr[1]      = BytePtr[0];
    BytePtr[0]      = TempByte;
}

/**
 *
 */

void SwapUInt32(uint32 *ValueToSwap)
{
    uint8 *BytePtr  = (uint8 *)ValueToSwap;
    uint8  TempByte = BytePtr[3];
    BytePtr[3]      = BytePtr[0];
    BytePtr[0]      = TempByte;
    TempByte        = BytePtr[2];
    BytePtr[2]      = BytePtr[1];
    BytePtr[1]      = TempByte;
}

void SwapUInt64(uint64 *ValueToSwap)
{
    uint8 *BytePtr = (uint8 *)ValueToSwap;
    uint8  TempByte;

    TempByte   = BytePtr[7];
    BytePtr[7] = BytePtr[0];
    BytePtr[0] = TempByte;

    TempByte   = BytePtr[6];
    BytePtr[6] = BytePtr[1];
    BytePtr[1] = TempByte;

    TempByte   = BytePtr[5];
    BytePtr[5] = BytePtr[2];
    BytePtr[2] = TempByte;

    TempByte   = BytePtr[4];
    BytePtr[4] = BytePtr[3];
    BytePtr[3] = TempByte;
}

/**
 *
 */

int32 GetStringFromMap(char *Result, ElfStrMap *Map, int32 Key)
{
    int32 Status = FAILED;

    while ((Map->String[0] != '*') && (Status == FAILED))
    {
        if (Map->Value == Key)
        {
            Status = SUCCESS;
            strcpy(Result, Map->String);
        }
        else
        {
            Map++;
        }
    }

    if (Status == FAILED)
    {
        sprintf(Result, Map->String, Key);
    }

    return Status;
}

/**
 *
 */

int32 GetTblDefInfo(void)
{
    int32  Status      = SUCCESS;
    uint32 SeekOffset  = 0;
    int32  NumDefsRead = 0;

    /* Read the data to be used to format the CFE File and Table Headers */
    if ((get_st_size(SymbolPtrs[TblDefSymbolIndex]) != sizeof(CFE_TBL_FileDef_t)) &&
        (get_st_size(SymbolPtrs[TblDefSymbolIndex]) != 0))
    {
        printf("Error! '%s' is not properly defined in '%s'.  Size of object is incorrect (%lu).\n",
               TBL_DEF_SYMBOL_NAME, SrcFilename, (long unsigned int)get_st_size(SymbolPtrs[TblDefSymbolIndex]));
        Status = FAILED;
    }
    else
    {
        /* fseek expects a long int, sh_offset and st_value are uint64 for elf64 */
        uint64_t calculated_offset = get_sh_offset(SectionHeaderPtrs[get_st_shndx(SymbolPtrs[TblDefSymbolIndex])]) +
                                     get_st_value(SymbolPtrs[TblDefSymbolIndex]);
        SeekOffset = (uint32_t)(calculated_offset);
        if (SeekOffset != calculated_offset)
        {
            printf("Error: SeekOffset may not be %lu\n", (long unsigned int)calculated_offset);
            Status = FAILED;
        }
        fseek(SrcFileDesc, SeekOffset, SEEK_SET);
        NumDefsRead = fread(&TblFileDef, sizeof(CFE_TBL_FileDef_t), 1, SrcFileDesc);

        /* ensuring all are strings are null-terminated */
        TblFileDef.ObjectName[sizeof(TblFileDef.ObjectName) - 1]   = '\0';
        TblFileDef.TableName[sizeof(TblFileDef.TableName) - 1]     = '\0';
        TblFileDef.Description[sizeof(TblFileDef.Description) - 1] = '\0';
        TblFileDef.TgtFilename[sizeof(TblFileDef.TgtFilename) - 1] = '\0';

        if (NumDefsRead != 1)
        {
            printf("Error! Unable to read data content of '%s' from '%s'.\n", TBL_DEF_SYMBOL_NAME, SrcFilename);
            Status = FAILED;
        }

        if (ByteSwapRequired)
            SwapUInt32(&TblFileDef.ObjectSize);

        if (Verbose)
        {
            printf("Table Defined as follows:\n");
            printf("   Data Object: %s\n", TblFileDef.ObjectName);
            printf("   Table Name : '%s'", TblFileDef.TableName);
            if (TableNameOverride == true)
            {
                printf(" overridden with : '%s'", TableName);
            }
            printf("\n");
            printf("   Description: '%s'", TblFileDef.Description);
            if (DescriptionOverride == true)
            {
                printf(" overridden with : '%s'", Description);
            }
            printf("\n");
            printf("   Output File: %s\n", TblFileDef.TgtFilename);
            printf("   Object Size: %d (0x%08x)\n", TblFileDef.ObjectSize, TblFileDef.ObjectSize);
        }
    }

    return Status;
}

/**
 *
 */

int32 LocateAndReadUserObject(void)
{
    int32  Status     = SUCCESS;
    int32  i          = 0;
    int32  j          = 0;
    uint32 SeekOffset = 0;
    uint8  AByte;

    /* Search the symbol table for the user defined object */
    if (Verbose)
        printf("\nTrying to match ObjectName '%s'... (length %lu)", TblFileDef.ObjectName,
               (long unsigned int)strlen(TblFileDef.ObjectName));
    while (i < NumSymbols)
    {
        if (Verbose)
            printf("\nSymbol Search loop %d: SymbolName ='%s' ", i, SymbolNames[i]);
        /* Check to see if the symbol names match as far as the ObjectName is defined */
        if (strncmp(SymbolNames[i], TblFileDef.ObjectName, strlen(TblFileDef.ObjectName)) == 0)
        {
            if (Verbose)
                printf("Found ObjectName '%s' inside SymbolName '%s'\n", TblFileDef.ObjectName, SymbolNames[i]);
            /* Check to see if the Symbol Name happens to have one extra character */
            if ((strlen(SymbolNames[i]) - strlen(TblFileDef.ObjectName)) == 1)
            {
                if (Verbose)
                    printf("Found an extra character...\n");
                /* If the character is non-printable, then we have a match */
                if (isprint(SymbolNames[i][strlen(SymbolNames[i])]) == 0)
                {
                    if (Verbose)
                        printf("...and it's unprintable!");
                    break;
                }
            }
            /* Check to see if the Symbol Name is an exact match */
            else if ((strlen(SymbolNames[i]) - strlen(TblFileDef.ObjectName)) == 0)
            {
                if (Verbose)
                    printf("\nFound an exact match! Symbol='%s' Object='%s'\n", SymbolNames[i], TblFileDef.ObjectName);
                break;
            }
        }

        if (Verbose)
        {
            printf("strstr[%d] = %s; strlenSN = %lu; strlenON = %lu\n", i,
                   strstr(SymbolNames[i], TblFileDef.ObjectName), (long unsigned int)strlen(SymbolNames[i]),
                   (long unsigned int)strlen(TblFileDef.ObjectName));
        }

        i++;
    }

    if (Verbose)
    {
        printf("\ni = %d, NumSymbols = %lu\n", i, (long unsigned int)NumSymbols);
        if (i < NumSymbols)
        {
            printf("\nSymbolName = '%s', ObjectName = '%s'\n", SymbolNames[i], TblFileDef.ObjectName);
            printf("\nSymbolName length = %lu, ObjectName length = %lu\n", (long unsigned int)strlen(SymbolNames[i]),
                   (long unsigned int)strlen(TblFileDef.ObjectName));
        }
    }

    if (i == NumSymbols)
    {
        printf("Error! Unable to find '%s' object in '%s'\n", TblFileDef.ObjectName, SrcFilename);
        Status = FAILED;
    }
    else
    {
        if (Verbose)
            printf("Found '%s' object as Symbol #%d\n", TblFileDef.ObjectName, (i + 1));
        UserObjSymbolIndex = i;

        if (strcmp(SectionNamePtrs[get_st_shndx(SymbolPtrs[UserObjSymbolIndex])], ".bss") == 0)
        {
            if (Verbose)
                printf("Table contents are in '.bss' section and are assumed to be all zeros.\n");
            TableDataIsAllZeros = true;

            if (Verbose)
            {
                printf("Object Data:\n");
                for (i = 0; i < get_st_size(SymbolPtrs[UserObjSymbolIndex]); i++)
                {
                    printf(" 0x%02x", 0);
                    j++;
                    if (j == 16)
                    {
                        printf("\n");
                        j = 0;
                    }
                }
            }
        }
        else
        {
            /* Locate data associated with symbol */
            uint64_t calculated_offset =
                get_sh_offset(SectionHeaderPtrs[get_st_shndx(SymbolPtrs[UserObjSymbolIndex])]) +
                get_st_value(SymbolPtrs[UserObjSymbolIndex]);
            SeekOffset = (uint32_t)(calculated_offset);
            if (SeekOffset != calculated_offset)
            {
                printf("Error: SeekOffset may not be %lu\n", (long unsigned int)calculated_offset);
                Status = FAILED;
            }
            fseek(SrcFileDesc, SeekOffset, SEEK_SET);

            /* Determine if the elf file contained the size of the object */
            if (get_st_size(SymbolPtrs[UserObjSymbolIndex]) != 0)
            {
                /* Check to see if the size in the elf file agrees with the size specified in our table def structure */
                if (get_st_size(SymbolPtrs[UserObjSymbolIndex]) != TblFileDef.ObjectSize)
                {
                    printf("ELF file indicates object '%s' is of size %lu but table definition structure indicates "
                           "size %d",
                           TblFileDef.ObjectName, (long unsigned int)get_st_size(SymbolPtrs[UserObjSymbolIndex]),
                           TblFileDef.ObjectSize);
                    if (TblFileDef.ObjectSize < get_st_size(SymbolPtrs[UserObjSymbolIndex]))
                    {
                        set_st_size(SymbolPtrs[UserObjSymbolIndex], TblFileDef.ObjectSize);
                    }

                    printf("Size of %lu is assumed.\n", (long unsigned int)get_st_size(SymbolPtrs[UserObjSymbolIndex]));
                }
            }
            else
            {
                /* Since the size is not available from the elf file, assume the value in the table def structure is
                 * right */
                set_st_size(SymbolPtrs[UserObjSymbolIndex], TblFileDef.ObjectSize);
            }

            if (Verbose)
            {
                printf("Object Data:\n");
                for (i = 0; i < get_st_size(SymbolPtrs[UserObjSymbolIndex]); i++)
                {
                    AByte = fgetc(SrcFileDesc);
                    printf(" 0x%02x", AByte);
                    j++;
                    if (j == 16)
                    {
                        printf("\n");
                        j = 0;
                    }
                }
                /* Reset the file pointer */
                fseek(SrcFileDesc, SeekOffset, SEEK_SET);
            }
        }
    }

    return Status;
}

/**
 *
 */

int32 OutputDataToTargetFile()
{
    int32 Status = SUCCESS;
    uint8 AByte  = 0;
    int32 i      = 0;

    /* Create the standard header */
    FileHeader.ContentType = 0x63464531;
    FileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    FileHeader.Length      = sizeof(CFE_FS_Header_t);

    if (ScIDSpecified == true)
    {
        FileHeader.SpacecraftID = SpacecraftID;
    }
    else
    {
        FileHeader.SpacecraftID = 0;
    }

    if (ProcIDSpecified == true)
    {
        FileHeader.ProcessorID = ProcessorID;
    }
    else
    {
        FileHeader.ProcessorID = 0;
    }

    if (AppIDSpecified == true)
    {
        FileHeader.ApplicationID = ApplicationID;
    }
    else
    {
        FileHeader.ApplicationID = 0;
    }

    if (EnableTimeTagInHeader)
    {
        FileHeader.TimeSeconds    = SrcFileTimeInScEpoch;
        FileHeader.TimeSubSeconds = 0;
    }
    else
    {
        FileHeader.TimeSeconds    = 0;
        FileHeader.TimeSubSeconds = 0;
    }

    memset(FileHeader.Description, 0, CFE_FS_HDR_DESC_MAX_LEN);

    if (DescriptionOverride == true)
    {
        strcpy(FileHeader.Description, Description);
    }
    else
    {
        strcpy(FileHeader.Description, TblFileDef.Description);
    }

    /* If this machine is little endian, the CFE header must be swapped */
    if (ThisMachineIsLittleEndian == true)
    {
        if (Verbose)
            printf("\ncFE Headers are being byte-swapped because this machine is 'Little Endian'\n");
        SwapUInt32(&FileHeader.ContentType);
        SwapUInt32(&FileHeader.SubType);
        SwapUInt32(&FileHeader.Length);
        SwapUInt32(&FileHeader.SpacecraftID);
        SwapUInt32(&FileHeader.ProcessorID);
        SwapUInt32(&FileHeader.ApplicationID);
        SwapUInt32(&FileHeader.TimeSeconds);
        SwapUInt32(&FileHeader.TimeSubSeconds);
    }

    /* Create the standard cFE Table Header */
    memset(&TableHeader, 0, sizeof(TableHeader));

    TableHeader.NumBytes = (uint32_t)(get_st_size(SymbolPtrs[UserObjSymbolIndex]));
    if (TableHeader.NumBytes != get_st_size(SymbolPtrs[UserObjSymbolIndex]))
    {
        printf("ERROR: TableHeader.NumBytes is too small for Sym64.st_size\n");
        Status = FAILED;
    }

    if (TableNameOverride == true)
    {
        strcpy(TableHeader.TableName, TableName);
    }
    else
    {
        strcpy(TableHeader.TableName, TblFileDef.TableName);
    }

    /* If this machine is little endian, the TBL header must be swapped */
    if (ThisMachineIsLittleEndian == true)
    {
        SwapUInt32(&TableHeader.Reserved);
        SwapUInt32(&TableHeader.Offset);
        SwapUInt32(&TableHeader.NumBytes);
    }

    /* Output the two headers to the target file */
    fwrite(&FileHeader.ContentType, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.SubType, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.Length, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.SpacecraftID, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.ProcessorID, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.ApplicationID, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.TimeSeconds, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.TimeSubSeconds, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.Description[0], sizeof(FileHeader.Description), 1, DstFileDesc);

    fwrite(&TableHeader.Reserved, sizeof(uint32), 1, DstFileDesc);
    fwrite(&TableHeader.Offset, sizeof(uint32), 1, DstFileDesc);
    fwrite(&TableHeader.NumBytes, sizeof(uint32), 1, DstFileDesc);
    fwrite(&TableHeader.TableName[0], sizeof(TableHeader.TableName), 1, DstFileDesc);

    /* Output the data from the object file */
    if (TableDataIsAllZeros)
    {
        AByte = 0;
        for (i = 0; i < get_st_size(SymbolPtrs[UserObjSymbolIndex]); i++)
        {
            fwrite(&AByte, 1, 1, DstFileDesc);
        }
    }
    else
    {
        for (i = 0; i < get_st_size(SymbolPtrs[UserObjSymbolIndex]); i++)
        {
            AByte = fgetc(SrcFileDesc);
            fwrite(&AByte, 1, 1, DstFileDesc);
        }
    }

    return Status;
}
