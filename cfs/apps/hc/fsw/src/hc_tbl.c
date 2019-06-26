/*
** $Id: $
**
** Title: Heater Control (HC) Application Table Definitions
**
** Purpose: Provides functions for the initialization, validation, and
**          management of the HC Type Table and the HC Component Table.
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** $Log: $ Changed HC_COMValidateTable function to reflect new ComponentTable structure
**	   cgesterling (18.Nov.2016)
**
*/

#include "cfe.h"
#include "hc_app.h"
#include "hc_custom.h"
#include "hc_events.h"
#include "hc_msg.h"
#include "hc_platform_cfg.h"
#include "hc_tbl.h"



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC -- access to global data structure (defined in hc_app.c)     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern HC_GlobalData_t  HC_GlobalData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC table function -- startup initialization                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 HC_TableInit(void)
{
    int32 Status = CFE_SUCCESS;

    /* Initialize heater control type table pointer */
    HC_GlobalData.TypeTablePtr = (HC_TypeTable_t *) NULL;
    
    /* Register the heater control type table - this must succeed! */
    Status = CFE_TBL_Register(&HC_GlobalData.TypeTableHandle,
                               HC_TABLE_CFE_NAME, sizeof(HC_TypeTable_t),
                             (CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP),
                             (CFE_TBL_CallbackFuncPtr_t) HC_ValidateTable);
   
    if (Status == CFE_SUCCESS)
    { 
        /* Make an attempt to load the default heater controls type table data */
        CFE_TBL_Load(HC_GlobalData.TypeTableHandle,
                     CFE_TBL_SRC_FILE, HC_TABLE_DEF_NAME); 

        /* Initialize components table pointer */
        HC_GlobalData.ComponentTablePtr = (HC_ComponentTable_t *) NULL;

        /* Register the components table - this must succeed! */
        Status = CFE_TBL_Register(&HC_GlobalData.ComponentTableHandle,
                               HC_COM_TABLE_CFE_NAME, sizeof(HC_ComponentTable_t),
                             (CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP),
                             (CFE_TBL_CallbackFuncPtr_t) HC_COMValidateTable);

        if (Status == CFE_SUCCESS)
        { 
            /* Make an attempt to load the default components table data - OK if this fails */
            CFE_TBL_Load(HC_GlobalData.ComponentTableHandle,
                     CFE_TBL_SRC_FILE, HC_COM_TABLE_DEF_NAME);

            /* Allow cFE a chance to dump, update, etc. */
            HC_AcquireTablePointers(); 
        }
    }

    return(Status);

} /* End HC_TableInit */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC Type table function -- table data verification               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 HC_ValidateTable(void *TableData)
{
    HC_TypeTable_t *TablePtr = (HC_TypeTable_t *) TableData;
    int32  Result = CFE_SUCCESS;
    int32  i;
    int32  CountBad = 0;
    int32  CountGood = 0;

    /* Cycle through each entry in the table */
    for (i = 0; i < HC_TABLE_ENTRY_COUNT; i++)
    {
       /* The enable flag must be either enabled or disabled */
       if ((TablePtr->Types[i].Enabled == HC_TYPE_ENABLED) || 
           (TablePtr->Types[i].Enabled == HC_TYPE_DISABLED))
       { 

           /* Must be a valid heater control type */

           if ((TablePtr->Types[i].Type < HC_NUM_CTRL_TYPES) && 
               (TablePtr->Types[i].Type >= 0))
           {
               /* Valid heater control type */

               /* Check temperature set points */
               /* Temp high set point must be greater than temp low set point */
               if (TablePtr->Types[i].TempHigh > TablePtr->Types[i].TempLow)
               {
                   /* Temp high is greater than temp low */
                   CountGood++;
               }
               else 
               {
                   /* Temp high is less than temp low error */
                   CountBad++;

                   /* Send event describing first error only*/
                   if (CountBad == 1)
                   {
                       CFE_EVS_SendEvent(HC_TABLE_VERIFY_ERR_EID, CFE_EVS_ERROR,
                       "HC Type Table verify error: index = %d, temperature set point error", i);
                   }
               }
           }
           else
           {
               /* Invalid heater control type */
               CountBad++;

               /* Send event describing first error only*/
               if (CountBad == 1)
               {
                   CFE_EVS_SendEvent(HC_TABLE_VERIFY_ERR_EID, CFE_EVS_ERROR,
                       "HC Type Table verify error: index = %d, invalid HC type", i);
               }
           }
        }
        else 
        {
            /* Invalid enable flag */
            CountBad++;

            /* Send event describing first error only*/
            if (CountBad == 1)
            {
                CFE_EVS_SendEvent(HC_TABLE_VERIFY_ERR_EID, CFE_EVS_ERROR,
                       "HC Type Table verify error: index = %d, invalid enable flag", i);
            }
        }
    }

    /* Display verify results */
    CFE_EVS_SendEvent(HC_TABLE_VERIFY_EID, CFE_EVS_INFORMATION,
       "HC Type Table verify results: good entries = %d, bad = %d",
                      CountGood, CountBad);

    /* Set error if there are bad table entries */
    if (CountBad != 0)
    {
        Result = HC_TABLE_VALIDATION_ERR;
    } 

    return(Result);

} /* End HC_ValidateTable */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC Components table function -- table data verification         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 HC_COMValidateTable(void *TableData)
{
    HC_ComponentTable_t *TablePtr = (HC_ComponentTable_t *) TableData;
    int32  Result = CFE_SUCCESS;
    int32  i;
    int32  j;
    int32  CountGood = 0;
    int32  CountBad = 0;
   
    /* Cycle through each entry in the table */
    for (i = 0; i < HC_COM_TABLE_ENTRY_COUNT; i++)
    {
        /* Must be a valid heater control type */
        if ((TablePtr->Components[i].Type < HC_NUM_CTRL_TYPES) &&
             (TablePtr->Components[i].Type >= 0))
        {
            /* Has a valid heater control type */

            /* Validate number of heaters */
            if ((TablePtr->Components[i].NumHeaters <= HC_MAX_NUM_HEATERS) &&
                (TablePtr->Components[i].NumHeaters > 0))
            {
                /* Has a valid number of heaters */
		
		for (j = 0; j < TablePtr->Components[i].NumHeaters; j++)
		{

                    /* Validate number of thermistors */
                    if ((TablePtr->Components[i].Heaters[j] <= HC_MAX_NUM_THERM) &&
                        (TablePtr->Components[i].Heaters[j] > 0))
		    {
			/* Has a valid number of thermistors */
			CountGood++;
		    }
                    else
                    {
			/* Invalid number of thermistors */
			CountBad++;

			/* Send event describing first error only*/
			if (CountBad == 1)
			{
			    CFE_EVS_SendEvent(HC_COM_TABLE_VERIFY_ERR_EID, CFE_EVS_ERROR,
                             "HC Components Table verify error: index = %d, heater = %d, invalid number of thermistors", i, j);
			}
		    }
		}
            }
            else
            {
                /* Invalid number of heaters */
                CountBad++;

                /* Send event describing first error only*/
                if (CountBad == 1)
                {
                    CFE_EVS_SendEvent(HC_COM_TABLE_VERIFY_ERR_EID, CFE_EVS_ERROR,
                     "HC Components Table verify error: index = %d, invalid number of heaters", i);
                }
            }
        }
        else
        {
            /* Invalid heater control type */
            CountBad++;

            /* Send event describing first error only*/
            if (CountBad == 1)
            {
                CFE_EVS_SendEvent(HC_COM_TABLE_VERIFY_ERR_EID, CFE_EVS_ERROR,
                 "HC Components Table verify error: index = %d, invalid HC type", i);
            }
        }
    }

    /* Display verify results */
    CFE_EVS_SendEvent(HC_COM_TABLE_VERIFY_EID, CFE_EVS_INFORMATION,
       "Component Table verify results: good entries = %d, bad = %d",
                      CountGood, CountBad);

    /* Set error if there are bad table entries */
    if (CountBad != 0)
    {
        Result = HC_TABLE_VALIDATION_ERR;
    } 

    return(Result);

} /* End HC_COMValidateTable */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC table function -- acquire table data pointer                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void HC_AcquireTablePointers(void)
{
    int32  Status;

    /* Allow cFE an opportunity to make component table updates */
    CFE_TBL_Manage(HC_GlobalData.ComponentTableHandle);

    /* Acquire pointer to component table */
    Status = CFE_TBL_GetAddress((void *) &HC_GlobalData.ComponentTablePtr,
                                          HC_GlobalData.ComponentTableHandle);

    if (Status == CFE_TBL_ERR_NEVER_LOADED)
    {
        /* Make sure we don't try to use the empty table buffer */
        HC_GlobalData.ComponentTablePtr = (HC_ComponentTable_t *) NULL;
    }

    /* Allow cFE an opportunity to make heater control type table updates */
    CFE_TBL_Manage(HC_GlobalData.TypeTableHandle);

    /* Acquire pointer to heater control type table */
    Status = CFE_TBL_GetAddress((void *) &HC_GlobalData.TypeTablePtr,
                                          HC_GlobalData.TypeTableHandle);

    if (Status == CFE_TBL_ERR_NEVER_LOADED)
    {
        /* Make sure we don't try to use the empty table buffer */
        HC_GlobalData.TypeTablePtr = (HC_TypeTable_t *) NULL;
    }

    return;

} /* End HC_AcquireTablePointers */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC table function -- release table data pointer                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void HC_ReleaseTablePointers(void)
{

    /* Release pointer to component table */
    CFE_TBL_ReleaseAddress(HC_GlobalData.ComponentTableHandle);

    /* Prevent table pointer use while released */
    HC_GlobalData.ComponentTablePtr = (HC_ComponentTable_t *) NULL;

    /* Release pointer to heater control type table */
    CFE_TBL_ReleaseAddress(HC_GlobalData.TypeTableHandle);

    /* Prevent table pointer use while released */
    HC_GlobalData.TypeTablePtr = (HC_TypeTable_t *) NULL;

    return;

} /* End HC_ReleaseTablePointers */

/************************/
/*  End of File Comment */
/************************/

