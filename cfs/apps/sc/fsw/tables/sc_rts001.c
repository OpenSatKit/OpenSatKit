/************************************************************************
** OSK Heater Control Demo
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */


/*
** Execution time for each sample command
*/
#define CMD2_TIME     0
#define CMD4_TIME     5


/*
** Calculate checksum for each sample command
*/
#define CMD2_XSUM     0x0088
#define CMD4_XSUM     0x0089


/*
** Command packet segment flags and sequence counter
** - 2 bits of segment flags (0xC000 = start and end of packet)
** - 14 bits of sequence count (unused for command packets)
*/
#define PKT_FLAGS     0xC000


/*
** Length of cmd pkt data (in bytes minus one) that follows primary header (thus, 0xFFFF = 64k)
*/
#define CMD2_LENGTH   3
#define CMD4_LENGTH   3


/*
** Sample cFE Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "RTS_Table001", "SC.RTS_TBL001", "SC Sample RTS_TBL001",
    "sc_rts001.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};


/*
** Sample RTS Table Data
*/
uint16 RTS_Table001[SC_RTS_BUFF_SIZE] =
{
  /*  cmd time,  <---------------------------- cmd pkt primary header ---------------------------->  <----- cmd pkt 2nd header ---->   <-- opt data ---> */
     CMD2_TIME,  CFE_MAKE_BIG16(SC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD2_LENGTH), ((SC_ENABLE_RTS_CC << 8) | CMD2_XSUM), 2,
     CMD4_TIME,  CFE_MAKE_BIG16(SC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD4_LENGTH), ((SC_ENABLE_RTS_CC << 8) | CMD4_XSUM), 3
};

/************************/
/*  End of File Comment */
/************************/
