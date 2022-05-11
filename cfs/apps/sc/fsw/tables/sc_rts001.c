/************************************************************************
** OSK Heater Control Demo
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */
#include "cfe_msgids.h"

/*
** Execution time for each sample command
*/
#define DELAY_0  0
#define DELAY_1  1
#define DELAY_2  2
#define DELAY_3  3
#define DELAY_4  4
#define DELAY_5  5

#define CFE_ES_NOOP_CC 0

/*
** Calculate checksum for each sample command
*/
#define CMD1_XSUM  0x0088
#define CMD2_XSUM  0x0020

/*
** Command packet segment flags and sequence counter
** - 2 bits of segment flags (0xC000 = start and end of packet)
** - 14 bits of sequence count (unused for command packets)
*/
#define PKT_FLAGS     0xC000


/*
** Length of cmd pkt data (in bytes minus one) that follows primary header (thus, 0xFFFF = 64k)
*/
#define CMD1_LENGTH   3
#define CMD2_LENGTH   1

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
     DELAY_0,  CFE_MAKE_BIG16(SC_CMD_MID),     CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD1_LENGTH), ((SC_ENABLE_RTS_CC << 8) | CMD1_XSUM), 2,
     DELAY_1,  CFE_MAKE_BIG16(CFE_ES_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD2_LENGTH), ((CFE_ES_NOOP_CC   << 8) | CMD2_XSUM)
};

/************************/
/*  End of File Comment */
/************************/
