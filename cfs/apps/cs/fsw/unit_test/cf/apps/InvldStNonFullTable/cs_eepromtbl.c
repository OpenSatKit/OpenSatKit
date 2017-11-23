/************************************************************************
 ** File:
 **   $Id: cs_eepromtbl.c 1.1.1.1 2015/03/03 11:57:28EST sstrege Exp  $
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
 **  The CFS Checksum (CS) Application Unit Test EEPROM Table Definition
 **
 ** Notes:
 **  This is a partially-populated table with an invalid entry (state).
 **  This table should be located in unit_test\cf\apps\InvldStNonFullTable
 **  The unit test overwrites the StartAddress and NumBytes fields with
 **   valid values after the table is loaded.  Otherwise there is a risk
 **   of generating a segmentation fault.
 **
 ** $Log: cs_eepromtbl.c  $
 ** Revision 1.1.1.1 2015/03/03 11:57:28EST sstrege 
 ** Added copyright information
 ** Revision 1.1 2011/09/06 15:25:01EDT jmdagost 
 ** Initial revision
 ** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/unit_test/cf/apps/InvldStNonFullTable/project.pj
 ** Revision 1.2 2010/05/03 17:30:08EDT jmdagost 
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
    "CS_EepromTable", "CS.DefEepromTbl", "CS EEPROM Tbl",
    "cs_eepromtbl.tbl", sizeof (CS_Def_EepromMemory_Table_Entry_t) * CS_MAX_NUM_EEPROM_TABLE_ENTRIES
};

CS_Def_EepromMemory_Table_Entry_t CS_EepromTable[CS_MAX_NUM_EEPROM_TABLE_ENTRIES]= 
{ 
    /*         State             Filler   StartAddress     NumBytes */
    /*  0 */ { 0x13            , 0x1234,  0x00034020,      0x00000100  },
    /*  1 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000200  },
    /*  2 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000400  },
    /*  3 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000800  },
    /*  4 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00001000  },
    /*  5 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000100  },
    /*  6 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000200  },
    /*  7 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000400  },
    /*  8 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000800  },
    /*  9 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00001000  },
    /* 10 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000100  },
    /* 11 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000200  },
    /* 12 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000400  },
    /* 13 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000800  },
    /* 14 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00003000  },
    /* 15 */ { CS_STATE_ENABLED, 0x1234,  0x00034020,      0x00000800  },

};

/************************/
/*  End of File Comment */
/************************/
