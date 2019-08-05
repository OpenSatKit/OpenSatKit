/************************************************************************
 ** File:
 **   $Id: cs_apptbl.c 1.1.1.1 2015/03/03 11:57:42EST sstrege Exp  $
 **
 **   Copyright © 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement. 
 **
 ** Purpose: 
 **  The CFS Checksum (CS) Application Unit Test Apps Table Definition
 **
 ** Notes:
 **  This is a partially-populated table with duplicate entries.
 **  This table should be located in unit_test\cf\apps\DupEntryNonFullTable
 **
 ** $Log: cs_apptbl.c  $
 ** Revision 1.1.1.1 2015/03/03 11:57:42EST sstrege 
 ** Added copyright information
 ** Revision 1.1 2011/09/06 15:23:36EDT jmdagost 
 ** Initial revision
 ** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/unit_test/cf/apps/DupEntryNonFullTable/project.pj
 ** Revision 1.1 2010/05/03 17:35:47EDT jmdagost 
 ** Initial revision
 ** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/unit_test/cf/apps/DupEntryNonFullTable/project.pj
 **
 *************************************************************************/

#include "cfe.h"
#include "cs_platform_cfg.h"
#include "cs_msgdefs.h"
#include "cs_tbldefs.h"
#include "cfe_tbl_filedef.h"

static CFE_TBL_FileDef_t CFE_TBL_FileDef =
{
    "CS_AppTable", "CS.DefAppTbl", "CS App Tbl",
    "cs_apptbl.tbl", sizeof (CS_Def_App_Table_Entry_t) * CS_MAX_NUM_APP_TABLE_ENTRIES
};

CS_Def_App_Table_Entry_t CS_AppTable[CS_MAX_NUM_APP_TABLE_ENTRIES]= 
{
    /*         State               Name   */
    /*   0 */ { CS_STATE_ENABLED,   "AA"    },
    /*   1 */ { CS_STATE_ENABLED,   "BB"    },
    /*   2 */ { CS_STATE_ENABLED,   "CC"    },
    /*   3 */ { CS_STATE_ENABLED,   "DD"    },
    /*   4 */ { CS_STATE_ENABLED,   "EE"    },
    /*   5 */ { CS_STATE_ENABLED,   "FF"    },
    /*   6 */ { CS_STATE_ENABLED,   "GG"    },  /* Entry duplicated below */
    /*   7 */ { CS_STATE_ENABLED,   "CS"    },
    /*   8 */ { CS_STATE_ENABLED,   "II"    },
    /*   9 */ { CS_STATE_ENABLED,   "JJ"    },  /* Entry duplicated below */
    /*  10 */ { CS_STATE_DISABLED,  "KK"    },  /* Entry disabled, duplicated below */
    /*  11 */ { CS_STATE_ENABLED,   "LL"    },
    /*  12 */ { CS_STATE_DISABLED,  "MM"    },  /* Entry disabled, duplicated below */
    /*  13 */ { CS_STATE_EMPTY,     "NN"    },
    /*  14 */ { CS_STATE_EMPTY,     ""      },
    /*  15 */ { CS_STATE_EMPTY,     ""      },
    /*  16 */ { CS_STATE_ENABLED,   "JJ"    },  /* Duplicate entry, marked as enabled */
    /*  17 */ { CS_STATE_EMPTY,     ""      },
    /*  18 */ { CS_STATE_DISABLED,  "KK"    },  /* Duplicate entry, marked as disabled */
    /*  19 */ { CS_STATE_DISABLED,  "NN"    },  /* Duplicate entry, marked as disabled */
    /*  20 */ { CS_STATE_EMPTY,     ""      },
    /*  21 */ { CS_STATE_EMPTY,     "GG"    },  /* Duplicate entry, marked as empty */
    /*  22 */ { CS_STATE_EMPTY,     "MM"    },  /* Duplicate entry, marked as empty */
    /*  23 */ { CS_STATE_EMPTY,     ""      },
};

/************************/
/*  End of File Comment */
/************************/
