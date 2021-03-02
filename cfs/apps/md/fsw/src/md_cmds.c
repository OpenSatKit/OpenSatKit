/*************************************************************************
** File: md_cmds.c
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
*
** Purpose: 
**   Functions for processing individual CFS Memory Dwell commands
**
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "md_cmds.h"
#include "md_utils.h"
/* Need to include md_msg.h for command type definitions */
#include "md_msg.h"
#include "md_platform_cfg.h"
#include <string.h>
#include "md_app.h"
#include "md_events.h"
#include "cfs_utils.h"
#include "md_dwell_tbl.h"

/* Global Data */
extern MD_AppData_t MD_AppData;

/******************************************************************************/

void MD_ProcessStartCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    int32               ErrorCount = 0;
    int32               Status = CFE_SUCCESS;
    int32               NumTblInMask = 0; /* Purely as info for event message */
    uint16              TableId = 0;
    uint16              TableIndex = 0;
    MD_CmdStartStop_t  *Start = NULL;
    bool             AnyTablesInMask = false;

     
    /*
    **  Cast message to StartStop Command.
    */
    Start = (MD_CmdStartStop_t *  ) MessagePtr;

    /*  Preview tables specified by command:                   */
    /*  Check that there's at least one valid table specified. */
    for (TableId=1; TableId <= MD_NUM_DWELL_TABLES; TableId++)
    {
        if (MD_TableIsInMask(TableId, Start->TableMask))
        {
            /* At least one valid Table Id is in Mask */
            AnyTablesInMask=true;
        }
    }
    
    /* 
    ** Handle specified operation.
    ** If nominal, start each of the specified tables.
    ** If error case was encountered, issue error message. 
    */
    if (AnyTablesInMask)
    {  /* Handle Nominal Case */
            
        for (TableId=1; TableId <= MD_NUM_DWELL_TABLES; TableId++)
        {
           if (MD_TableIsInMask(TableId, Start->TableMask))
           {    
              NumTblInMask++;

              /* Setting Countdown to 1 causes a dwell packet to be issued */
              /* on first wakeup call received. */
              TableIndex = TableId-1;
              MD_AppData.MD_DwellTables[ TableIndex ].Enabled = MD_DWELL_STREAM_ENABLED;
              MD_AppData.MD_DwellTables[ TableIndex ].Countdown = 1;
              MD_AppData.MD_DwellTables[ TableIndex ].CurrentEntry = 0;
              MD_AppData.MD_DwellTables[ TableIndex ].PktOffset = 0;
                          
              /* Change value in Table Services managed buffer */
              Status = MD_UpdateTableEnabledField (TableIndex, MD_DWELL_STREAM_ENABLED);
              if(Status != CFE_SUCCESS)
              {
                  ErrorCount++;
              }

              /* If table contains a rate of zero, report that no processing will occur */
              if (MD_AppData.MD_DwellTables[ TableIndex ].Rate == 0)
              {
                  CFE_EVS_SendEvent(MD_ZERO_RATE_CMD_INF_EID, CFE_EVS_EventType_INFORMATION, 
                  "Dwell Table %d is enabled with a delay of zero so no processing will occur", TableId); 
              }       
           }
        }
        
        if(ErrorCount == 0)
        {
            MD_AppData.CmdCounter++;

            CFE_EVS_SendEvent(MD_START_DWELL_INF_EID,  CFE_EVS_EventType_INFORMATION,
                "Start Dwell Table command processed successfully for table mask 0x%04X",
                                                      Start->TableMask);

        }
        else 
        {    
            MD_AppData.ErrCounter++;

            CFE_EVS_SendEvent(MD_START_DWELL_ERR_EID, CFE_EVS_EventType_ERROR,
                "Start Dwell Table for mask 0x%04X failed for %d of %d tables",
                Start->TableMask, ErrorCount, NumTblInMask);
        }
    }
    else /* No valid table id's specified in mask */
    {
        MD_AppData.ErrCounter++;
        CFE_EVS_SendEvent(MD_EMPTY_TBLMASK_ERR_EID,  CFE_EVS_EventType_ERROR,
       "%s command rejected because no tables were specified in table mask (0x%04X)",
        "Start Dwell", Start->TableMask );
    }
    return;
    
}  /* End of MD_ProcessStartCmd */
 
/******************************************************************************/

void MD_ProcessStopCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    int32               ErrorCount = 0;
    int32               Status = CFE_SUCCESS;
    int32               NumTblInMask = 0; /* Purely as info for event message */
    MD_CmdStartStop_t  *Stop = NULL;
    uint16              TableId = 0;
    uint16              TableIndex = 0;
    bool             AnyTablesInMask = false;
             
    /*
    **  Cast message to StartStop Command.
    */
    Stop = (MD_CmdStartStop_t *  ) MessagePtr;

    for (TableId=1; TableId <= MD_NUM_DWELL_TABLES; TableId++)
    {
        if (MD_TableIsInMask(TableId, Stop->TableMask))
        {
            NumTblInMask++;
            TableIndex = TableId-1;
            MD_AppData.MD_DwellTables[ TableIndex ].Enabled = MD_DWELL_STREAM_DISABLED;
            MD_AppData.MD_DwellTables[ TableIndex ].Countdown = 0;
            MD_AppData.MD_DwellTables[ TableIndex ].CurrentEntry = 0;
            MD_AppData.MD_DwellTables[ TableIndex ].PktOffset = 0;
            
            AnyTablesInMask=true;
            
            /* Change value in Table Services managed buffer */
            Status = MD_UpdateTableEnabledField (TableIndex, MD_DWELL_STREAM_DISABLED);
            if(Status != CFE_SUCCESS)
            {
                ErrorCount++;
            }
        }
    }
    
    if (AnyTablesInMask)
    {
        if(ErrorCount == 0)
        {
            CFE_EVS_SendEvent(MD_STOP_DWELL_INF_EID,  CFE_EVS_EventType_INFORMATION,
                "Stop Dwell Table command processed successfully for table mask 0x%04X",
                Stop->TableMask );

            MD_AppData.CmdCounter++;
        }
        else 
        {    
            MD_AppData.ErrCounter++;

            CFE_EVS_SendEvent(MD_STOP_DWELL_ERR_EID, CFE_EVS_EventType_ERROR,
                "Stop Dwell Table for mask 0x%04X failed for %d of %d tables",
                Stop->TableMask, ErrorCount, NumTblInMask);
        }
    }
    else
    {
        CFE_EVS_SendEvent(MD_EMPTY_TBLMASK_ERR_EID,  CFE_EVS_EventType_ERROR,
       "%s command rejected because no tables were specified in table mask (0x%04X)",
        "Stop Dwell", Stop->TableMask );
        MD_AppData.ErrCounter++;
    }
    return;
}  /* End of MD_ProcessStopCmd */
 
/******************************************************************************/

void MD_ProcessJamCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* Local variables */
    int32                   Status = CFE_SUCCESS;
    MD_CmdJam_t            *Jam = 0;
    bool                    AllInputsValid = true;
    cpuaddr                 ResolvedAddr = 0;
    MD_DwellControlEntry_t *DwellEntryPtr = 0; /* points to local task data */
    uint16                  EntryIndex = 0;
    uint8                   TableIndex = 0;
    CFS_SymAddr_t           NewDwellAddress;
            
    /*
    **  Cast message to Jam Command.
    */
    Jam         = (MD_CmdJam_t * ) MessagePtr;
    
    /* In case Dwell Address sym name isn't null terminated, do it now. */
    Jam->DwellAddress.SymName[OS_MAX_SYM_LEN - 1] = '\0';
    
    /*
    **  Check that TableId and EntryId command arguments,
    **  which are used as array indexes, are valid.
    */
    if (  !MD_ValidTableId ( Jam->TableId) )
    {        
        CFE_EVS_SendEvent(MD_INVALID_JAM_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
         "Jam Cmd rejected due to invalid Tbl Id arg = %d (Expect 1.. %d)",
          Jam->TableId, MD_NUM_DWELL_TABLES);

        AllInputsValid = false;
    }
    
    else if (!MD_ValidEntryId ( Jam->EntryId))
    {
        CFE_EVS_SendEvent(MD_INVALID_ENTRY_ARG_ERR_EID, CFE_EVS_EventType_ERROR,
         "Jam Cmd rejected due to invalid Entry Id arg = %d (Expect 1.. %d)",
          Jam->EntryId, MD_DWELL_TABLE_SIZE);

        AllInputsValid = false;
    }
    else
    {
        AllInputsValid = true;
    }
    
    
    
    /*
    **  If all inputs checked so far are valid, continue.
    */    
    if (AllInputsValid == true)
    {
        TableIndex  = Jam->TableId-1;
        EntryIndex  = Jam->EntryId-1;
                
        DwellEntryPtr   = (MD_DwellControlEntry_t *) &MD_AppData.MD_DwellTables[TableIndex].Entry[EntryIndex];

        if (Jam->FieldLength == 0)
        /*
        **  Jam a null entry.  Set all entry fields to zero.
        */    
        {
            /* Assign local values */
            DwellEntryPtr->ResolvedAddress = 0;
            DwellEntryPtr->Length          = 0;
            DwellEntryPtr->Delay           = 0;
            
            /* Update Table Services buffer */
            NewDwellAddress.Offset = 0;
            NewDwellAddress.SymName[0] = '\0';
            Status = MD_UpdateTableDwellEntry (TableIndex, EntryIndex, 0, 0, NewDwellAddress);

            /* Issue event */
            if(Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(MD_JAM_NULL_DWELL_INF_EID, CFE_EVS_EventType_INFORMATION,
                "Successful Jam of a Null Dwell Entry to Dwell Tbl#%d Entry #%d", 
                               Jam->TableId, Jam->EntryId  );
            }
            else
            {
                CFE_EVS_SendEvent(MD_JAM_NULL_DWELL_ERR_EID, CFE_EVS_EventType_ERROR,
                "Failed Jam of a Null Dwell Entry to Dwell Tbl#%d Entry #%d", 
                               Jam->TableId, Jam->EntryId  );
                
                AllInputsValid = false;
            }
        } 
        else
        /*
        **  Process non-null entry.
        */   
        { 
            /*
            **  Check that address and field length arguments pass all validity checks.
            */    

            /* Resolve and Validate Dwell Address */
            if (CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == false)
            {
                /* If DwellAddress argument couldn't be resolved, issue error event */
                CFE_EVS_SendEvent(MD_CANT_RESOLVE_JAM_ADDR_ERR_EID, 
                                  CFE_EVS_EventType_ERROR,
                                 "Jam Cmd rejected because symbolic address '%s' couldn't be resolved",
                                  Jam->DwellAddress.SymName);
                AllInputsValid = false;
            }
            else if (!MD_ValidFieldLength(Jam->FieldLength))
            {        
                CFE_EVS_SendEvent(MD_INVALID_LEN_ARG_ERR_EID, CFE_EVS_EventType_ERROR,
                                 "Jam Cmd rejected due to invalid Field Length arg = %d (Expect 0,1,2,or 4)",
                                  Jam->FieldLength);
                AllInputsValid = false;
            }
            else if (!MD_ValidAddrRange(ResolvedAddr, Jam->FieldLength))
            {
                /* Issue event message that ResolvedAddr is invalid */
                CFE_EVS_SendEvent(MD_INVALID_JAM_ADDR_ERR_EID, CFE_EVS_EventType_ERROR,
                                 "Jam Cmd rejected because address 0x%08X is not in a valid range", 
                                  (unsigned int)ResolvedAddr);
                AllInputsValid = false;
            }
#if MD_ENFORCE_DWORD_ALIGN == 0
            else  if ((Jam->FieldLength == 4) && 
                      CFS_Verify16Aligned(ResolvedAddr, (uint32)Jam->FieldLength) != true)
            {
                CFE_EVS_SendEvent(MD_JAM_ADDR_NOT_16BIT_ERR_EID, CFE_EVS_EventType_ERROR,
                                 "Jam Cmd rejected because address 0x%08X is not 16-bit aligned", 
                                  ResolvedAddr);
                AllInputsValid = false;
            }
#else    
            else  if ((Jam->FieldLength == 4) && 
                      CFS_Verify32Aligned(ResolvedAddr, (uint32)Jam->FieldLength) != true)
            {
                CFE_EVS_SendEvent(MD_JAM_ADDR_NOT_32BIT_ERR_EID, CFE_EVS_EventType_ERROR,
                                 "Jam Cmd rejected because address 0x%08X is not 32-bit aligned", 
                                  (unsigned int)ResolvedAddr);
                AllInputsValid = false;
            }
#endif
            else  if ((Jam->FieldLength == 2) && CFS_Verify16Aligned(ResolvedAddr, (uint32)Jam->FieldLength) != true)
            {
                CFE_EVS_SendEvent(MD_JAM_ADDR_NOT_16BIT_ERR_EID, CFE_EVS_EventType_ERROR,
                                 "Jam Cmd rejected because address 0x%08X is not 16-bit aligned", 
                                  (unsigned int)ResolvedAddr);
                AllInputsValid = false;
            }
            else
            {
                /* All inputs are valid */
                AllInputsValid = true;
            }
            


            if (AllInputsValid == true)
            /* 
            ** Perform Jam Operation : Copy Resolved DwellAddress, Length, and Delay to
            ** local control structure.
            */
            {
                /* Jam the new values into Application control structure */
                DwellEntryPtr->ResolvedAddress = ResolvedAddr;
                DwellEntryPtr->Length          = Jam->FieldLength;
                DwellEntryPtr->Delay           = Jam->DwellDelay;
                
                /* Update values in Table Services buffer */
                NewDwellAddress.Offset = Jam->DwellAddress.Offset;
                
                strncpy (NewDwellAddress.SymName, Jam->DwellAddress.SymName, OS_MAX_SYM_LEN-1);
                NewDwellAddress.SymName[OS_MAX_SYM_LEN-1] = '\0';
                
                Status = MD_UpdateTableDwellEntry (TableIndex, 
                                                   EntryIndex, 
                                                   Jam->FieldLength, 
                                                   Jam->DwellDelay, 
                                                   NewDwellAddress);
                
                /* Issue event */
                if(Status == CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(MD_JAM_DWELL_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Successful Jam to Dwell Tbl#%d Entry #%d", 
                               Jam->TableId, Jam->EntryId  );
                }
                else
                {
                    CFE_EVS_SendEvent(MD_JAM_DWELL_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Failed Jam to Dwell Tbl#%d Entry #%d", 
                               Jam->TableId, Jam->EntryId  );
                    
                    AllInputsValid = false;
                }
            }
            
        } /* end else Process non-null entry */  
          
    } /* end if AllInputsValid */    
    
    /*
    **  Handle bookkeeping.
    */
    if (AllInputsValid == true)
    {
        MD_AppData.CmdCounter++;   
                    
        /* Update Dwell Table Control Info, including rate */
        MD_UpdateDwellControlInfo(TableIndex);

        /* If table contains a rate of zero, and it enabled report that no processing will occur */
        if ((MD_AppData.MD_DwellTables[ TableIndex ].Rate == 0) &&
            (MD_AppData.MD_DwellTables[ TableIndex ].Enabled == MD_DWELL_STREAM_ENABLED))
        {
            CFE_EVS_SendEvent(MD_ZERO_RATE_CMD_INF_EID, CFE_EVS_EventType_INFORMATION, 
            "Dwell Table %d is enabled with a delay of zero so no processing will occur", Jam->TableId); 
        }
    }
    else
    {
        MD_AppData.ErrCounter++;
    }
            
    return;
    
} /* End of MD_ProcessJamCmd */



/******************************************************************************/
#if MD_SIGNATURE_OPTION == 1   

void MD_ProcessSignatureCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    int32                  Status = CFE_SUCCESS;
    MD_CmdSetSignature_t  *SignatureCmd = NULL;
    uint16                 TblId = 0;
    uint16                 StringLength;
    
     
    /*
    **  Cast message to Signature Command.
    */
    SignatureCmd = (MD_CmdSetSignature_t * ) MessagePtr;

    TblId = SignatureCmd->TableId;

    /*
    **  Check for Null Termination of string
    */
    for(StringLength = 0; StringLength < MD_SIGNATURE_FIELD_LENGTH; StringLength++)
    {
       if(SignatureCmd->Signature[StringLength] == '\0')
          break;
    }

    if (StringLength >= MD_SIGNATURE_FIELD_LENGTH)
    {      
        CFE_EVS_SendEvent(MD_INVALID_SIGNATURE_LENGTH_ERR_EID, CFE_EVS_EventType_ERROR,
         "Set Signature cmd rejected due to invalid Signature length");
          
        MD_AppData.ErrCounter++;
    }

    /*
    ** Check for valid TableId argument
    */
    else if (  !MD_ValidTableId ( TblId) )
    {      
        CFE_EVS_SendEvent(MD_INVALID_SIGNATURE_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
         "Set Signature cmd rejected due to invalid Tbl Id arg = %d (Expect 1.. %d)",
          TblId, MD_NUM_DWELL_TABLES);
          
        MD_AppData.ErrCounter++;
    }

    else
    
    /*
    **  Handle nominal case.
    */
    {
       /* Copy signature field to local dwell control structure */
       strncpy(MD_AppData.MD_DwellTables[TblId-1].Signature, 
             SignatureCmd->Signature, MD_SIGNATURE_FIELD_LENGTH-1);
       MD_AppData.MD_DwellTables[TblId-1].Signature[MD_SIGNATURE_FIELD_LENGTH-1]='\0';

       /* Update signature in Table Services buffer */
       Status = MD_UpdateTableSignature(TblId-1,SignatureCmd->Signature);
       if(Status == CFE_SUCCESS)
       {
            CFE_EVS_SendEvent(MD_SET_SIGNATURE_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Successfully set signature for Dwell Tbl#%d to '%s'", 
                               TblId, SignatureCmd->Signature  );

            MD_AppData.CmdCounter++;
       }
       else
       {
            CFE_EVS_SendEvent(MD_SET_SIGNATURE_ERR_EID, CFE_EVS_EventType_ERROR,
                  "Failed to set signature for Dwell Tbl#%d. Update returned 0x%08X",
                  TblId, Status);

            MD_AppData.ErrCounter++;
       }
    }
    return;

}

#endif

/************************/
/*  End of File Comment */
/************************/
