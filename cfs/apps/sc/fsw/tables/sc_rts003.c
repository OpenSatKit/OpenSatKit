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
#include "hc_msgdefs.h"         /* defines SC command code values */
#include "hc_msgids.h"          /* defines SC packet msg ID's */
#include "lc_msgdefs.h"         /* defines SC command code values */
#include "lc_msgids.h"          /* defines SC packet msg ID's */



/*
** Execution time for each sample command
*/
#define CMD1_TIME     1
#define CMD2_TIME     1
#define CMD3_TIME     0
#define CMD4_TIME     0


/*
** Calculate checksum for each sample command
*/
#define CMD1_XSUM     0x0085
#define CMD2_XSUM     0x0096
#define CMD3_XSUM     0x0082
#define CMD4_XSUM     0x0083


/*
** Command packet segment flags and sequence counter
** - 2 bits of segment flags (0xC000 = start and end of packet)
** - 14 bits of sequence count (unused for command packets)
*/
#define PKT_FLAGS     0xC000


/*
** Length of cmd pkt data (in bytes minus one) that follows primary header (thus, 0xFFFF = 64k)
*/
#define CMD1_LENGTH   5
#define CMD2_LENGTH   2
#define CMD3_LENGTH   3
#define CMD4_LENGTH   3


/*
** Sample cFE Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "RTS_Table003", "SC.RTS_TBL003", "SC Sample RTS_TBL003",
    "sc_rts003.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};


/*
** Sample RTS Table Data
*/
uint16 RTS_Table003[SC_RTS_BUFF_SIZE] =
{
  /*  cmd time,  <---------------------------- cmd pkt primary header ---------------------------->  <----- cmd pkt 2nd header ---->   <-- opt data ---> */
     CMD3_TIME,  CFE_MAKE_BIG16(HC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD3_LENGTH), ((HC_SET_STATUS_CC << 8) | CMD3_XSUM), 0x0101,
     CMD4_TIME,  CFE_MAKE_BIG16(HC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD4_LENGTH), ((HC_SET_STATUS_CC << 8) | CMD4_XSUM), 0x0100 ,
     CMD2_TIME,  CFE_MAKE_BIG16(SIM_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD2_LENGTH), ((SIM_HEATER_MISBEHAVE_CC << 8) | CMD2_XSUM), 1,
     CMD1_TIME,  CFE_MAKE_BIG16(LC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD1_LENGTH), ((LC_SET_LC_STATE_CC << 8) | CMD1_XSUM), 1, 0 
};

/************************/
/*  End of File Comment */
/************************/
