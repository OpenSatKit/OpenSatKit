/************************************************************************
 ** File:
 **   $Id: cs_cmds.c 1.5.1.1 2015/03/03 11:58:03EST sstrege Exp  $
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
 **   The CFS Checksum (CS) Application's commands for OS code segement,
 **   the cFE core code segment, and for CS in general
 **
 **   $Log: cs_cmds.c  $
 **   Revision 1.5.1.1 2015/03/03 11:58:03EST sstrege 
 **   Added copyright information
 **   Revision 1.5 2011/09/06 14:48:02EDT jmdagost 
 **   Corrected OneShot event messages text.
 **   Revision 1.4 2009/06/18 10:11:52EDT rmcgraw 
 **   DCR8291:1 Changed #defines from OS_MEM_ to CFE_PSP_MEM_
 **   Revision 1.3 2009/06/11 11:20:14EDT rmcgraw 
 **   DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
 **   Revision 1.2 2008/07/23 10:34:40EDT njyanchik 
 **   Check in of CS Unit test
 **   Revision 1.1 2008/06/13 09:04:09EDT njyanchik 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/src/project.pj
 ** 
 *************************************************************************/

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_app.h"
#include "cs_events.h"
#include "cs_cmds.h"
#include "cs_utils.h"
#include "cs_compute.h"

/**************************************************************************
 **
 ** Function Prototypes
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable all background checksumming command                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableAllCSCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )  
    {
        CS_AppData.ChecksumState = CS_STATE_DISABLED;
        
        /* zero out the temp values in all the tables
         so that when the checksums are re-enabled,
         they don't start with a half-old value */
        CS_ZeroEepromTempValues();
        CS_ZeroMemoryTempValues();
        CS_ZeroTablesTempValues();
        CS_ZeroAppTempValues();
        CS_ZeroCfeCoreTempValues();
        CS_ZeroOSTempValues();
        
        CS_AppData.CmdCounter++;
        
        CFE_EVS_SendEvent (CS_DISABLE_ALL_INF_EID,
                           CFE_EVS_INFORMATION,
                           "Background Checksumming Disabled");
    }
    return;
} /* End of CS_DisableAllCSCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable all background checksumming command                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableAllCSCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )  
    {
        CS_AppData.ChecksumState = CS_STATE_ENABLED;
        
        CS_AppData.CmdCounter++;
        
        CFE_EVS_SendEvent (CS_ENABLE_ALL_INF_EID,
                           CFE_EVS_INFORMATION,
                           "Background Checksumming Enabled");
    }
    return;
} /* End of CS_EnableAllCSCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of the cFE core command          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableCfeCoreCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )  
    {
        CS_AppData.CfeCoreCSState = CS_STATE_DISABLED;
        CS_ZeroCfeCoreTempValues();
        
        CFE_EVS_SendEvent (CS_DISABLE_CFECORE_INF_EID,
                           CFE_EVS_INFORMATION, 
                           "Checksumming of cFE Core is Disabled");
        
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_DisableCfeCoreCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of the cFE core command           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableCfeCoreCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )  
    {
        CS_AppData.CfeCoreCSState = CS_STATE_ENABLED;
        
        CFE_EVS_SendEvent (CS_ENABLE_CFECORE_INF_EID,
                           CFE_EVS_INFORMATION, 
                           "Checksumming of cFE Core is Enabled");
        
        CS_AppData.CmdCounter++;
    }
    
    return;
    
} /* End of CS_EnableCfeCoreCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of the OS code segment command   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableOSCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )  
    {
        CS_AppData.OSCSState = CS_STATE_DISABLED;
        CS_ZeroOSTempValues();
        
        CFE_EVS_SendEvent (CS_DISABLE_OS_INF_EID,
                           CFE_EVS_INFORMATION, 
                           "Checksumming of OS code segment is Disabled");
        
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_DisableOSCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of the OS code segment command    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableOSCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )   
    {
        CS_AppData.OSCSState = CS_STATE_ENABLED;
        
        CFE_EVS_SendEvent (CS_ENABLE_OS_INF_EID,
                           CFE_EVS_INFORMATION, 
                           "Checksumming of OS code segment is Enabled");
        
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_OSEnableCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum for the cFE core command        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReportBaselineCfeCoreCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        if (CS_AppData.CfeCoreCodeSeg.ComputedYet == TRUE)
        {
            CFE_EVS_SendEvent (CS_BASELINE_CFECORE_INF_EID,
                               CFE_EVS_INFORMATION, 
                               "Baseline of cFE Core is 0x%08X", 
                               CS_AppData.CfeCoreCodeSeg.ComparisonValue);
        }
        else
        {
            CFE_EVS_SendEvent (CS_NO_BASELINE_CFECORE_INF_EID,
                               CFE_EVS_INFORMATION, 
                               "Baseline of cFE Core has not been computed yet");   
        }
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_ReportBaselineCfeCoreCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum for the OS code segment command */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReportBaselineOSCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        if (CS_AppData.OSCodeSeg.ComputedYet == TRUE)
        {
            CFE_EVS_SendEvent (CS_BASELINE_OS_INF_EID,
                               CFE_EVS_INFORMATION, 
                               "Baseline of OS code segment is 0x%08X", 
                               CS_AppData.OSCodeSeg.ComparisonValue);
        }
        else
        {
            CFE_EVS_SendEvent (CS_NO_BASELINE_OS_INF_EID, 
                               CFE_EVS_INFORMATION, 
                               "Baseline of OS code segment has not been computed yet");   
        }
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_ReportBaselineOSCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline checksum for the cFE core command     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeBaselineCfeCoreCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    uint32              ChildTaskID;
    int32               Status;
    
    /* Verify command packet length... */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        if (CS_AppData.ChildTaskInUse == FALSE)
        {
            /* There is no child task running right now, we can use it*/
            CS_AppData.ChildTaskInUse                = TRUE;
            CS_AppData.OneShotTaskInUse              = FALSE;
            
            /* fill in child task variables */
            CS_AppData.ChildTaskTable                = CS_CFECORE;
            CS_AppData.ChildTaskEntryID              = 0;
            CS_AppData.RecomputeEepromMemoryEntryPtr = &CS_AppData.CfeCoreCodeSeg;
            
            
            Status= CFE_ES_CreateChildTask(&ChildTaskID,
                                           CS_RECOMP_CFECORE_TASK_NAME,
                                           CS_RecomputeEepromMemoryChildTask,
                                           NULL,
                                           CFE_ES_DEFAULT_STACK_SIZE,
                                           CS_CHILD_TASK_PRIORITY,
                                           0);
            
            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEvent (CS_RECOMPUTE_CFECORE_STARTED_DBG_EID,
                                   CFE_EVS_DEBUG, 
                                   "Recompute of cFE core started");
                CS_AppData.CmdCounter++;
            }
            else/* child task creation failed */
            {
                CFE_EVS_SendEvent (CS_RECOMPUTE_CFECORE_CREATE_CHDTASK_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Recompute cFE core failed, CFE_ES_CreateChildTask returned: 0x%08X",
                                   Status);
                CS_AppData.CmdErrCounter++;
                CS_AppData.ChildTaskInUse = FALSE;
            }
        }
        else
        {
            /*send event that we can't start another task right now */
            CFE_EVS_SendEvent (CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID,
                               CFE_EVS_ERROR,
                               "Recompute cFE core failed: a child task is in use");
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
}/* end CS_RecomputeBaselineCfeCoreCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline checksum for the OS code seg command  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeBaselineOSCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    uint32              ChildTaskID;
    int32               Status;
    
    /* Verify command packet length... */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        if (CS_AppData.ChildTaskInUse == FALSE)
        {
            /* There is no child task running right now, we can use it*/
            CS_AppData.ChildTaskInUse                = TRUE;
            CS_AppData.OneShotTaskInUse              = FALSE;
            
            /* fill in child task variables */
            CS_AppData.ChildTaskTable                = CS_OSCORE;
            CS_AppData.ChildTaskEntryID              = 0;
            CS_AppData.RecomputeEepromMemoryEntryPtr = &CS_AppData.OSCodeSeg;
            
            
            Status= CFE_ES_CreateChildTask(&ChildTaskID,
                                           CS_RECOMP_OS_TASK_NAME,
                                           CS_RecomputeEepromMemoryChildTask,
                                           NULL,
                                           CFE_ES_DEFAULT_STACK_SIZE,
                                           CS_CHILD_TASK_PRIORITY,
                                           0);
            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEvent (CS_RECOMPUTE_OS_STARTED_DBG_EID, 
                                   CFE_EVS_DEBUG, 
                                   "Recompute of OS code segment started");
                CS_AppData.CmdCounter++;
            }
            else/* child task creation failed */
            {
                CFE_EVS_SendEvent (CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Recompute OS code segment failed, CFE_ES_CreateChildTask returned: 0x%08X",
                                   Status);
                CS_AppData.CmdErrCounter++;
                CS_AppData.ChildTaskInUse = FALSE;
            }
        }
        else
        {
            /*send event that we can't start another task right now */
            CFE_EVS_SendEvent (CS_RECOMPUTE_OS_CHDTASK_ERR_EID,
                               CFE_EVS_ERROR,
                               "Recompute OS code segment failed: a child task is in use");
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
}/* end CS_RecomputeBaselineOSCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Compute the OneShot checksum command                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_OneShotCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_OneShotCmd_t);
    uint32              ChildTaskID;
    int32               Status;
    CS_OneShotCmd_t   * CmdPtr;
    
    /* Verify command packet length... */    
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_OneShotCmd_t*) MessagePtr;
        
        /* validate size and address */
        Status = CFE_PSP_MemValidateRange(CmdPtr -> Address, CmdPtr -> Size, CFE_PSP_MEM_ANY);
        
        if (Status == OS_SUCCESS)
        {
            if (CS_AppData.ChildTaskInUse == FALSE)
            {
                /* There is no child task running right now, we can use it*/
                CS_AppData.ChildTaskInUse                   = TRUE;
                CS_AppData.OneShotTaskInUse                 = TRUE;
                
                CS_AppData.LastOneShotAddress   = CmdPtr -> Address;
                CS_AppData.LastOneShotSize      = CmdPtr -> Size;
                CS_AppData.LastOneShotChecksum  = 0;
                
                Status = CFE_ES_CreateChildTask(&ChildTaskID,
                                                CS_ONESHOT_TASK_NAME,
                                                CS_OneShotChildTask,
                                                NULL,
                                                CFE_ES_DEFAULT_STACK_SIZE,
                                                CS_CHILD_TASK_PRIORITY,
                                                0);
                if (Status == CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent (CS_ONESHOT_STARTED_DBG_EID,
                                       CFE_EVS_DEBUG,
                                       "OneShot checksum started on address: 0x%08X, size: %d",
                                       CmdPtr -> Address,
                                       CmdPtr -> Size);
                    
                    CS_AppData.ChildTaskID = ChildTaskID;
                    CS_AppData.CmdCounter++;
                }
                else/* child task creation failed */
                {
                    CFE_EVS_SendEvent (CS_ONESHOT_CREATE_CHDTASK_ERR_EID,
                                       CFE_EVS_ERROR,
                                       "OneShot checkum failed, CFE_ES_CreateChildTask returned: 0x%08X",
                                       Status);
                    
                    CS_AppData.CmdErrCounter++;
                    CS_AppData.ChildTaskInUse   = FALSE;
                    CS_AppData.OneShotTaskInUse = FALSE;
                }
            }
            else
            {
                /*send event that we can't start another task right now */
                CFE_EVS_SendEvent (CS_ONESHOT_CHDTASK_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "OneShot checksum failed: a child task is in use");
                
                CS_AppData.CmdErrCounter++;
            }
        }/* end if CFE_PSP_MemValidateRange */
        else
        {
            CFE_EVS_SendEvent (CS_ONESHOT_MEMVALIDATE_ERR_EID,
                               CFE_EVS_ERROR,
                               "OneShot checksum failed, CFE_PSP_MemValidateRange returned: 0x%08X",
                               Status);
            
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
}/* end CS_OneShotCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Cancel the OneShot checksum command                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_CancelOneShotCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    int32               Status;
    
    /* Verify command packet length... */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        /* Make sure there is a OneShot command in use */
        if (CS_AppData.ChildTaskInUse == TRUE  && CS_AppData.OneShotTaskInUse == TRUE)
        {
            Status= CFE_ES_DeleteChildTask(CS_AppData.ChildTaskID);
            
            if (Status == CFE_SUCCESS)
            {
                CS_AppData.ChildTaskID          = 0;
                CS_AppData.ChildTaskInUse       = FALSE;
                CS_AppData.OneShotTaskInUse     = FALSE;
                CS_AppData.CmdCounter++;
                CFE_EVS_SendEvent (CS_ONESHOT_CANCELLED_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   "OneShot checksum calculation has been cancelled");
            }
            else
            {
                CFE_EVS_SendEvent (CS_ONESHOT_CANCEL_DELETE_CHDTASK_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Cancel OneShot checksum failed, CFE_ES_DeleteChildTask returned:  0x%08X",
                                   Status);
                CS_AppData.CmdErrCounter++;
            }
        }
        else
        {
            CFE_EVS_SendEvent (CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID,
                               CFE_EVS_ERROR,
                               "Cancel OneShot checksum failed. No OneShot active");
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
}/* end CS_CancelOneShotCmd */


/************************/
/*  End of File Comment */
/************************/
