/************************************************************************
** File:
**   $Id: cs_tablestbl.c 1.3.1.1 2015/03/03 11:58:43EST sstrege Exp  $
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
**  The CFS Checksum (CS) Application Default Tables Table Definition
**
** Notes:
**
** $Log: cs_tablestbl.c  $
** Revision 1.3.1.1 2015/03/03 11:58:43EST sstrege 
** Added copyright information
** Revision 1.3 2012/09/14 14:54:53EDT aschoeni 
** Now uses macro for header
** Revision 1.2 2010/07/16 13:22:34EDT jmdagost 
** Corrected app name from "CS_APP" to "CS".
** Revision 1.1 2010/03/09 15:30:33EST jmdagost 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/tables/project.pj
**
*************************************************************************/


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cs_platform_cfg.h"
#include "cs_msgdefs.h"
#include "cs_tbldefs.h"
#include "cfe_tbl_filedef.h"

CS_Def_Tables_Table_Entry_t      CS_TablesTable[CS_MAX_NUM_TABLES_TABLE_ENTRIES] =
{
    /*         State             Name   */

    /*   0 */ { CS_STATE_EMPTY,   ""    },
    /*   1 */ { CS_STATE_EMPTY,   ""    },
    /*   2 */ { CS_STATE_EMPTY,   ""    },
    /*   3 */ { CS_STATE_EMPTY,   ""    },
    /*   4 */ { CS_STATE_EMPTY,   ""    },
    /*   5 */ { CS_STATE_EMPTY,   ""    },
    /*   6 */ { CS_STATE_EMPTY,   ""    },
    /*   7 */ { CS_STATE_EMPTY,   ""    },
    /*   8 */ { CS_STATE_EMPTY,   ""    },
    /*   9 */ { CS_STATE_EMPTY,   ""    },
    /*  10 */ { CS_STATE_EMPTY,   ""    },
    /*  11 */ { CS_STATE_EMPTY,   ""    },
    /*  12 */ { CS_STATE_EMPTY,   ""    },
    /*  13 */ { CS_STATE_EMPTY,   ""    },
    /*  14 */ { CS_STATE_EMPTY,   ""    },
    /*  15 */ { CS_STATE_EMPTY,   ""    },
    /*  16 */ { CS_STATE_EMPTY,   ""    },
    /*  17 */ { CS_STATE_EMPTY,   ""    },
    /*  18 */ { CS_STATE_EMPTY,   ""    },
    /*  19 */ { CS_STATE_EMPTY,   ""    },
    /*  20 */ { CS_STATE_EMPTY,   ""    },
    /*  21 */ { CS_STATE_EMPTY,   ""    },
    /*  22 */ { CS_STATE_EMPTY,   ""    },
    /*  23 */ { CS_STATE_EMPTY,   ""    }

};

/*
** Table file header
*/
CFE_TBL_FILEDEF(CS_TablesTable, CS.DefTablesTbl, CS Tables Tbl, cs_tablestbl.tbl)

/************************/
/*  End of File Comment */
/************************/
