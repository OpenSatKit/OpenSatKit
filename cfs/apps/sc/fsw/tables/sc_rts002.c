/************************************************************************
** OSK Payload Power Off Command Sequence
** - Payload is power is simulaed by the PL_SIM app
** - Science data collection is performed by the PL_MGR app
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */
#include "lc_msgdefs.h"         /* defines SC command code values */
#include "lc_msgids.h"          /* defines SC packet msg ID's */
#include "pl_sim_msgids.h"      /* PL_SIM msg ID's */
#include "pl_mgr_msgids.h"      /* PL_MGR msg ID's */



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

#define PL_MGR_START_SCI_CMD_FC  2
#define PL_MGR_STOP_SCI_CMD_FC   3

#define PL_MGR_START_SCI_XSUM 0x00E5
#define PL_MGR_STOP_SCI_XSUM  0x00E4


#define PL_SIM_POWER_ON_CMD_FC     2
#define PL_SIM_POWER_OFF_CMD_FC    3
#define PL_SIM_POWER_RESET_CMD_FC  4

#define PL_SIM_POWER_ON_XSUM    0x00D5
#define PL_SIM_POWER_OFF_XSUM   0x00D4
#define PL_SIM_POWER_RESET_XSUM 0x00D3


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
    "RTS_Table002", "SC.RTS_TBL002", "Payload Fault Recovery",
    "sc_rts002.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};


/*
** RTS Table Data
*/
uint16 RTS_Table002[SC_RTS_BUFF_SIZE] =
{
  /*  cmd time,  <---------------------------- cmd pkt primary header ---------------------------->  <----- cmd pkt 2nd header ---->   <-- opt data ---> */
      DELAY_0, CFE_MAKE_BIG16(PL_MGR_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD_LEN_NO_PARAM), ((PL_MGR_STOP_SCI_CMD_FC  << 8) | PL_MGR_STOP_SCI_XSUM),
      DELAY_4, CFE_MAKE_BIG16(PL_SIM_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD_LEN_NO_PARAM), ((PL_SIM_POWER_OFF_CMD_FC << 8) | PL_SIM_POWER_OFF_XSUM)
};

/************************/
/*  End of File Comment */
/************************/
