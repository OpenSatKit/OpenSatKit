/************************************************************************
** OSK Heater Control Demo
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */
#include "sim_msgdefs.h"         /* defines SC command code values */
#include "sim_msgids.h"          /* defines SC packet msg ID's */
#include "lc_msgdefs.h"         /* defines SC command code values */
#include "lc_msgids.h"          /* defines SC packet msg ID's */


/*
** Execution time for each sample command
*/
#define CMD1_TIME     0
#define CMD2_TIME     120
#define CMD3_TIME     0


/*
** Calculate checksum for each sample command
*/
#define CMD1_XSUM     0x0097
#define CMD2_XSUM     0x0084
#define CMD3_XSUM     0x0085


/*
** Command packet segment flags and sequence counter
** - 2 bits of segment flags (0xC000 = start and end of packet)
** - 14 bits of sequence count (unused for command packets)
*/
#define PKT_FLAGS     0xC000


/*
** Length of cmd pkt data (in bytes minus one) that follows primary header (thus, 0xFFFF = 64k)
*/
#define CMD1_LENGTH   2
#define CMD2_LENGTH   5
#define CMD3_LENGTH   5


/*
** Sample cFE Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "RTS_Table002", "SC.RTS_TBL002", "SC Sample RTS_TBL002",
    "sc_rts002.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};


/*
** Sample RTS Table Data
*/
uint16 RTS_Table002[SC_RTS_BUFF_SIZE] =
{
  /*  cmd time,  <---------------------------- cmd pkt primary header ---------------------------->  <----- cmd pkt 2nd header ---->   <-- opt data ---> */
     CMD1_TIME,  CFE_MAKE_BIG16(SIM_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD1_LENGTH), ((SIM_HEATER_MISBEHAVE_CC << 8) | CMD1_XSUM), 0,
     CMD2_TIME,  CFE_MAKE_BIG16(LC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD2_LENGTH), ((LC_SET_AP_STATE_CC << 8) | CMD2_XSUM), 0, 1,
     CMD3_TIME,  CFE_MAKE_BIG16(LC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD3_LENGTH), ((LC_SET_AP_STATE_CC << 8) | CMD3_XSUM), 1, 1
};

/************************/
/*  End of File Comment */
/************************/
