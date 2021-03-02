/************************************************************************
** File: md_dw01.c
**
** NASA Docket No. GSC-18,450-1, identified as “Core Flight Software System (CFS)
** Memory Dwell Application Version 2.3.3” 
**
** Copyright © 2019 United States Government as represented by the Administrator of
** the National Aeronautics and Space Administration. All Rights Reserved. 
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
** http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License. 
**
** Purpose:
**  The CFS Memory Dwell (MD) Example Dwell Table #1
**
** Notes:
**
**
*************************************************************************/


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "md_tbldefs.h"
#include "md_app.h"
#include "cfs_utils.h"
#include "cfe_tbl_filedef.h"
#include "md_platform_cfg.h"


MD_DwellTableLoad_t     MD_Default_Dwell1_Tbl =
{
/* Enabled State */ MD_DWELL_STREAM_DISABLED,
#if MD_SIGNATURE_OPTION == 1  
/* Signature     */ "Default Table 1",
#endif
/* Entry    Length    Delay    Offset           SymName     */
/*   1 */{{      0,       0,  {     0,               ""  }   },
/*   2 */ {      0,       0,  {     0,               ""  }   },
/*   3 */ {      0,       0,  {     0,               ""  }   },
/*   4 */ {      0,       0,  {     0,               ""  }   },
/*   5 */ {      0,       0,  {     0,               ""  }   },
/*   6 */ {      0,       0,  {     0,               ""  }   },
/*   7 */ {      0,       0,  {     0,               ""  }   },
/*   8 */ {      0,       0,  {     0,               ""  }   },
/*   9 */ {      0,       0,  {     0,               ""  }   },
/*  10 */ {      0,       0,  {     0,               ""  }   },
/*  11 */ {      0,       0,  {     0,               ""  }   },
/*  12 */ {      0,       0,  {     0,               ""  }   },
/*  13 */ {      0,       0,  {     0,               ""  }   },
/*  14 */ {      0,       0,  {     0,               ""  }   },
/*  15 */ {      0,       0,  {     0,               ""  }   },
/*  16 */ {      0,       0,  {     0,               ""  }   },
/*  17 */ {      0,       0,  {     0,               ""  }   },
/*  18 */ {      0,       0,  {     0,               ""  }   },
/*  19 */ {      0,       0,  {     0,               ""  }   },
/*  20 */ {      0,       0,  {     0,               ""  }   },
/*  21 */ {      0,       0,  {     0,               ""  }   },
/*  22 */ {      0,       0,  {     0,               ""  }   },
/*  23 */ {      0,       0,  {     0,               ""  }   },
/*  24 */ {      0,       0,  {     0,               ""  }   },
/*  25 */ {      0,       0,  {     0,               ""  }   },
}
};

CFE_TBL_FILEDEF(MD_Default_Dwell1_Tbl, MD.DWELL_TABLE1, MD Dwell Table 1,md_dw01.tbl)

/************************/
/*  End of File Comment */
/************************/

