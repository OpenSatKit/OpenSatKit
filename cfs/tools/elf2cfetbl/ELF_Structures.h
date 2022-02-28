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
*/
#ifndef ELF_STRUCTURES_H
#define ELF_STRUCTURES_H

#include <stdint.h>

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t  Elf32_Sword;
typedef uint32_t Elf32_Word;

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t  Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;

#define EI_NIDENT 16 /**< \brief Size of e_ident[]       */

/**
 *    Elf 32 bit Header Format
 */
typedef struct
{
    unsigned char e_ident[EI_NIDENT]; /**< \brief Machine independent data to allow decoding of file */
    Elf32_Half    e_type;             /**< \brief Identifies object file type */
    Elf32_Half    e_machine;          /**< \brief Specifies required architecture for file */
    Elf32_Word    e_version;          /**< \brief Object file version */
    Elf32_Addr    e_entry;            /**< \brief Virtual start address for process */
    Elf32_Off     e_phoff;            /**< \brief File offset to beginning of Program Header Table */
    Elf32_Off     e_shoff;            /**< \brief File offset to beginning of Section Header Table */
    Elf32_Word    e_flags;            /**< \brief Processor specific flags */
    Elf32_Half    e_ehsize;           /**< \brief ELF Header's size, in bytes */
    Elf32_Half    e_phentsize;        /**< \brief Size, in bytes, of each Program Header Table Entry */
    Elf32_Half    e_phnum;            /**< \brief Number of entries the Program Header Table contains */
    Elf32_Half    e_shentsize;        /**< \brief Size, in bytes, of each Section Header Table Entry */
    Elf32_Half    e_shnum;            /**< \brief Number of entries the Section Header Table contains */
    Elf32_Half    e_shstrndx;         /**< \brief Section Header Table index for the Section Name String Table */
} Elf32_Ehdr;

/**
 *    Elf 64 bit Header Format
 */
typedef struct
{
    unsigned char e_ident[EI_NIDENT]; /**< \brief Machine independent data to allow decoding of file */
    Elf64_Half    e_type;             /**< \brief Identifies object file type */
    Elf64_Half    e_machine;          /**< \brief Specifies required architecture for file */
    Elf64_Word    e_version;          /**< \brief Object file version */
    Elf64_Addr    e_entry;            /**< \brief Virtual start address for process */
    Elf64_Off     e_phoff;            /**< \brief File offset to beginning of Program Header Table */
    Elf64_Off     e_shoff;            /**< \brief File offset to beginning of Section Header Table */
    Elf64_Word    e_flags;            /**< \brief Processor specific flags */
    Elf64_Half    e_ehsize;           /**< \brief ELF Header's size, in bytes */
    Elf64_Half    e_phentsize;        /**< \brief Size, in bytes, of each Program Header Table Entry */
    Elf64_Half    e_phnum;            /**< \brief Number of entries the Program Header Table contains */
    Elf64_Half    e_shentsize;        /**< \brief Size, in bytes, of each Section Header Table Entry */
    Elf64_Half    e_shnum;            /**< \brief Number of entries the Section Header Table contains */
    Elf64_Half    e_shstrndx;         /**< \brief Section Header Table index for the Section Name String Table */
} Elf64_Ehdr;

union Elf_Ehdr
{
    Elf32_Ehdr Ehdr32;
    Elf64_Ehdr Ehdr64;
};

/**
 *    e_type values are as follows:
 */

#define ET_NONE   0      /**< \brief No file type        */
#define ET_REL    1      /**< \brief Relocatable file    */
#define ET_EXEC   2      /**< \brief Executable file     */
#define ET_DYN    3      /**< \brief Shared object file  */
#define ET_CORE   4      /**< \brief Core file           */
#define ET_LOPROC 0xff00 /**< \brief Processor-specific  */
#define ET_HIPROC 0xffff /**< \brief Processor-specific  */

/**
 *   e_machine values are as follows:
 */
#define EM_NONE          0   /**< \brief No machine                                                      */
#define EM_M32           1   /**< \brief AT&T WE 32100                                                   */
#define EM_SPARC         2   /**< \brief SPARC                                                           */
#define EM_386           3   /**< \brief Intel 80386                                                     */
#define EM_68K           4   /**< \brief Motorola 68000                                                  */
#define EM_88K           5   /**< \brief Motorola 88000                                                  */
#define EM_860           7   /**< \brief Intel 80860                                                     */
#define EM_MIPS          8   /**< \brief MIPS RS3000                                                     */
#define EM_S370          9   /**< \brief IBM System/370 Processor                                        */
#define EM_MIPS_RS3_LE   10  /**< \brief MIPS RS3000 Little-endian                                       */
#define EM_PARISC        15  /**< \brief Hewlett-Packard PA-RISC                                         */
#define EM_VPP500        17  /**< \brief Fujitsu VPP500                                                  */
#define EM_SPARC32PLUS   18  /**< \brief Enhanced instruction set SPARC                                  */
#define EM_960           19  /**< \brief Intel 80960                                                     */
#define EM_PPC           20  /**< \brief PowerPC                                                         */
#define EM_PPC64         21  /**< \brief 64-bit PowerPC                                                  */
#define EM_S390          22  /**< \brief IBM System/390 Processor                                        */
#define EM_SPU           23  /**< \brief IBM SPU/SPC                                                     */
#define EM_V800          36  /**< \brief NEC V800                                                        */
#define EM_FR20          37  /**< \brief Fujitsu FR20                                                    */
#define EM_RH32          38  /**< \brief TRW RH-32                                                       */
#define EM_RCE           39  /**< \brief Motorola RCE                                                    */
#define EM_ARM           40  /**< \brief Advanced RISC Machines ARM                                      */
#define EM_ALPHA         41  /**< \brief Digital Alpha                                                   */
#define EM_SH            42  /**< \brief Hitachi SH                                                      */
#define EM_SPARCV9       43  /**< \brief SPARC Version 9                                                 */
#define EM_TRICORE       44  /**< \brief Siemens Tricore embedded processor                              */
#define EM_ARC           45  /**< \brief Argonaut RISC Core, Argonaut Technologies Inc.                  */
#define EM_H8_300        46  /**< \brief Hitachi H8/300                                                  */
#define EM_H8_300H       47  /**< \brief Hitachi H8/300H                                                 */
#define EM_H8S           48  /**< \brief Hitachi H8S                                                     */
#define EM_H8_500        49  /**< \brief Hitachi H8/500                                                  */
#define EM_IA_64         50  /**< \brief Intel IA-64 processor architecture                              */
#define EM_MIPS_X        51  /**< \brief Stanford MIPS-X                                                 */
#define EM_COLDFIRE      52  /**< \brief Motorola ColdFire                                               */
#define EM_68HC12        53  /**< \brief Motorola M68HC12                                                */
#define EM_MMA           54  /**< \brief Fujitsu MMA Multimedia Accelerator                              */
#define EM_PCP           55  /**< \brief Siemens PCP                                                     */
#define EM_NCPU          56  /**< \brief Sony nCPU embedded RISC processor                               */
#define EM_NDR1          57  /**< \brief Denso NDR1 microprocessor                                       */
#define EM_STARCORE      58  /**< \brief Motorola Star*Core processor                                    */
#define EM_ME16          59  /**< \brief Toyota ME16 processor                                           */
#define EM_ST100         60  /**< \brief STMicroelectronics ST100 processor                              */
#define EM_TINYJ         61  /**< \brief Advanced Logic Corp. TinyJ embedded processor family            */
#define EM_X86_64        62  /**< \brief AMD x86-64 architecture                                         */
#define EM_PDSP          63  /**< \brief Sony DSP Processor                                              */
#define EM_PDP10         64  /**< \brief Digital Equipment Corp. PDP-10                                  */
#define EM_PDP11         65  /**< \brief Digital Equipment Corp. PDP-11                                  */
#define EM_FX66          66  /**< \brief Siemens FX66 microcontroller                                    */
#define EM_ST9PLUS       67  /**< \brief STMicroelectronics ST9+ 8/16 bit microcontroller                */
#define EM_ST7           68  /**< \brief STMicroelectronics ST7 8-bit microcontroller                    */
#define EM_68HC16        69  /**< \brief Motorola MC68HC16 Microcontroller                               */
#define EM_68HC11        70  /**< \brief Motorola MC68HC11 Microcontroller                               */
#define EM_68HC08        71  /**< \brief Motorola MC68HC08 Microcontroller                               */
#define EM_68HC05        72  /**< \brief Motorola MC68HC05 Microcontroller                               */
#define EM_SVX           73  /**< \brief Silicon Graphics SVx                                            */
#define EM_ST19          74  /**< \brief STMicroelectronics ST19 8-bit microcontroller                   */
#define EM_VAX           75  /**< \brief Digital VAX                                                     */
#define EM_CRIS          76  /**< \brief Axis Communications 32-bit embedded processor                   */
#define EM_JAVELIN       77  /**< \brief Infineon Technologies 32-bit embedded processor                 */
#define EM_FIREPATH      78  /**< \brief Element 14 64-bit DSP Processor                                 */
#define EM_ZSP           79  /**< \brief LSI Logic 16-bit DSP Processor                                  */
#define EM_MMIX          80  /**< \brief Donald Knuth's educational 64-bit processor                     */
#define EM_HUANY         81  /**< \brief Harvard University machine-independent object files             */
#define EM_PRISM         82  /**< \brief SiTera Prism                                                    */
#define EM_AVR           83  /**< \brief Atmel AVR 8-bit microcontroller                                 */
#define EM_FR30          84  /**< \brief Fujitsu FR30                                                    */
#define EM_D10V          85  /**< \brief Mitsubishi D10V                                                 */
#define EM_D30V          86  /**< \brief Mitsubishi D30V                                                 */
#define EM_V850          87  /**< \brief NEC v850                                                        */
#define EM_M32R          88  /**< \brief Mitsubishi M32R                                                 */
#define EM_MN10300       89  /**< \brief Matsushita MN10300                                              */
#define EM_MN10200       90  /**< \brief Matsushita MN10200                                              */
#define EM_PJ            91  /**< \brief picoJava                                                        */
#define EM_OPENRISC      92  /**< \brief OpenRISC 32-bit embedded processor                              */
#define EM_ARC_COMPACT   93  /**< \brief ARC International ARCompact processor (old spelling/synonym: EM_ARC_A5) */
#define EM_XTENSA        94  /**< \brief Tensilica Xtensa Architecture                                   */
#define EM_VIDEOCORE     95  /**< \brief Alphamosaic VideoCore processor                                 */
#define EM_TMM_GPP       96  /**< \brief Thompson Multimedia General Purpose Processor                   */
#define EM_NS32K         97  /**< \brief National Semiconductor 32000 series                             */
#define EM_TPC           98  /**< \brief Tenor Network TPC processor                                     */
#define EM_SNP1K         99  /**< \brief Trebia SNP 1000 processor                                       */
#define EM_ST200         100 /**< \brief STMicroelectronics (www.st.com) ST200 microcontroller           */
#define EM_IP2K          101 /**< \brief Ubicom IP2xxx microcontroller family                            */
#define EM_MAX           102 /**< \brief MAX Processor                                                   */
#define EM_CR            103 /**< \brief National Semiconductor CompactRISC microprocessor               */
#define EM_F2MC16        104 /**< \brief Fujitsu F2MC16                                                  */
#define EM_MSP430        105 /**< \brief Texas Instruments embedded microcontroller msp430               */
#define EM_BLACKFIN      106 /**< \brief Analog Devices Blackfin (DSP) processor                         */
#define EM_SE_C33        107 /**< \brief S1C33 Family of Seiko Epson processors                          */
#define EM_SEP           108 /**< \brief Sharp embedded microprocessor                                   */
#define EM_ARCA          109 /**< \brief Arca RISC Microprocessor                                        */
#define EM_UNICORE       110 /**< \brief Microprocessor series from PKU-Unity Ltd. and MPRC of Peking University */
#define EM_EXCESS        111 /**< \brief eXcess: 16/32/64-bit configurable embedded CPU                  */
#define EM_DXP           112 /**< \brief Icera Semiconductor Inc. Deep Execution Processor               */
#define EM_ALTERA_NIOS2  113 /**< \brief Altera Nios II soft-core processor                            */
#define EM_CRX           114 /**< \brief National Semiconductor CompactRISC CRX microprocessor           */
#define EM_XGATE         115 /**< \brief Motorola XGATE embedded processor                               */
#define EM_C166          116 /**< \brief Infineon C16x/XC16x processor                                   */
#define EM_M16C          117 /**< \brief Renesas M16C series microprocessors                             */
#define EM_DSPIC30F      118 /**< \brief Microchip Technology dsPIC30F Digital Signal Controller         */
#define EM_CE            119 /**< \brief Freescale Communication Engine RISC core                        */
#define EM_M32C          120 /**< \brief Renesas M32C series microprocessors                             */
#define EM_TSK3000       131 /**< \brief Altium TSK3000 core                                             */
#define EM_RS08          132 /**< \brief Freescale RS08 embedded processor                               */
#define EM_SHARC         133 /**< \brief Analog Devices SHARC family of 32-bit DSP processors            */
#define EM_ECOG2         134 /**< \brief Cyan Technology eCOG2 microprocessor                            */
#define EM_SCORE7        135 /**< \brief Sunplus S+core7 RISC processor                                  */
#define EM_DSP24         136 /**< \brief New Japan Radio (NJR) 24-bit DSP Processor                      */
#define EM_VIDEOCORE3    137 /**< \brief Broadcom VideoCore III processor                                */
#define EM_LATTICEMICO32 138 /**< \brief RISC processor for Lattice FPGA architecture                 */
#define EM_SE_C17        139 /**< \brief Seiko Epson C17 family                                          */
#define EM_TI_C6000      140 /**< \brief The Texas Instruments TMS320C6000 DSP family                    */
#define EM_TI_C2000      141 /**< \brief The Texas Instruments TMS320C2000 DSP family                    */
#define EM_TI_C5500      142 /**< \brief The Texas Instruments TMS320C55x DSP family                     */
#define EM_TI_ARP32      143 /**< \brief Texas Instruments Application Specific RISC Processor, 32bit fetch */
#define EM_TI_PRU        144 /**< \brief Texas Instruments Programmable Realtime Unit                    */
#define EM_MMDSP_PLUS    160 /**< \brief EM_MMDSP_PLUS 160 STMicroelectronics 64bit VLIW Data Signal Processor */
#define EM_CYPRESS_M8C   161 /**< \brief Cypress M8C microprocessor                                     */
#define EM_R32C          162 /**< \brief Renesas R32C series microprocessors                             */
#define EM_TRIMEDIA      163 /**< \brief NXP Semiconductors TriMedia architecture family                 */
#define EM_QDSP6         164 /**< \brief QUALCOMM DSP6 Processor                                         */
#define EM_8051          165 /**< \brief Intel 8051 and variants                                         */
#define EM_STXP7X        166 /**< \brief STMicroelectronics STxP7x family of configurable and extensible RISC processors */
#define EM_NDS32         167 /**< \brief Andes Technology compact code size embedded RISC processor family */
#define EM_ECOG1         168 /**< \brief Cyan Technology eCOG1X family                                   */
#define EM_ECOG1X        168 /**< \brief Cyan Technology eCOG1X family                                   */
#define EM_MAXQ30        169 /**< \brief Dallas Semiconductor MAXQ30 Core Micro-controllers              */
#define EM_XIMO16        170 /**< \brief New Japan Radio (NJR) 16-bit DSP Processor                      */
#define EM_MANIK         171 /**< \brief M2000 Reconfigurable RISC Microprocessor                        */
#define EM_RX            173 /**< \brief Renesas RX family                                               */
#define EM_METAG         174 /**< \brief Imagination Technologies META processor architecture            */
#define EM_MCST_ELBRUS   175 /**< \brief MCST Elbrus general purpose hardware architecture              */
#define EM_ECOG16        176 /**< \brief Cyan Technology eCOG16 family                                   */
#define EM_CR16          177 /**< \brief National Semiconductor CompactRISC CR16 16-bit microprocessor   */
#define EM_ETPU          178 /**< \brief Freescale Extended Time Processing Unit                         */
#define EM_SLE9X         179 /**< \brief Infineon Technologies SLE9X core                                */
#define EM_L10M          180 /**< \brief Intel L10M                                                      */
#define EM_K10M          181 /**< \brief Intel K10M                                                      */
#define EM_AARCH64       183 /**< \brief ARM 64-bit architecture (AARCH64)                               */
#define EM_AVR32         185 /**< \brief Atmel Corporation 32-bit microprocessor family                  */
#define EM_STM8          186 /**< \brief STMicroeletronics STM8 8-bit microcontroller                    */
#define EM_TILE64        187 /**< \brief Tilera TILE64 multicore architecture family                     */
#define EM_TILEPRO       188 /**< \brief Tilera TILEPro multicore architecture family                    */
#define EM_MICROBLAZE    189 /**< \brief Xilinx MicroBlaze 32-bit RISC soft processor core               */
#define EM_CUDA          190 /**< \brief NVIDIA CUDA architecture                                        */
#define EM_TILEGX        191 /**< \brief Tilera TILE-Gx multicore architecture family                    */
#define EM_CLOUDSHIELD   192 /**< \brief CloudShield architecture family                                */
#define EM_COREA_1ST     193 /**< \brief KIPO-KAIST Core-A 1st generation processor family               */
#define EM_COREA_2ND     194 /**< \brief KIPO-KAIST Core-A 2nd generation processor family               */
#define EM_ARC_COMPACT2  195 /**< \brief Synopsis ARCompact V2                                         */
#define EM_OPEN8         196 /**< \brief Open8 8-bit RISC soft processor core                            */
#define EM_RL78          197 /**< \brief Renesas RL78 family                                             */
#define EM_VIDEOCORE5    198 /**< \brief Broadcom VideoCore V processor                                  */
#define EM_78KOR         199 /**< \brief Renesas 78KOR family                                            */
#define EM_56800EX       200 /**< \brief Freescale 56800EX Digital Signal Controller (DSC)               */
#define EM_BA1           201 /**< \brief Beyond BA1 CPU architecture                                     */
#define EM_BA2           202 /**< \brief Beyond BA2 CPU architecture                                     */
#define EM_XCORE         203 /**< \brief XMOS xCORE processor family                                     */
#define EM_MCHP_PIC      204 /**< \brief Microchip 8-bit PIC(r) family                                   */
#define EM_INTEL205      205 /**< \brief Reserved by Intel                                               */
#define EM_INTEL206      206 /**< \brief Reserved by Intel                                               */
#define EM_INTEL207      207 /**< \brief Reserved by Intel                                               */
#define EM_INTEL208      208 /**< \brief Reserved by Intel                                               */
#define EM_INTEL209      209 /**< \brief Reserved by Intel                                               */
#define EM_KM32          210 /**< \brief KM211 KM32 32-bit processor                                     */
#define EM_KMX32         211 /**< \brief KM211 KMX32 32-bit processor                                    */
#define EM_KMX16         212 /**< \brief KM211 KMX16 16-bit processor                                    */
#define EM_KMX8          213 /**< \brief KM211 KMX8 8-bit processor                                      */
#define EM_KVARC         214 /**< \brief KM211 KVARC processor                                           */
#define EM_CDP           215 /**< \brief Paneve CDP architecture family                                  */
#define EM_COGE          216 /**< \brief Cognitive Smart Memory Processor                                */
#define EM_COOL          217 /**< \brief Bluechip Systems CoolEngine                                     */
#define EM_NORC          218 /**< \brief Nanoradio Optimized RISC                                        */
#define EM_CSR_KALIMBA   219 /**< \brief CSR Kalimba architecture family                                */
#define EM_Z80           220 /**< \brief Zilog Z80                                                       */
#define EM_VISIUM        221 /**< \brief Controls and Data Services VISIUMcore processor                 */
#define EM_FT32          222 /**< \brief FTDI Chip FT32 high performance 32-bit RISC architecture        */
#define EM_MOXIE         223 /**< \brief Moxie processor family                                          */
#define EM_AMDGPU        224 /**< \brief AMD GPU architecture                                            */
#define EM_RISCV         243 /**< \brief RISC-VEM_FR30 84 Fujitsu FR30                                   */

/**
 *    e_version values are as follows:
 */
#define EV_NONE    0 /**< \brief version         */
#define EV_CURRENT 1 /**< \brief Current version */

/**
 *    e_ident[] index values are as follows:
 */
#define EI_MAG0    0 /**< \brief File identification     */
#define EI_MAG1    1 /**< \brief File identification     */
#define EI_MAG2    2 /**< \brief File identification     */
#define EI_MAG3    3 /**< \brief File identification     */
#define EI_CLASS   4 /**< \brief File class              */
#define EI_DATA    5 /**< \brief Data encoding           */
#define EI_VERSION 6 /**< \brief File version            */
#define EI_PAD     7 /**< \brief Start of padding bytes  */

/**
 *   e_ident[ELFMAG0...ELFMAG3] values are as follows:
 */
#define ELFMAG0 0x7f /**< \brief e_ident[EI_MAG0] */
#define ELFMAG1 'E'  /**< \brief e_ident[EI_MAG1] */
#define ELFMAG2 'L'  /**< \brief e_ident[EI_MAG2] */
#define ELFMAG3 'F'  /**< \brief e_ident[EI_MAG3] */

/**
 *   e_ident[EI_CLASS] values are as follows:
 */
#define ELFCLASSNONE 0 /**< \brief Invalid class  */
#define ELFCLASS32   1 /**< \brief 32-bit objects */
#define ELFCLASS64   2 /**< \brief 64-bit objects */

/**
 *   e_ident[EI_DATA] values are as follows:
 */
#define ELFDATANONE 0 /**< \brief Invalid data encoding  */
#define ELFDATA2LSB 1 /**< \brief Little-Endian storage  */
#define ELFDATA2MSB 2 /**< \brief Big-Endian storage     */

/**
 *   Special Section Indexes:
 */

#define SHN_UNDEF     0      /**< \brief Marks an undefined, missing, irrelevant, or otherwise meaningless section */
#define SHN_LORESERVE 0xff00 /**< \brief Specifies the lower bound of the range of reserved indexes */
#define SHN_LOPROC    0xff00 /**< \brief Values >= than this are reserved for processor-specific semantics */
#define SHN_HIPROC    0xff1f /**< \brief Values <= than this are reserved for processor-specific semantics */
#define SHN_ABS       0xfff1 /**< \brief Specifies absolute values for the corresponding reference */
#define SHN_COMMON    0xfff2 /**< \brief Symbols defined relative to this section are common symbols */
#define SHN_HIRESERVE 0xffff /**< \brief Specifies the upper bound of the range of reserved indexes */

/**
 *    32 bit Section Header Format
 */
typedef struct
{
    Elf32_Word sh_name;      /**< \brief Index into Section Header String Table giving location of section name */
    Elf32_Word sh_type;      /**< \brief Section type */
    Elf32_Word sh_flags;     /**< \brief Section attributes */
    Elf32_Addr sh_addr;      /**< \brief Address at which first byte of section should reside */
    Elf32_Off  sh_offset;    /**< \brief File offset to first byte of Section */
    Elf32_Word sh_size;      /**< \brief Section size, in bytes */
    Elf32_Word sh_link;      /**< \brief Section Header Table index link (interpretation depends upon Section Type) */
    Elf32_Word sh_info;      /**< \brief Extra information (interpretation depends upon Section Type) */
    Elf32_Word sh_addralign; /**< \brief Section memory address alignment constraints */
    Elf32_Word sh_entsize;   /**< \brief Size of an entry for a Section containing a table of fixed size entries */
} Elf32_Shdr;

/**
 *    64 bit Section Header Format
 */
typedef struct
{
    Elf64_Word  sh_name;      /**< \brief Index into Section Header String Table giving location of section name */
    Elf64_Word  sh_type;      /**< \brief Section type */
    Elf64_Xword sh_flags;     /**< \brief Section attributes */
    Elf64_Addr  sh_addr;      /**< \brief Address at which first byte of section should reside */
    Elf64_Off   sh_offset;    /**< \brief File offset to first byte of Section */
    Elf64_Xword sh_size;      /**< \brief Section size, in bytes */
    Elf64_Word  sh_link;      /**< \brief Section Header Table index link (interpretation depends upon Section Type) */
    Elf64_Word  sh_info;      /**< \brief Extra information (interpretation depends upon Section Type) */
    Elf64_Xword sh_addralign; /**< \brief Section memory address alignment constraints */
    Elf64_Xword sh_entsize;   /**< \brief Size of an entry for a Section containing a table of fixed size entries */
} Elf64_Shdr;

union Elf_Shdr
{
    Elf32_Shdr Shdr32;
    Elf64_Shdr Shdr64;
};

/**
 *    sh_type values are as follows:
 */
#define SHT_NULL          0  /**< \brief Marks Section Header as inactive */
#define SHT_PROGBITS      1  /**< \brief Section contains information defined by program */
#define SHT_SYMTAB        2  /**< \brief Section contains symbols for link editing */
#define SHT_STRTAB        3  /**< \brief Section contains a string table */
#define SHT_RELA          4  /**< \brief Section contains relocation entries with explicit addends */
#define SHT_HASH          5  /**< \brief Section contains a symbol hash table */
#define SHT_DYNAMIC       6  /**< \brief Section contains information for dynamic linking */
#define SHT_NOTE          7  /**< \brief Section contains information for marking the file */
#define SHT_NOBITS        8  /**< \brief Section occupies no file space but otherwise resembles #SHT_PROGBITS */
#define SHT_REL           9  /**< \brief Section contains relocation entries without explicit addends */
#define SHT_SHLIB         10 /**< \brief Reserved Section type */
#define SHT_DYNSYM        11 /**< \brief Section contains minimal set of dynamic linking symbols */
#define SHT_INIT_ARRAY    14 /**< \brief Array of pointers to initialization functions */
#define SHT_FINI_ARRAY    15 /**< \brief Array of pointers to termination functions */
#define SHT_PREINIT_ARRAY 16 /**< \brief Array of pointers to functions invoked prior to all init functions */
#define SHT_GROUP         17 /**< \brief A set of sections that are related and must be treated specially by the linker */
#define SHT_SYMTAB_SHNDX  18 /**< \brief An array of values corresponding (one-to-one) with symbol table entries */

#define SHT_LOPROC 0x70000000 /**< \brief Values >= are reserved for Processor specific semantics */
#define SHT_HIPROC 0x7fffffff /**< \brief Values <= are reserved for Processor specific semantics */
#define SHT_LOUSER 0x80000000 /**< \brief Values >= are reserved for application programs */
#define SHT_HIUSER 0xffffffff /**< \brief Values <= are reserved for application programs */

/**
 *    sh_flags values are as follows:
 */
#define SHF_WRITE            0x1   /**< \brief Section contains data that should be writable during execution */
#define SHF_ALLOC            0x2   /**< \brief Section occupies memory during process execution */
#define SHF_EXECINSTR        0x4   /**< \brief Section contains executable machine instructions */
#define SHF_MERGE            0x10  /**< \brief Section contains data that can be merged to eliminate duplication */
#define SHF_STRINGS          0x20  /**< \brief Section consists of null-terminated character strings */
#define SHF_INFO_LINK        0x40  /**< \brief sh_info field of this section contains section header table index */
#define SHF_LINK_ORDER       0x80  /**< \brief Section requires special ordering requirements for link editors */
#define SHF_OS_NONCONFORMING 0x100 /**< \brief Section requires special OS-specific processing */
#define SHF_GROUP            0x200 /**< \brief Section is a member of a section group */

#define SHF_MASKPROC 0xf0000000 /**< \brief Bits in this mask are reserved for processor specific semantics */

/**
 *    32 bit Symbol Table Entry
 */
typedef struct
{
    Elf32_Word    st_name;  /**< \brief String Table Index that gives the symbol name */
    Elf32_Addr    st_value; /**< \brief Value of associated symbol */
    Elf32_Word    st_size;  /**< \brief Size, in bytes, of symbol */
    unsigned char st_info;  /**< \brief Symbol's type and binding attributes */
    unsigned char st_other; /**< \brief  */
    Elf32_Half    st_shndx; /**< \brief  */
} Elf32_Sym;

/**
 *    64 bit Symbol Table Entry
 */
typedef struct
{
    Elf64_Word    st_name;  /**< \brief String Table Index that gives the symbol name */
    unsigned char st_info;  /**< \brief Symbol's type and binding attributes */
    unsigned char st_other; /**< \brief  */
    Elf64_Half    st_shndx; /**< \brief  */
    Elf64_Addr    st_value; /**< \brief Value of associated symbol */
    Elf64_Xword   st_size;  /**< \brief Size, in bytes, of symbol */
} Elf64_Sym;

union Elf_Sym
{
    Elf32_Sym Sym32;
    Elf64_Sym Sym64;
};

#define ELF32_ST_INFO (b, t)(((b) << 4) + ((t)&0xf))

/**
 *   Extraction and possible values of symbol table entry binding attributes in st_info
 */
#define ELF32_ST_BIND (i)((i) >> 4)

#define STB_LOCAL  0 /**< \brief Local symbols are not visible outside the object file */
#define STB_GLOBAL 1 /**< \brief Global symbols are visible to all object files being combined */
#define STB_WEAK   2 /**< \brief Same as Global but with lower precedence */

#define STB_LOPROC 13 /**< \brief Values >= are reserved for processor specific semantics */
#define STB_HIPROC 15 /**< \brief Values <= are reserved for processor specific semantics */

/**
 *   Extraction and possible values of symbol table entry type attributes in st_info
 */
#define ELF32_ST_TYPE (i)((i)&0xf)

#define STT_NOTYPE  0 /**< \brief The symbol's type is unspecified */
#define STT_OBJECT  1 /**< \brief Symbol associated with a data object (variable, array, etc) */
#define STT_FUNC    2 /**< \brief Symbol associated with a function or other executable code  */
#define STT_SECTION 3 /**< \brief Symbol associated with a section */
#define STT_FILE    4 /**< \brief Symbol's name gives the source file */
#define STT_COMMON  5 /**< \brief Symbol labels an uninitialized common block */

#define STT_LOPROC 13 /**< \brief Values >= are reserved for processor specific semantics */
#define STT_HIPROC 15 /**< \brief Values <= are reserved for processor specific semantics */

#endif
