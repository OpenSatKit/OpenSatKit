/*************************************************************************
** File: 
**  $Id: lc_def_wdt.c 1.19 2013/11/07 17:55:27EST sstrege Exp  $
** 
** Purpose: 
**   Limit Checker (LC) Watchpoint Definition Table (WDT)
** 
** Notes: 
**   This table is computer generated and is NOT to be hand modified. 
**   Generated November 7, 2013, 10:43 am 
** 
** 
**   $Log: lc_def_wdt.c  $ 
**   Revision 1.19 2013/11/07 17:55:27EST sstrege  
**   GPM-CCR-1133 Updates
**   2014/11/18 M. Lambertson, Columbus Technologies and Services Inc.
**                            CCR-035: Added WPs 194-196 for GMI SMA Undertemperature.
**   2015/09/29 M. Lambertson CCR-057: Modified WP 82.  
** 
*/ 
/************************************************************************* 
** Includes 
*************************************************************************/ 
#include "lc_msg.h"    
#include "lc_tbl.h"   
#include "cfe_tbl_filedef.h"  
 
/************************************************************************* 
** Exported Data 
*************************************************************************/ 
/* 
** Table file header 
*/ 
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) = 
{
    "LC_WDT", "LC.LC_WDT", "LC watchpoint definition table", 
    "lc_def_wdt.tbl", (sizeof(LC_WDTEntry_t) * LC_MAX_WATCHPOINTS) 
}; 
/* 
** Watchpoint Definition Table (WDT) data
*/ 
LC_WDTEntry_t LC_WDT[LC_MAX_WATCHPOINTS] =
{ 
    /** DataType,   OperatorID,   MessageID,   WatchpointOffset,   BitMask,   ComparisonValue,   CustomFuncArgument **/ 

    /*  0 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  1 (GC_SCOMA_SCRATCHPAD0 - SCOMA SCRATCHPAD REGISTER 0  - REG 0x22)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x86F, .WatchpointOffset = 146, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0x4E53, .CustomFuncArgument = 0},

    /*  2 (GC_SCOMB_SCRATCHPAD0 - SCOMB SCRATCHPAD REGISTER 0  - REG 0x22)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x879, .WatchpointOffset = 146, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0x4E53, .CustomFuncArgument = 0},

    /*  3 (GC_MACA_T0_1_BW - MAC A T0-1 Breakwire (J236:7)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0xA00, .WatchpointOffset = 580, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xE8E, .CustomFuncArgument = 0},

    /*  4 (GC_MACA_T0_1_BW - MAC A T0-1 Breakwire (J236:7)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LE, .MessageID = 0xA00, .WatchpointOffset = 580, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xED7, .CustomFuncArgument = 0},

    /*  5 (GC_MACB_T0_2_BW - MAC B T0-2 Breakwire (J1236:7)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0xA02, .WatchpointOffset = 580, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xE84, .CustomFuncArgument = 0},

    /*  6 (GC_MACB_T0_2_BW - MAC B T0-2 Breakwire (J1236:7)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LE, .MessageID = 0xA02, .WatchpointOffset = 580, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xECE, .CustomFuncArgument = 0},

    /*  7 (GC_SW_FS_BW_1A - Fairing Breakwire 1A Status)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A7, .WatchpointOffset = 188, .BitMask = 0x2200, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x200, .CustomFuncArgument = 0},

    /*  8 (GC_SW_FS_BW_1B - Fairing Breakwire 1B Status)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A7, .WatchpointOffset = 190, .BitMask = 0x2200, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x200, .CustomFuncArgument = 0},

    /*  9 (GC_MACA_FS_BW_2A - MAC A Fairing Breakwire 2A (J236:5)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0xA00, .WatchpointOffset = 578, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xBFA, .CustomFuncArgument = 0},

    /*  10 (GC_MACA_FS_BW_2A - MAC A Fairing Breakwire 2A (J236:5)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LE, .MessageID = 0xA00, .WatchpointOffset = 578, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xC40, .CustomFuncArgument = 0},

    /*  11 (GC_MACB_FS_BW_2B - MAC B Fairing Breakwire 2B (J1236:5)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0xA02, .WatchpointOffset = 578, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xBF6, .CustomFuncArgument = 0},

    /*  12 (GC_MACB_FS_BW_2B - MAC B Fairing Breakwire 2B (J1236:5)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LE, .MessageID = 0xA02, .WatchpointOffset = 578, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xC3B, .CustomFuncArgument = 0},

    /*  13 (GC_MACA_LV_BW_2A - MAC A LV Breakwire 2A (J236:3)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0xA00, .WatchpointOffset = 576, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xE3A, .CustomFuncArgument = 0},

    /*  14 (GC_MACA_LV_BW_2A - MAC A LV Breakwire 2A (J236:3)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LE, .MessageID = 0xA00, .WatchpointOffset = 576, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xE83, .CustomFuncArgument = 0},

    /*  15 (GC_MACB_LV_BW_2B - MAC B LV Breakwire 2B (J1236:3)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0xA02, .WatchpointOffset = 576, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xE32, .CustomFuncArgument = 0},

    /*  16 (GC_MACB_LV_BW_2B - MAC B LV Breakwire 2B (J1236:3)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LE, .MessageID = 0xA02, .WatchpointOffset = 576, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xE7B, .CustomFuncArgument = 0},

    /*  17 (GC_SW_LV_SW_1A - Launch Vehicle Separation Switch 1A Status)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A7, .WatchpointOffset = 188, .BitMask = 0x1100, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x100, .CustomFuncArgument = 0},

    /*  18 (GC_SW_LV_SW_1B - Launch Vehicle Separation Switch 1B Status)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A7, .WatchpointOffset = 190, .BitMask = 0x1100, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x100, .CustomFuncArgument = 0},

    /*  19 (GC_SW_LV_SW_2A - Launch Vehicle Separation Switch 2A Status)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A7, .WatchpointOffset = 188, .BitMask = 0x4400, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x400, .CustomFuncArgument = 0},

    /*  20 (GC_SW_LV_SW_2B - Launch Vehicle Separation Switch 2B Status)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A7, .WatchpointOffset = 190, .BitMask = 0x4400, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x400, .CustomFuncArgument = 0},

    /*  21 (GC_MACA_LV_BW_1A - MAC A LV Breakwire 1A (J236:1)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0xA00, .WatchpointOffset = 574, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xFE1, .CustomFuncArgument = 0},

    /*  22 (GC_MACB_LV_BW_1B - MAC B LV Breakwire 1B (J1236:1)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0xA02, .WatchpointOffset = 574, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xFE1, .CustomFuncArgument = 0},

    /*  23 (GC_ATBA_SAPOS5 - SA Position Sensor 5)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9DE, .WatchpointOffset = 24, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 3512, .CustomFuncArgument = 0},

    /*  24 (GC_ATBB_SAPOS5 - SA Position Sensor 5)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9E0, .WatchpointOffset = 24, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 3544, .CustomFuncArgument = 0},

    /*  25 (GC_ATBA_SAPOS8 - SA Position Sensor 8)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9DE, .WatchpointOffset = 30, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 3508, .CustomFuncArgument = 0},

    /*  26 (GC_ATBB_SAPOS8 - SA Position Sensor 8)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9E0, .WatchpointOffset = 30, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 3539, .CustomFuncArgument = 0},

    /*  27 (GC_MACA_I_CAL - MAC A Current Calibration (I_CAL=2048))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0xA00, .WatchpointOffset = 244, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x7E8, .CustomFuncArgument = 0},

    /*  28 (GC_MACA_I_CAL - MAC A Current Calibration (I_CAL=2048))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LE, .MessageID = 0xA00, .WatchpointOffset = 244, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x824, .CustomFuncArgument = 0},

    /*  29 (GC_MACB_I_CAL - MAC B Current Calibration (I_CAL=2048))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0xA02, .WatchpointOffset = 244, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x7E8, .CustomFuncArgument = 0},

    /*  30 (GC_MACB_I_CAL - MAC B Current Calibration (I_CAL=2048))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LE, .MessageID = 0xA02, .WatchpointOffset = 244, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x824, .CustomFuncArgument = 0},

    /*  31 (GC_SCOMA_SCRATCHPAD1 - SCOMA SCRATCHPAD REGISTER 1  - REG 0x23)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x86F, .WatchpointOffset = 150, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0x505A, .CustomFuncArgument = 0},

    /*  32 (GC_SCOMB_SCRATCHPAD1 - SCOMB SCRATCHPAD REGISTER 1  - REG 0x23)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x879, .WatchpointOffset = 150, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0x505A, .CustomFuncArgument = 0},

    /*  33 (GC_SCOMA_SCRATCHPAD2 - SCOMA SCRATCHPAD REGISTER 2  - REG 0x24)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x86F, .WatchpointOffset = 154, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0x2DA5, .CustomFuncArgument = 0},

    /*  34 (GC_SCOMB_SCRATCHPAD2 - SCOMB SCRATCHPAD REGISTER 2  - REG 0x24)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x879, .WatchpointOffset = 154, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0x2DA5, .CustomFuncArgument = 0},

    /*  35 (GC_SCOMA_SCRATCHPAD3 - SCOMA SCRATCHPAD REGISTER 3  - REG 0x25)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x86F, .WatchpointOffset = 158, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0xC396, .CustomFuncArgument = 0},

    /*  36 (GC_SCOMB_SCRATCHPAD3 - SCOMB SCRATCHPAD REGISTER 3  - REG 0x25)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x879, .WatchpointOffset = 158, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0xC396, .CustomFuncArgument = 0},

    /*  37 (GC_SW_LINK1STATE - SBC Link 1 State)  */
    { .DataType = LC_DATA_UBYTE, .OperatorID = LC_OPER_NE, .MessageID = 0x9A7, .WatchpointOffset = 20, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned8in32.Unsigned8 = 0x7, .CustomFuncArgument = 0},

    /*  38 (GC_SW_LINK2STATE - SBC Link 2 State)  */
    { .DataType = LC_DATA_UBYTE, .OperatorID = LC_OPER_NE, .MessageID = 0x9A7, .WatchpointOffset = 21, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned8in32.Unsigned8 = 0x7, .CustomFuncArgument = 0},

    /*  39 (GC_RM_SDRFREESPACE - RM  Free space in SDR partitions)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0x9A6, .WatchpointOffset = 12, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 1288490189, .CustomFuncArgument = 0},

    /*  40 (GC_DM_WRITEERRORCNT - DM File write error counter)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A2, .WatchpointOffset = 26, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 0, .CustomFuncArgument = 0},

    /*  41 (GC_GM_WRITEERRORCNT - GM File write error counter)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A3, .WatchpointOffset = 24, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 0, .CustomFuncArgument = 0},

    /*  42 (GC_PS_MAC_TLMSTALE - PS MAC Telemetry Stale Flag)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A5, .WatchpointOffset = 18, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 1, .CustomFuncArgument = 0},

    /*  43 (GC_CF_TOTALINPROGTRANS - Total (uplink & downlink) transactions in progress)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x8B0, .WatchpointOffset = 64, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 200, .CustomFuncArgument = 0},

    /*  44 (GC_DS_FILEWRITEERRCNT - The number of bad destination file writes)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x8B8, .WatchpointOffset = 34, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 0, .CustomFuncArgument = 0},

    /*  45 (GC_RM_SDRFREESPACE - RM  Free space in SDR partitions)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0x9A6, .WatchpointOffset = 12, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 2791728742, .CustomFuncArgument = 0},

    /*  46 (GC_SB_MSGLIMEC - GC  SB Message ID to Pipe Limit Error Counter)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x803, .WatchpointOffset = 26, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 10, .CustomFuncArgument = 0},

    /*  47 (GC_SB_MSGLIMEC - GC  SB Message ID to Pipe Limit Error Counter)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x803, .WatchpointOffset = 26, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 100, .CustomFuncArgument = 0},

    /*  48 (GC_CI_CMDELAPSEDTIME - CI Number of seconds since last ground command)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A0, .WatchpointOffset = 36, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 28800, .CustomFuncArgument = 0},

    /*  49 (GC_CI_CMDELAPSEDTIME - CI Number of seconds since last ground command)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A0, .WatchpointOffset = 36, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 86400, .CustomFuncArgument = 0},

    /*  50 (GC_DI_FDCFAULTHGA1553MSG - DI Fault state - HGA raw 1553 data not received by XB )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 12, .BitMask = 0x100, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x100, .CustomFuncArgument = 0},

    /*  51 (GC_DI_FDCFAULTHGAINENCDIFF - DI Fault state - HGA INNER maximum encoder vs. pulse position difference exceeded )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 16, .BitMask = 0x20, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x20, .CustomFuncArgument = 0},

    /*  52 (GC_DI_FDCFAULTHGAOUTENCDIFF - DI Fault state - HGA OUTER maximum encoder vs. pulse position difference exceeded )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 16, .BitMask = 0x40, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x40, .CustomFuncArgument = 0},

    /*  53 (GC_DI_FDCFAULTHGAINMAXANGLE - DI Fault state - HGA INNER maximum valid angle exceeded  )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 12, .BitMask = 0x1000, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1000, .CustomFuncArgument = 0},

    /*  54 (GC_DI_FDCFAULTHGAOUTMAXANGLE - DI Fault state - HGA OUTER maximum valid angle exceeded  )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 12, .BitMask = 0x2000, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x2000, .CustomFuncArgument = 0},

    /*  55 (GC_XB_SCRETRYFAILS_PSEA - XB Spacecraft Failure Counts (RT 16 = PSEA))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A8, .WatchpointOffset = 66, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 30, .CustomFuncArgument = 0},

    /*  56 (GC_XB_SCRETRYFAILS_PSEB - XB Spacecraft Failure Counts (RT 17 = PSEB))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A8, .WatchpointOffset = 68, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 30, .CustomFuncArgument = 0},

    /*  57 (GC_XB_SCRETRYFAILS_PSEA - XB Spacecraft Failure Counts (RT 16 = PSEA))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A8, .WatchpointOffset = 66, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 10, .CustomFuncArgument = 0},

    /*  58 (GC_PSEA_LIMCYC_FLT_CRCT - PSEA Limit Cycle Fault Corrected Status)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xA17, .WatchpointOffset = 76, .BitMask = 0x20, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x20, .CustomFuncArgument = 0},

    /*  59 (GC_PSEA_SAM1_PWM1_ST - SAM-1 PWM State)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xA19, .WatchpointOffset = 30, .BitMask = 0x1, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1, .CustomFuncArgument = 0},

    /*  60 (GC_PSEA_SAM2_PWM2_ST - SAM-2 PWM State)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xA19, .WatchpointOffset = 34, .BitMask = 0x1, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1, .CustomFuncArgument = 0},

    /*  61 (GC_PSEA_BAT_1_V - Battery 1 Voltage)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0xA16, .WatchpointOffset = 290, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xC04, .CustomFuncArgument = 0},

    /*  62 (GC_PSEA_BAT_2_V - Battery 2 Voltage)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0xA16, .WatchpointOffset = 292, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xC04, .CustomFuncArgument = 0},

    /*  63 (GC_PSEA_BAT_3_V - Battery 3 Voltage)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0xA16, .WatchpointOffset = 294, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xC04, .CustomFuncArgument = 0},

    /*  64 (GC_PSEA_BAT_1_V - Battery 1 Voltage)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0xA16, .WatchpointOffset = 290, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xB50, .CustomFuncArgument = 0},

    /*  65 (GC_PSEA_BAT_2_V - Battery 2 Voltage)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0xA16, .WatchpointOffset = 292, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xB50, .CustomFuncArgument = 0},

    /*  66 (GC_PSEA_BAT_3_V - Battery 3 Voltage)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0xA16, .WatchpointOffset = 294, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xB50, .CustomFuncArgument = 0},

    /*  67 (GC_PSEA_BAT_1_V - Battery 1 Voltage)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA16, .WatchpointOffset = 290, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xD2F, .CustomFuncArgument = 0},

    /*  68 (GC_PSEA_BAT_2_V - Battery 2 Voltage)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA16, .WatchpointOffset = 292, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xD2F, .CustomFuncArgument = 0},

    /*  69 (GC_PSEA_BAT_3_V - Battery 3 Voltage)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA16, .WatchpointOffset = 294, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xD2F, .CustomFuncArgument = 0},

    /*  70 (GC_PSEA_SM1_BAT1_N_I - SM1)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_CUSTOM, .MessageID = 0xA16, .WatchpointOffset = 358, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x0, .CustomFuncArgument = 0},

    /*  71 (GC_PSEA_SAM1_PRCNT_PWM_I - SAM1)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA16, .WatchpointOffset = 328, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xF60, .CustomFuncArgument = 0},

    /*  72 (GC_PS_AMP_HR_OUT_PSEA_BATT1 - PSE-A Batt 1 Amp-hours Out)  */
    { .DataType = LC_DATA_FLOAT_BE, .OperatorID = LC_OPER_CUSTOM, .MessageID = 0x9A5, .WatchpointOffset = 128, .BitMask = LC_NO_BITMASK, .ComparisonValue.Float32 = 0, .CustomFuncArgument = 0},

    /*  73 (GC_PSEA_SM1_BAT1_N_I - SM1)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA16, .WatchpointOffset = 358, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x199, .CustomFuncArgument = 0},

    /*  74 (GC_PSEA_SM1_BAT2_N_I - SM1)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA16, .WatchpointOffset = 360, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x199, .CustomFuncArgument = 0},

    /*  75 (GC_PSEA_SM1_BAT3_N_I - SM1)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA16, .WatchpointOffset = 362, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x199, .CustomFuncArgument = 0},

    /*  76 (GC_PS_AVG_TEMP_PSEA_BATT1 - PSE-A Batt 1 Avg Temperature)  */
    { .DataType = LC_DATA_FLOAT_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A5, .WatchpointOffset = 72, .BitMask = LC_NO_BITMASK, .ComparisonValue.Float32 = 32.0, .CustomFuncArgument = 0},

    /*  77 (GC_PS_AVG_TEMP_PSEA_BATT2 - PSE-A Batt 2 Avg Temperature)  */
    { .DataType = LC_DATA_FLOAT_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A5, .WatchpointOffset = 76, .BitMask = LC_NO_BITMASK, .ComparisonValue.Float32 = 32.0, .CustomFuncArgument = 0},

    /*  78 (GC_PS_AVG_TEMP_PSEA_BATT3 - PSE-A Batt 3 Avg Temperature)  */
    { .DataType = LC_DATA_FLOAT_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A5, .WatchpointOffset = 80, .BitMask = LC_NO_BITMASK, .ComparisonValue.Float32 = 32.0, .CustomFuncArgument = 0},

    /*  79 (GC_PSEA_SAM1_SEG_ST - SAM-1 Segment Status)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_CUSTOM, .MessageID = 0xA18, .WatchpointOffset = 410, .BitMask = 0xF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x0, .CustomFuncArgument = 0},

    /*  80 (GC_PS_BATT_VOLT_ERR_CNTR_PSEA - PSE-A Battery Voltage Error count)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A5, .WatchpointOffset = 224, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 0, .CustomFuncArgument = 0},

    /*  81 (GC_PSEA_SM1_BAT1_P_I - SM1)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_CUSTOM, .MessageID = 0xA16, .WatchpointOffset = 352, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x0, .CustomFuncArgument = 0},

    /*  82 (GC_PS_AVG_VOLTAGE_PSEA - PSE-A Avg Voltage)  */
    { .DataType = LC_DATA_FLOAT_BE, .OperatorID = LC_OPER_LT, .MessageID = 0x9A5, .WatchpointOffset = 64, .BitMask = LC_NO_BITMASK, .ComparisonValue.Float32 = 31.8, .CustomFuncArgument = 0},

    /*  83 (GC_DI_FDCFAULTSA1553MSG - DI Fault state - SA raw 1553 data not received by XB )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 18, .BitMask = 0x4000, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x4000, .CustomFuncArgument = 0},

    /*  84 (GC_DI_FDCFAULTSAPYENCDIFF - DI Fault state - SA +Y maximum encoder vs. pulse position difference exceeded )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 16, .BitMask = 0x8, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x8, .CustomFuncArgument = 0},

    /*  85 (GC_DI_FDCFAULTSAMYENCDIFF - DI Fault state - SA -Y maximum encoder vs. pulse position difference exceeded )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 16, .BitMask = 0x10, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x10, .CustomFuncArgument = 0},

    /*  86 (GC_SADA_PY_LOCATION - Special Locations)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_NE, .MessageID = 0x9F0, .WatchpointOffset = 16, .BitMask = 0xC000, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x8000, .CustomFuncArgument = 0},

    /*  87 (GC_SADA_MY_LOCATION - Special Locations)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_NE, .MessageID = 0x9F0, .WatchpointOffset = 32, .BitMask = 0xC000, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xC000, .CustomFuncArgument = 0},

    /*  88 (GC_DI_FDCFAULTSAPYMAXANGLE - DI Fault state - SA +Y gimbal maximum valid angle exceeded  )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 20, .BitMask = 0x4, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x4, .CustomFuncArgument = 0},

    /*  89 (GC_DI_FDCFAULTSAMYMAXANGLE - DI Fault state - SA -Y gimbal maximum valid angle exceeded  )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 20, .BitMask = 0x8, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x8, .CustomFuncArgument = 0},

    /*  90 (GC_CL_ACSMODE - Attitude Control Mode)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D6, .WatchpointOffset = 196, .BitMask = 0x1C0, .ComparisonValue.Unsigned32 = 0x0, .CustomFuncArgument = 0},

    /*  91 (GC_CL_ACSMODE - Attitude Control Mode)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D6, .WatchpointOffset = 196, .BitMask = 0x1C0, .ComparisonValue.Unsigned32 = 0x40, .CustomFuncArgument = 0},

    /*  92 (GC_CL_ACSMODE - Attitude Control Mode)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D6, .WatchpointOffset = 196, .BitMask = 0x1C0, .ComparisonValue.Unsigned32 = 0x80, .CustomFuncArgument = 0},

    /*  93 (GC_CL_ACSMODE - Attitude Control Mode)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D6, .WatchpointOffset = 196, .BitMask = 0x1C0, .ComparisonValue.Unsigned32 = 0xC0, .CustomFuncArgument = 0},

    /*  94 (GC_CL_ACSMODE - Attitude Control Mode)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D6, .WatchpointOffset = 196, .BitMask = 0x1C0, .ComparisonValue.Unsigned32 = 0x100, .CustomFuncArgument = 0},

    /*  95 (GC_CL_ACSMODE - Attitude Control Mode)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D6, .WatchpointOffset = 196, .BitMask = 0x1C0, .ComparisonValue.Unsigned32 = 0x140, .CustomFuncArgument = 0},

    /*  96 (GC_CL_ACSMODE - Attitude Control Mode)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D6, .WatchpointOffset = 196, .BitMask = 0x1C0, .ComparisonValue.Unsigned32 = 0x1C0, .CustomFuncArgument = 0},

    /*  97 (GC_CL_ACSMODE - Attitude Control Mode)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D6, .WatchpointOffset = 196, .BitMask = 0x1C0, .ComparisonValue.Unsigned32 = 0x180, .CustomFuncArgument = 0},

    /*  98 (GC_DI_DCSSECLIPSESTATE - DI Eclipse Flag indicated by CSS measurement)  */
    { .DataType = LC_DATA_UBYTE, .OperatorID = LC_OPER_EQ, .MessageID = 0xAB0, .WatchpointOffset = 82, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned8in32.Unsigned8 = 1, .CustomFuncArgument = 0},

    /*  99 (GC_SD_FDCFAULTSUNAVOID - SD Fault state - Sun Vector is within a specified angle of a sun avoidance vector)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 14, .BitMask = 0x80, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x80, .CustomFuncArgument = 0},

    /*  100 (GC_SD_MOMSYSMOMMAG - Magnitude of S/C complete Angular Momentum)  */
    { .DataType = LC_DATA_FLOAT_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9C3, .WatchpointOffset = 120, .BitMask = LC_NO_BITMASK, .ComparisonValue.Float32 = 200, .CustomFuncArgument = 0},

    /*  101 (GC_SD_MOMSYSMOMMAG - Magnitude of S/C complete Angular Momentum)  */
    { .DataType = LC_DATA_FLOAT_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9C3, .WatchpointOffset = 120, .BitMask = LC_NO_BITMASK, .ComparisonValue.Float32 = 200, .CustomFuncArgument = 0},

    /*  102 (GC_SD_MOMSYSMOMMAG - Magnitude of S/C complete Angular Momentum)  */
    { .DataType = LC_DATA_FLOAT_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9C3, .WatchpointOffset = 120, .BitMask = LC_NO_BITMASK, .ComparisonValue.Float32 = 160, .CustomFuncArgument = 0},

    /*  103 (GC_SD_FDCFAULTDELTAMOMWHLMODE - SD Fault state - Failure Detection System Momentum Magnitude change (from one cycle) too high for wheel control mode)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 16, .BitMask = 0x100, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x100, .CustomFuncArgument = 0},

    /*  104 (GC_SD_FDCFAULTDELTAMOMTHRMODE - SD Fault state - Failure Detection System Momentum Magnitude change (from one cycle) too high for thruster control mode )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 16, .BitMask = 0x200, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x200, .CustomFuncArgument = 0},

    /*  105 (GC_CL_FDCFAULTMSMATTERR - CL Fault set state - Mission Science Mode Attitude Error Check  )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D1, .WatchpointOffset = 12, .BitMask = 0x4, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x4, .CustomFuncArgument = 0},

    /*  106 (GC_CL_FDCFAULTDVMATTERR - CL Fault set state - DeltaV Mode Attitude Error Check)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D1, .WatchpointOffset = 12, .BitMask = 0x10, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x10, .CustomFuncArgument = 0},

    /*  107 (GC_CL_FDCFAULTSLMATTERR - CL Fault set state - Slew Mode Attitude Error Check)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D1, .WatchpointOffset = 12, .BitMask = 0x8, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x8, .CustomFuncArgument = 0},

    /*  108 (GC_CL_SLMEXPIRED - SLEW Mode: Expected slew completion time has passed flag)  */
    { .DataType = LC_DATA_UBYTE, .OperatorID = LC_OPER_EQ, .MessageID = 0xA3D, .WatchpointOffset = 209, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned8in32.Unsigned8 = 1, .CustomFuncArgument = 0},

    /*  109 (GC_CL_FDCFAULTDHMTIMEOUT - CL Fault set state - DeltaH Timout Check)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D1, .WatchpointOffset = 12, .BitMask = 0x20, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x20, .CustomFuncArgument = 0},

    /*  110 (GC_SD_SSTAMMSSSUNDIRVALID - MSS Direction Vector Valid )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9C1, .WatchpointOffset = 128, .BitMask = 0x4, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x0, .CustomFuncArgument = 0},

    /*  111 (GC_SD_MSS_ENABLED - MSS Usage Enabled)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D5, .WatchpointOffset = 164, .BitMask = 0x1, .ComparisonValue.Unsigned32 = 0x1, .CustomFuncArgument = 0},

    /*  112 (GC_CL_SPMTIMEINMODE - SP Mode: Time in Mode)  */
    { .DataType = LC_DATA_FLOAT_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA3A, .WatchpointOffset = 104, .BitMask = LC_NO_BITMASK, .ComparisonValue.Float32 = 4500, .CustomFuncArgument = 0},

    /*  113 (GC_CL_GSPTIMEINMODE - GSP Mode: Time in Mode)  */
    { .DataType = LC_DATA_FLOAT_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA3B, .WatchpointOffset = 92, .BitMask = LC_NO_BITMASK, .ComparisonValue.Float32 = 4500, .CustomFuncArgument = 0},

    /*  114 (GC_SD_FDCFAULTSUNVECTOR - SD Fault state - Ephemeris Sun Vector and Sensor Implied Sun Vector Inconsistent )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 14, .BitMask = 0x200, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x200, .CustomFuncArgument = 0},

    /*  115 (GC_SD_FDCFAULTATTITUDE - SD Fault state - Failure Detection IRU propagated attitude vs ST measurement angular difference too high )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 16, .BitMask = 0x400, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x400, .CustomFuncArgument = 0},

    /*  116 (GC_SD_FDCFAULTKFRATE - SD Fault state - Failure Detection Kalman Filter Rate Correction Magnitude too high)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 16, .BitMask = 0x80, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x80, .CustomFuncArgument = 0},

    /*  117 (GC_SD_FDCFAULTSTRESIDUALREL - SD Fault state - Star Tracker Relative Residual Magnitude too large)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 12, .BitMask = 0x1, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1, .CustomFuncArgument = 0},

    /*  118 (GC_SD_KFKFINBACK - Kalman Filter in Background Mode )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9C2, .WatchpointOffset = 288, .BitMask = 0x4, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x0, .CustomFuncArgument = 0},

    /*  119 (GC_SD_FDCFAULTKFXATTCONV - SD Fault state - Attitude Diagonal X-Axis Covariance Term exceeds Convergence Tolerance)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 12, .BitMask = 0x4000, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x4000, .CustomFuncArgument = 0},

    /*  120 (GC_SD_FDCFAULTKFYATTCONV - SD Fault state - Attitude Diagonal Y-Axis Covariance Term exceeds Convergence Tolerance)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 12, .BitMask = 0x8000, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x8000, .CustomFuncArgument = 0},

    /*  121 (GC_SD_FDCFAULTKFZATTCONV - SD Fault state - Attitude Diagonal Z-Axis Covariance Term exceeds Convergence Tolerance)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 14, .BitMask = 0x1, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1, .CustomFuncArgument = 0},

    /*  122 (GC_SD_FDCFAULTKFXBIASCONV - SD Fault state - Bias Diagonal X-Axis Covariance Term exceeds Convergence Tolerance)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 14, .BitMask = 0x2, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x2, .CustomFuncArgument = 0},

    /*  123 (GC_SD_FDCFAULTKFYBIASCONV - SD Fault state - Bias Diagonal Y-Axis Covariance Term exceeds Convergence Tolerance)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 14, .BitMask = 0x4, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x4, .CustomFuncArgument = 0},

    /*  124 (GC_SD_FDCFAULTKFZBIASCONV - SD Fault state - Bias Diagonal Z-Axis Covariance Term exceeds Convergence Tolerance)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 14, .BitMask = 0x8, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x8, .CustomFuncArgument = 0},

    /*  125 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  126 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  127 (GC_DI_FDCFAULTATB1553MSG - DI Fault state - ATB raw 1553 data not received by XB )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 12, .BitMask = 0x8, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x8, .CustomFuncArgument = 0},

    /*  128 (GC_SCOMA_SCRATCHPAD4 - SCOMA SCRATCHPAD REGISTER 4  - REG 0x26)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x86F, .WatchpointOffset = 162, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0xB86A, .CustomFuncArgument = 0},

    /*  129 (GC_SCOMB_SCRATCHPAD4 - SCOMB SCRATCHPAD REGISTER 4  - REG 0x26)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x879, .WatchpointOffset = 162, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0xB86A, .CustomFuncArgument = 0},

    /*  130 (GC_XB_SCRETRYFAILS_PROPA1 - XB Spacecraft Failure Counts (RT 8 = PROPA1))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A8, .WatchpointOffset = 50, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 5, .CustomFuncArgument = 0},

    /*  131 (GC_XB_SCRETRYFAILS_PROPA2 - XB Spacecraft Failure Counts (RT 10 = PROPA2))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A8, .WatchpointOffset = 54, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 5, .CustomFuncArgument = 0},

    /*  132 (GC_DI_FDCFAULTGPSGEONSNODATA - DI Fault state - GPS GEONS data not available )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 22, .BitMask = 0x100, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x100, .CustomFuncArgument = 0},

    /*  133 (GC_STRB_HK_MODE - Current operating mode)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_NE, .MessageID = 0xA21, .WatchpointOffset = 24, .BitMask = 0x7, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x2, .CustomFuncArgument = 0},

    /*  134 (GC_DI_FDCFAULTGPSPTSOLNODATA - DI Fault state - GPS point solution data not available )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 22, .BitMask = 0x200, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x200, .CustomFuncArgument = 0},

    /*  135 (GC_DI_FDCFAULTGPSPTSOLDIFF - DI Fault state - GPS point solution data difference check fail (curr/previous position or velocity vector diff mag too large))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 22, .BitMask = 0x800, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x800, .CustomFuncArgument = 0},

    /*  136 (GC_DI_FDCFAULTGPSGEONSDIFF - DI Fault state - GPS GEONS data difference check fail (curr/previous position or velocity vector diff mag too large))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 22, .BitMask = 0x400, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x400, .CustomFuncArgument = 0},

    /*  137 (GC_DI_FDCFAULTIRU1553MSG - DI Fault state - IRU raw 1553 data not received by XB )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 14, .BitMask = 0x40, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x40, .CustomFuncArgument = 0},

    /*  138 (GC_DI_FDCFAULTIRUTIMEDELTA - DI Fault state - Time from IRU has invalid delta )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 14, .BitMask = 0x20, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x20, .CustomFuncArgument = 0},

    /*  139 (GC_DI_IRUDATAVALID - IRU data valid (VALID = all validity checks are VALID))  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D3, .WatchpointOffset = 156, .BitMask = 0x4, .ComparisonValue.Unsigned32 = 0x0, .CustomFuncArgument = 0},

    /*  140 (GC_SD_FDCFAULTGYRORATE - SD Fault state - Gyro rates compared against other gyro rates are inconsistent )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D0, .WatchpointOffset = 14, .BitMask = 0x100, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x100, .CustomFuncArgument = 0},

    /*  141 (GC_CG_FDCFAULTMTBCURR - CG Fault state - MTB Current fault (Any axis))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D2, .WatchpointOffset = 12, .BitMask = 0x20, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x20, .CustomFuncArgument = 0},

    /*  142 (DATASTALE - DI RW Data Stale (no XB message or missing RW 1553 data))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xAB3, .WatchpointOffset = 30, .BitMask = 0x1, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1, .CustomFuncArgument = 0},

    /*  143 (DATASTALE - DI RW Data Stale (no XB message or missing RW 1553 data))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xAB3, .WatchpointOffset = 30, .BitMask = 0x1, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1, .CustomFuncArgument = 0},

    /*  144 (DATASTALE - DI RW Data Stale (no XB message or missing RW 1553 data))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xAB3, .WatchpointOffset = 30, .BitMask = 0x1, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1, .CustomFuncArgument = 0},

    /*  145 (DATASTALE - DI RW Data Stale (no XB message or missing RW 1553 data))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xAB3, .WatchpointOffset = 30, .BitMask = 0x1, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1, .CustomFuncArgument = 0},

    /*  146 (DATASTALE - DI RW Data Stale (no XB message or missing RW 1553 data))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xAB3, .WatchpointOffset = 30, .BitMask = 0x1, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1, .CustomFuncArgument = 0},

    /*  147 (GC_CG_FDCFAULTRW1TRQMOM - CG Fault state - RW Torque / Momentum compare fault (RW 1))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D2, .WatchpointOffset = 12, .BitMask = 0x400, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x400, .CustomFuncArgument = 0},

    /*  148 (GC_CG_FDCFAULTRW2TRQMOM - CG Fault state - RW Torque / Momentum compare fault (RW 2))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D2, .WatchpointOffset = 12, .BitMask = 0x800, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x800, .CustomFuncArgument = 0},

    /*  149 (GC_CG_FDCFAULTRW3TRQMOM - CG Fault state - RW Torque / Momentum compare fault (RW 3))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D2, .WatchpointOffset = 12, .BitMask = 0x1000, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1000, .CustomFuncArgument = 0},

    /*  150 (GC_CG_FDCFAULTRW4TRQMOM - CG Fault state - RW Torque / Momentum compare fault (RW 4))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D2, .WatchpointOffset = 12, .BitMask = 0x2000, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x2000, .CustomFuncArgument = 0},

    /*  151 (GC_CG_FDCFAULTRW5TRQMOM - CG Fault state - RW Torque / Momentum compare fault (RW 5))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9D2, .WatchpointOffset = 12, .BitMask = 0x4000, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x4000, .CustomFuncArgument = 0},

    /*  152 (GC_XB_SCRETRYFAILS_RWA1 - XB Spacecraft Failure Counts (RT 24 = RWA1))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_CUSTOM, .MessageID = 0x9A8, .WatchpointOffset = 82, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 0, .CustomFuncArgument = 0},

    /*  153 (GC_DI_FDCFAULTST1553MSG - DI Fault state - ST raw 1553 data not received by XB )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 20, .BitMask = 0x200, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x200, .CustomFuncArgument = 0},

    /*  154 (GC_DI_FDCFAULTSTDELTA - DI Fault state - Delta angle limit exceeded for ST )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 20, .BitMask = 0x100, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x100, .CustomFuncArgument = 0},

    /*  155 (GC_STRA_HK_MODE - Current operating mode)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_NE, .MessageID = 0xA1E, .WatchpointOffset = 24, .BitMask = 0x7, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x2, .CustomFuncArgument = 0},

    /*  156 (GC_SD_STEARTHOCCULT - Star Tracker Occulted by Earth)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_NE, .MessageID = 0x9D5, .WatchpointOffset = 164, .BitMask = 0x800000, .ComparisonValue.Unsigned32 = 0x800000, .CustomFuncArgument = 0},

    /*  157 (GC_SD_STSUNOCCULT - Star Tracker Occulted by Sun)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_NE, .MessageID = 0x9D5, .WatchpointOffset = 164, .BitMask = 0x1000000, .ComparisonValue.Unsigned32 = 0x1000000, .CustomFuncArgument = 0},

    /*  158 (GC_SD_STLUNAROCCULT - Star Tracker Occulted by Moon)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_NE, .MessageID = 0x9D5, .WatchpointOffset = 164, .BitMask = 0x2000000, .ComparisonValue.Unsigned32 = 0x2000000, .CustomFuncArgument = 0},

    /*  159 (GC_DI_FDCFAULTTAMMTBFIELDMIN - DI Fault state - TAM Measured Mag Field below tbl-defined minimum )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 22, .BitMask = 0x1, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1, .CustomFuncArgument = 0},

    /*  160 (GC_DI_FDCFAULTTAMMTBFIELDMAX - DI Fault state - TAM Measured Mag Field above tbl-defined maximum )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 22, .BitMask = 0x2, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x2, .CustomFuncArgument = 0},

    /*  161 (GC_DI_FDCFAULTSAPYMINANGLE - DI Fault state - SA +Y gimbal below minimum valid angle     )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 20, .BitMask = 0x10, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x10, .CustomFuncArgument = 0},

    /*  162 (GC_DI_FDCFAULTSAMYMINANGLE - DI Fault state - SA -Y gimbal below minimum valid angle     )  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9CE, .WatchpointOffset = 20, .BitMask = 0x20, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x20, .CustomFuncArgument = 0},

    /*  163 (GC_PS_PROP_HTR_CTRL_STATUS - PS Propulsion Tank Heather Control status)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A5, .WatchpointOffset = 266, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 0, .CustomFuncArgument = 0},

    /*  164 (GC_GMI_HSD_CAT1_ERR_ID - Category 1 Error ID - Power Off)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_NE, .MessageID = 0x890, .WatchpointOffset = 16, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 0, .CustomFuncArgument = 0},

    /*  165 (GC_GM_GMIKEEPALIVESTALE - GM GMI instrument Keep Alive Counter stale flag)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A3, .WatchpointOffset = 56, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 1, .CustomFuncArgument = 0},

    /*  166 (GC_XB_INRETRYFAILS_GMI - XB Spacecraft Failure Counts (RT 5 = GMI))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A8, .WatchpointOffset = 116, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 50, .CustomFuncArgument = 0},

    /*  167 (GC_XB_INRETRYFAILS_GMI - XB Spacecraft Failure Counts (RT 5 = GMI))  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A8, .WatchpointOffset = 116, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 150, .CustomFuncArgument = 0},

    /*  168 (GC_MACA_GMI_RS_RCVR_T - MAC A GMI - RS Receiver Box Temp (J231:21)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0xA00, .WatchpointOffset = 308, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x3E6, .CustomFuncArgument = 0},

    /*  169 (GC_MACA_GMI_SMA_T - MAC A GMI - SMA Temp (J231:17)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0xA00, .WatchpointOffset = 304, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x744, .CustomFuncArgument = 0},

    /*  170 (GC_MACA_GMI_ICA_T - MAC A GMI - ICA Radiator Temp (J231:19)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0xA00, .WatchpointOffset = 306, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x78F, .CustomFuncArgument = 0},

    /*  171 (GC_MACA_GMI_RS_RCVR_T - MAC A GMI - RS Receiver Box Temp (J231:21)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA00, .WatchpointOffset = 308, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xFDC, .CustomFuncArgument = 0},

    /*  172 (GC_MACA_GMI_SMA_T - MAC A GMI - SMA Temp (J231:17)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA00, .WatchpointOffset = 304, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xFDC, .CustomFuncArgument = 0},

    /*  173 (GC_MACA_GMI_ICA_T - MAC A GMI - ICA Radiator Temp (J231:19)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA00, .WatchpointOffset = 306, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0xFFE, .CustomFuncArgument = 0},

    /*  174 (P200_1553_XFER_STATUS - 1553 Transfer Status Flags)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xA00, .WatchpointOffset = 12, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0},

    /*  175 (P202_1553_XFER_STATUS - 1553 Transfer Status Flags)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xA02, .WatchpointOffset = 12, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0},

    /*  176 (P216_1553_XFER_STATUS - 1553 Transfer Status Flags)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xA16, .WatchpointOffset = 12, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0},

    /*  177 (P219_1553_XFER_STATUS - 1553 Transfer Status Flags)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xA19, .WatchpointOffset = 12, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0},

    /*  178 (GC_PS_PSE_TLMSTALE - PS PSE Telemetry Stale Flag)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A5, .WatchpointOffset = 20, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 0, .CustomFuncArgument = 0},

    /*  179 (P1F0_1553_XFER_STATUS - 1553 Transfer Status Flags)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9F0, .WatchpointOffset = 12, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0},

    /*  180 (GC_MACA_HGAS_ELBOW_DMPR_T - MAC A HGAS Elbow Hinge Damper Temp (J231:41)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA00, .WatchpointOffset = 328, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x890, .CustomFuncArgument = 0},

    /*  181 (GC_MACA_HGAS_SHOULDER_DMPR_T - MAC A HGAS Shoulder Hinge Damper Temp (J231:45)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0xA00, .WatchpointOffset = 332, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x890, .CustomFuncArgument = 0},

    /*  182 (GC_MACA_HGAS_ELBOW_DMPR_T - MAC A HGAS Elbow Hinge Damper Temp (J231:41)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0xA00, .WatchpointOffset = 328, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x204, .CustomFuncArgument = 0},

    /*  183 (GC_MACA_HGAS_SHOULDER_DMPR_T - MAC A HGAS Shoulder Hinge Damper Temp (J231:45)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LT, .MessageID = 0xA00, .WatchpointOffset = 332, .BitMask = 0xFFF, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x204, .CustomFuncArgument = 0},

    /*  184 (GC_SW_UNKNOWNPKTRCVCNT - Packets received with an unknown PID)  */
    { .DataType = LC_DATA_UDWORD_BE, .OperatorID = LC_OPER_GT, .MessageID = 0x9A7, .WatchpointOffset = 44, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 100, .CustomFuncArgument = 0},

    /*  185 (GC_SW_PT3PARITY_ERRCNT - Parity Error Count)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0x9A7, .WatchpointOffset = 176, .BitMask = 0x1C0, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1C0, .CustomFuncArgument = 0},

    /*  186 (GC_SW_PT3FLOWCNTLUNDER_ERR - Flow Control Underflow Error)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A7, .WatchpointOffset = 176, .BitMask = 0x4, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x4, .CustomFuncArgument = 0},

    /*  187 (GC_SW_PT3FLOWCNTLOVER_ERR - Flow Control Overflow Error)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A7, .WatchpointOffset = 176, .BitMask = 0x2, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x2, .CustomFuncArgument = 0},

    /*  188 (GC_SW_PT4PARITY_ERRCNT - Parity Error Count)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_GE, .MessageID = 0x9A7, .WatchpointOffset = 178, .BitMask = 0x1C0, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x1C0, .CustomFuncArgument = 0},

    /*  189 (GC_SW_PT4FLOWCNTLUNDER_ERR - Flow Control Underflow Error)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A7, .WatchpointOffset = 178, .BitMask = 0x4, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x4, .CustomFuncArgument = 0},

    /*  190 (GC_SW_PT4FLOWCNTLOVER_ERR - Flow Control Overflow Error)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0x9A7, .WatchpointOffset = 178, .BitMask = 0x2, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x2, .CustomFuncArgument = 0},

    /*  191 (GC_PS_AMP_HR_IN_PSEA_BATT1 - PSE-A Batt 1 Amp-hours In)  */
    { .DataType = LC_DATA_FLOAT_BE, .OperatorID = LC_OPER_CUSTOM, .MessageID = 0x9A5, .WatchpointOffset = 104, .BitMask = LC_NO_BITMASK, .ComparisonValue.Float32 = 0, .CustomFuncArgument = 0},

    /*  192 (GC_PSEB_SAM1_A_SLCT_N - SAM-1 A Select Inverted Bit Status)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xA1D, .WatchpointOffset = 28, .BitMask = 0x100, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x0, .CustomFuncArgument = 0},

    /*  193 (GC_PSEB_SAM1_B_SLCT_N - SAM-1 B Select Inverted Bit Status)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_EQ, .MessageID = 0xA1D, .WatchpointOffset = 28, .BitMask = 0x200, .ComparisonValue.Unsigned16in32.Unsigned16 = 0x200, .CustomFuncArgument = 0},

    /*  194 (GC_GMI_HSD_SMA_BEARING_TMP_RAW -  GMI SMA Bearing Temperature)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LE, .MessageID = 0x890, .WatchpointOffset = 306, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 37084, .CustomFuncArgument = 0},

    /*  195 (GC_GMI_HSD_SMA_MTR_TEMP_RAW -  GMI SMA Motor Temperature)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LE, .MessageID = 0x890, .WatchpointOffset = 312, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 37084, .CustomFuncArgument = 0},

    /*  196 (GC_GMI_HSD_SMA_SLPRHTR_TMP_RAW -  GMI SMA Slip Ring Heater Temperature)  */
    { .DataType = LC_DATA_UWORD_BE, .OperatorID = LC_OPER_LE, .MessageID = 0x890, .WatchpointOffset = 346, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned16in32.Unsigned16 = 37084, .CustomFuncArgument = 0},

    /*  197 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  198 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  199 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  200 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  201 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  202 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  203 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  204 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  205 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  206 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  207 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  208 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  209 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  210 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  211 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  212 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  213 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  214 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  215 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  216 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  217 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  218 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  219 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  220 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  221 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  222 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  223 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  224 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  225 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  226 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  227 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  228 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  229 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  230 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  231 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  232 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  233 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  234 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  235 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  236 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  237 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  238 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  239 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  240 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  241 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  242 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  243 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  244 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  245 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  246 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  247 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  248 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  249 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  250 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  251 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  252 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  253 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  254 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 

    /*  255 (UNUSED - ) */
    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0, .CustomFuncArgument = 0 }, 


} ; /* end LC_WDT */ 

/************************/ 
/*  End of File */ 
/************************/ 
