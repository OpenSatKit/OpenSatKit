/************************************************************************
** File: to_sub_table.h
**
** Purpose: 
**  Define TO Lab CPU specific subscription table 
**
** Notes:
**
** $Log: to_lab_sub_table.h  $
** Revision 1.3 2014/07/16 14:44:45GMT-05:00 acudmore 
** Member renamed from to_sub_table.h to to_lab_sub_table.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/to_lab/fsw/platform_inc/project.pj.
** Revision 1.2 2014/07/16 14:44:45ACT acudmore 
** Updated TO_LAB subscription table header file ( comments, clean up )
**
*************************************************************************/

/* 
** Add the proper include file for the message IDs below
*/
#include "cfe_msgids.h"

/*
** Common CFS app includes below are commented out
*/
#include "ci_lab_msgids.h"

#if 0
#include "hs_msgids.h"
#include "fm_msgids.h"
#include "sc_msgids.h"
#include "ds_msgids.h"
#include "lc_msgids.h"
#endif

static TO_subsciption_t  TO_SubTable[] =
{
            /* CFS App Subscriptions */
            {0x0843,                {0,0},  4},
            {0x08FA,                {0,0},  4},
            /* telemetry test sub */
            {0x0888,                {0,0},  4},
            {TO_UNUSED,              {0,0},  0}
};

/************************
** End of File Comment ** 
************************/
