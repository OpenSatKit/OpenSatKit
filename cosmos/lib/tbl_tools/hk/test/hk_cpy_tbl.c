/******************************************************************************
** hk_cpy_tbl.c
**
** Purpose:
**   Define the contents of the HK copy table that defines the contents of
**   each HK combo packet.
**
** Notes:
**   1. This table was automatically generated
**      from /mnt/hgfs/OpenSatKit/cosmos/lib/tbl_tools/HK/test/hk_cpy_tbl_test.json
**      at 2021-02-14 12:18:18 -0800 
**      and should NOT be manually modified.
**   2. The table entry MID symbols are resolved during the table generation
**      process so its not necessary to include headers from the apps that  
**      supply the data for the combo packets.
**
** Template parameters:
**   <src-file>   JSON file that defines the copy table combo packets
**   <date-time>  The date and time when the file was generated.
**
******************************************************************************/

/*
** Includes
*/

#include "hk_platform_cfg.h"
#include "hk_utils.h"
#include "hk_tbldefs.h"
#include "cfe_tbl_filedef.h"


/*
** Exported Data
*/

/*
** Default table data
*/
hk_copy_table_entry_t      HK_CopyTable[HK_COPY_TABLE_ENTRIES] =
{
/*         inputMid        inputOffset     outputMid    outputOffset  numBytes*/



{ 0x0800,  12, 0x089C,  12,   1 },   /* 000: {"tlm-mid"=>"CFE_ES_HK_TLM_MID", "tlm-item"=>"CFE_ES HK_TLM_PKT CMD_VALID_COUNT"}          */
{ 0x0800,  13, 0x089C,  13,   1 },   /* 001: {"tlm-mid"=>"CFE_ES_HK_TLM_MID", "tlm-item"=>"CFE_ES HK_TLM_PKT CMD_ERROR_COUNT"}          */
{ 0x0800,  14, 0x089C,  14,   2 },   /* 002: {"tlm-mid"=>"CFE_ES_HK_TLM_MID", "tlm-item"=>"CFE_ES HK_TLM_PKT CFE_CORE_CHECKSUM"}        */
{ 0x0801,  12, 0x089C,  16,   1 },   /* 003: {"tlm-mid"=>"CFE_EVS_HK_TLM_MID", "tlm-item"=>"CFE_EVS HK_TLM_PKT CMD_VALID_COUNT"}        */
{ 0x0801,  13, 0x089C,  17,   1 },   /* 004: {"tlm-mid"=>"CFE_EVS_HK_TLM_MID", "tlm-item"=>"CFE_EVS HK_TLM_PKT CMD_ERROR_COUNT"}        */
{ 0x0801,  14, 0x089C,  18,   1 },   /* 005: {"tlm-mid"=>"CFE_EVS_HK_TLM_MID", "tlm-item"=>"CFE_EVS HK_TLM_PKT MSG_FMT_MODE"}           */
{ 0x0801,  15, 0x089C,  19,   1 },   /* 006: {"tlm-mid"=>"CFE_EVS_HK_TLM_MID", "tlm-item"=>"CFE_EVS HK_TLM_PKT MSG_TRUNC_CTR"}          */
{ 0x0803,  12, 0x089C,  20,   1 },   /* 007: {"tlm-mid"=>"CFE_SB_HK_TLM_MID", "tlm-item"=>"CFE_SB HK_TLM_PKT CMD_VALID_COUNT"}          */
{ 0x0803,  13, 0x089C,  21,   1 },   /* 008: {"tlm-mid"=>"CFE_SB_HK_TLM_MID", "tlm-item"=>"CFE_SB HK_TLM_PKT CMD_ERROR_COUNT"}          */
{ 0x0803,  14, 0x089C,  22,   1 },   /* 009: {"tlm-mid"=>"CFE_SB_HK_TLM_MID", "tlm-item"=>"CFE_SB HK_TLM_PKT NO_SUBSCRIBE_CNT"}         */
{ 0x0803,  15, 0x089C,  23,   1 },   /* 010: {"tlm-mid"=>"CFE_SB_HK_TLM_MID", "tlm-item"=>"CFE_SB HK_TLM_PKT MSG_SEND_ERR_CNT"}         */
{ 0x0804,  12, 0x089C,  24,   1 },   /* 011: {"tlm-mid"=>"CFE_TBL_HK_TLM_MID", "tlm-item"=>"CFE_TBL HK_TLM_PKT CMD_VALID_COUNT"}        */
{ 0x0804,  13, 0x089C,  25,   1 },   /* 012: {"tlm-mid"=>"CFE_TBL_HK_TLM_MID", "tlm-item"=>"CFE_TBL HK_TLM_PKT CMD_ERROR_COUNT"}        */
{ 0x0804,  14, 0x089C,  26,   2 },   /* 013: {"tlm-mid"=>"CFE_TBL_HK_TLM_MID", "tlm-item"=>"CFE_TBL HK_TLM_PKT NUM_TABLES"}             */
{ 0x0805,  12, 0x089C,  28,   1 },   /* 014: {"tlm-mid"=>"CFE_TIME_HK_TLM_MID", "tlm-item"=>"CFE_TIME HK_TLM_PKT CMD_VALID_COUNT"}      */
{ 0x0805,  13, 0x089C,  29,   1 },   /* 015: {"tlm-mid"=>"CFE_TIME_HK_TLM_MID", "tlm-item"=>"CFE_TIME HK_TLM_PKT CMD_ERROR_COUNT"}      */
{ 0x0805,  14, 0x089C,  30,   2 },   /* 016: {"tlm-mid"=>"CFE_TIME_HK_TLM_MID", "tlm-item"=>"CFE_TIME HK_TLM_PKT CLOCK_STATE_FLAGS"}    */
{ 0x09C0,  18, 0x089D,  12,   1 },   /* 017: {"tlm-mid"=>"ISIM_HK_TLM_MID", "tlm-item"=>"ISIM HK_TLM_PKT INSTR_STATE"}                  */
{ 0x09C0,  19, 0x089D,  13,   1 },   /* 018: {"tlm-mid"=>"ISIM_HK_TLM_MID", "tlm-item"=>"ISIM HK_TLM_PKT SCI_STATE"}                    */
{ 0x09C0,  20, 0x089D,  14,   1 },   /* 019: {"tlm-mid"=>"ISIM_HK_TLM_MID", "tlm-item"=>"ISIM HK_TLM_PKT FAULT"}                        */
{ 0x09C0,  21, 0x089D,  15,   1 },   /* 020: {"tlm-mid"=>"ISIM_HK_TLM_MID", "tlm-item"=>"ISIM HK_TLM_PKT SPARE"}                        */
{ 0x09D0,  22, 0x089D,  16,   2 },   /* 021: {"tlm-mid"=>"F42_HK_TLM_MID", "tlm-item"=>"F42 HK_TLM_PKT CTRL_MODE"}                      */
{ 0x09D1,  12, 0x089D,  18,   4 },   /* 022: {"tlm-mid"=>"F42_CTRL_TLM_MID", "tlm-item"=>"F42 CONTROL_PKT WBN_1"}                       */
{ 0x09D1,  16, 0x089D,  22,   4 },   /* 023: {"tlm-mid"=>"F42_CTRL_TLM_MID", "tlm-item"=>"F42 CONTROL_PKT WBN_2"}                       */
{ 0x09D1,  20, 0x089D,  26,   4 },   /* 024: {"tlm-mid"=>"F42_CTRL_TLM_MID", "tlm-item"=>"F42 CONTROL_PKT WBN_3"}                       */
{ 0x09D1,  36, 0x089D,  30,   4 },   /* 025: {"tlm-mid"=>"F42_CTRL_TLM_MID", "tlm-item"=>"F42 CONTROL_PKT QBR_1"}                       */
{ 0x09D1,  40, 0x089D,  34,   4 },   /* 026: {"tlm-mid"=>"F42_CTRL_TLM_MID", "tlm-item"=>"F42 CONTROL_PKT QBR_2"}                       */
{ 0x09D1,  44, 0x089D,  38,   4 },   /* 027: {"tlm-mid"=>"F42_CTRL_TLM_MID", "tlm-item"=>"F42 CONTROL_PKT QBR_3"}                       */
{ 0x09D1,  48, 0x089D,  42,   4 },   /* 028: {"tlm-mid"=>"F42_CTRL_TLM_MID", "tlm-item"=>"F42 CONTROL_PKT QBR_4"}                       */
{ 0x09D1,  52, 0x089D,  46,   4 },   /* 029: {"tlm-mid"=>"F42_CTRL_TLM_MID", "tlm-item"=>"F42 CONTROL_PKT THERR_X"}                     */
{ 0x09D1,  56, 0x089D,  50,   4 },   /* 030: {"tlm-mid"=>"F42_CTRL_TLM_MID", "tlm-item"=>"F42 CONTROL_PKT THERR_Y"}                     */
{ 0x09D1,  60, 0x089D,  54,   4 },   /* 031: {"tlm-mid"=>"F42_CTRL_TLM_MID", "tlm-item"=>"F42 CONTROL_PKT THERR_Z"}                     */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 032: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 033: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 034: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 035: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 036: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 037: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 038: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 039: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 040: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 041: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 042: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 043: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 044: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 045: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 046: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 047: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 048: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 049: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 050: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 051: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 052: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 053: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 054: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 055: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 056: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 057: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 058: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 059: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 060: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 061: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 062: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 063: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 064: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 065: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 066: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 067: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 068: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 069: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 070: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 071: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 072: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 073: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 074: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 075: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 076: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 077: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 078: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 079: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 080: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 081: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 082: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 083: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 084: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 085: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 086: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 087: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 088: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 089: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 090: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 091: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 092: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 093: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 094: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 095: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 096: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 097: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 098: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 099: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 100: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 101: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 102: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 103: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 104: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 105: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 106: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 107: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 108: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 109: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 110: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 111: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 112: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 113: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 114: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 115: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 116: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 117: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 118: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 119: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 120: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 121: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 122: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 123: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 124: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 125: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 126: Unused entry */
{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 },   /* 127: Unused entry */

} ; /* End HK_CopyTable */

/*
** Table file header
*/
CFE_TBL_FILEDEF(HK_CopyTable, HK.CopyTable, HK Copy Tbl, hk_cpy_tbl.tbl)

