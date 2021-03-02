/************************************************************************
** OSK ISIM Power Off Command Sequence
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */
#include "lc_msgdefs.h"         /* defines SC command code values */
#include "lc_msgids.h"          /* defines SC packet msg ID's */
#include "isim_msgids.h"        /* ISIM msg ID's */
#include "app_cfg.h"            /* ISIM cmd function codes */



/*
** Execution time for each sample command
*/
#define DELAY_0  0
#define DELAY_1  1
#define DELAY_2  2
#define DELAY_3  3
#define DELAY_4  4
#define DELAY_5  5


/*
** Calculate checksum for each sample command
*/

#define ISIM_PWR_ON_XSUM      0x00E3
#define ISIM_PWR_OFF_XSUM     0x00E2
#define ISIM_PWR_RESET_XSUM   0x00E1
#define ISIM_START_SCI_XSUM   0x00E0
#define ISIM_STOP_SCI_XSUM    0x00EF


/*
** Command packet segment flags and sequence counter
** - 2 bits of segment flags (0xC000 = start and end of packet)
** - 14 bits of sequence count (unused for command packets)
*/
#define PKT_FLAGS     0xC000


/*
** Length of cmd pkt data (in bytes minus one) that follows primary header (thus, 0xFFFF = 64k)
*/

#define CMD_LEN_NO_PARAM 1


/*
** cFE Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "RTS_Table006", "SC.RTS_TBL006", "ISIM Fault Recovery",
    "sc_rts006.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};


/*
** RTS Table Data
*/
uint16 RTS_Table006[SC_RTS_BUFF_SIZE] =
{
  /*  cmd time,  <---------------------------- cmd pkt primary header ---------------------------->  <----- cmd pkt 2nd header ---->   <-- opt data ---> */
      DELAY_0, CFE_MAKE_BIG16(ISIM_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD_LEN_NO_PARAM), ((ISIM_STOP_SCI_CMD_FC      << 8) | ISIM_STOP_SCI_XSUM),
      DELAY_4, CFE_MAKE_BIG16(ISIM_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD_LEN_NO_PARAM), ((ISIM_PWR_OFF_INSTR_CMD_FC << 8) | ISIM_PWR_OFF_XSUM)
};

/************************/
/*  End of File Comment */
/************************/
