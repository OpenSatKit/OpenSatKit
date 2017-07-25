/************************************************************************
 ** File:
 **   $Id: cs_apptbl.c 1.1.1.1 2015/03/03 11:57:45EST sstrege Exp  $
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
 **  This is a fully-populated table of valid entries, with all entries enabled.
 **  This table should be located in unit_test\cf\apps\CorrFullTable
 **
 ** $Log: cs_apptbl.c  $
 ** Revision 1.1.1.1 2015/03/03 11:57:45EST sstrege 
 ** Added copyright information
 ** Revision 1.1 2011/09/06 15:21:44EDT jmdagost 
 ** Initial revision
 ** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/unit_test/cf/apps/CorrFullTable/project.pj
 ** Revision 1.2 2010/05/03 17:30:04EDT jmdagost 
 ** Major updates to test table data for CS unit test changes.
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
    /*   0 */ { CS_STATE_ENABLED,   "CS"    },
    /*   1 */ { CS_STATE_ENABLED,   "AA"    },
    /*   2 */ { CS_STATE_ENABLED,   "BB"    },
    /*   3 */ { CS_STATE_ENABLED,   "CC"    },
    /*   4 */ { CS_STATE_ENABLED,   "DD"    },
    /*   5 */ { CS_STATE_ENABLED,   "EE"    },
    /*   6 */ { CS_STATE_ENABLED,   "FF"    },
    /*   7 */ { CS_STATE_ENABLED,   "GG"    },
    /*   8 */ { CS_STATE_ENABLED,   "HH"    },
    /*   9 */ { CS_STATE_ENABLED,   "II"    },
    /*  10 */ { CS_STATE_ENABLED,   "JJ"    },
    /*  11 */ { CS_STATE_ENABLED,   "KK"    },
    /*  12 */ { CS_STATE_ENABLED,   "LL"    },
    /*  13 */ { CS_STATE_ENABLED,   "MM"    },
    /*  14 */ { CS_STATE_ENABLED,   "NN"    },
    /*  15 */ { CS_STATE_ENABLED,   "OO"    },
    /*  16 */ { CS_STATE_ENABLED,   "PP"    },
    /*  17 */ { CS_STATE_ENABLED,   "QQ"    },
    /*  18 */ { CS_STATE_ENABLED,   "RR"    },
    /*  19 */ { CS_STATE_ENABLED,   "SS"    },
    /*  20 */ { CS_STATE_ENABLED,   "TT"    },
    /*  21 */ { CS_STATE_ENABLED,   "UU"    },
    /*  22 */ { CS_STATE_ENABLED,   "VV"    },
    /*  23 */ { CS_STATE_ENABLED,   "WW"    },
};

/************************/
/*  End of File Comment */
/************************/
