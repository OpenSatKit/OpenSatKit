/*
** Purpose: Implement the spacecraft simulator.
**
** Notes:
**   1. Information events are used in order to trace execution for
**      demonstrations.
**   2. For the initial versions of the subsystem simulations I broke from
**      convention of defining one object per file. The simulations are
**      trivial and tightly coupled for hard coded scenarios.
**   3. TODO - Add sanity and some error checks after initial version. 
**      There are lots of assumptions. This is not flight code and its 
**      purely for demonstration so it doesn't have to be bullet proof
**      but some checks will help especially in SCSIM_Execute() and
**      SIM_ExecuteEvtCmd().
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>

#include "cfe_msgids.h"
#include "cfe_time_msg.h"

#include "app_cfg.h"
#include "scsim.h"


/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience Macros */

#define  ADCS  (&(ScSim->Adcs))
#define  CDH   (&(ScSim->Cdh))
#define  COMM  (&(ScSim->Comm))
#define  FSW   (&(ScSim->Fsw))
#define  INSTR (&(ScSim->Instr))
#define  PWR   (&(ScSim->Pwr))
#define  THRM  (&(ScSim->Thrm))


/**********************/
/** Type Definitions **/
/**********************/


/**********************/
/** Global File Data **/
/**********************/

static SCSIM_Class*  ScSim = NULL;



static CFE_TIME_SetTime_t         CfeSetTimeCmd;
static CFE_TIME_NoArgsCmd_t       CfeClrEvtLogCmd;   /* EVS doesn't have a no args type & anyone will do */
static CFE_EVS_EnableAppEvents_t  CfeEnaAppEvtsCmd;
static CFE_EVS_DisableAppEvents_t CfeDisAppEvtsCmd;

/*
** Scanf used to load event command parameters
*/

static char* ScanfStr[SCSIM_SCANF_TYPE_CNT] = {

   "UNDEF",
   "%i",               /* SCANF_1_INT */   
   "%i %i",            /* SCANF_2_INT */   
   "%i %i %i",         /* SCANF_3_INT */   
   "%f",               /* SCANF_1_FLT */
   "%lf %lf %lf",      /* SCANF_3_FLT */
   "%lf %lf %lf %lf",  /* SCANF_4_FLT */
   "NONE"

};

static SCSIM_EvtCmd SimIdleCmd = { {0,0}, SCSIM_IDLE_TIME, SCSIM_SUBSYS_SIM,  SIM_EVT_IDLE,  SCSIM_SCANF_NONE,  NULL};

/* Links get resolved when sim started */
static SCSIM_EvtCmd SimScenario1[SCSIM_EVT_CMD_MAX] = {

  /* SCSIM_PHASE_INIT */
  
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_ADCS, ADCS_EVT_SET_MODE,        SCSIM_SCANF_1_INT, "3"},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_ADCS, ADCS_EVT_ENTER_ECLIPSE,   SCSIM_SCANF_NONE,  NULL},
  
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_FSW,  FSW_EVT_SET_REC_FILE_CNT, SCSIM_SCANF_1_INT, "10"},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_FSW,  FSW_EVT_SET_REC_PCT_USED, SCSIM_SCANF_1_FLT, "5"},

  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_COMM, COMM_EVT_SET_DATA_RATE,   SCSIM_SCANF_1_INT, "1024"},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_COMM, COMM_EVT_SET_TDRS_ID,     SCSIM_SCANF_1_FLT,  "1"},

  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_INSTR, INSTR_EVT_ENA_POWER,     SCSIM_SCANF_NONE,  NULL},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_INSTR, INSTR_EVT_ENA_SCIENCE,   SCSIM_SCANF_NONE,  NULL},

  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_PWR,  PWR_EVT_SET_BATT_SOC,     SCSIM_SCANF_1_FLT,  "50"},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_PWR,  PWR_EVT_SET_SA_CURRENT,   SCSIM_SCANF_1_FLT,  "0"},

  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_THRM, THRM_EVT_ENA_HEATER_1,    SCSIM_SCANF_1_INT,  "1"},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_THRM, THRM_EVT_ENA_HEATER_2,    SCSIM_SCANF_1_INT,  "1"},

  /* SCSIM_PHASE_TIME_LAPSE */

  { {0,0}, (SCSIM_REALTIME_EPOCH-3500), SCSIM_SUBSYS_FSW,  FSW_EVT_CLR_EVT_LOG,   SCSIM_SCANF_NONE,  NULL},
  { {0,0}, (SCSIM_REALTIME_EPOCH-2400), SCSIM_SUBSYS_ADCS, ADCS_EVT_EXIT_ECLIPSE, SCSIM_SCANF_NONE,  NULL},

  /* SCSIM_PHASE_REALTIME */
  
  { {0,0}, SCSIM_REALTIME_EPOCH,        SCSIM_SUBSYS_COMM, COMM_EVT_SCH_AOS,       SCSIM_SCANF_3_INT, "30 240 1"},
  { {0,0}, SCSIM_REALTIME_EPOCH,        SCSIM_SUBSYS_COMM, COMM_EVT_SET_TDRS_ID,   SCSIM_SCANF_1_INT, "1"},  
  { {0,0}, (SCSIM_REALTIME_EPOCH+120),  SCSIM_SUBSYS_ADCS, ADCS_EVT_ENTER_ECLIPSE, SCSIM_SCANF_NONE,  NULL},
  { {0,0}, (SCSIM_REALTIME_EPOCH+300),  SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,       SCSIM_SCANF_NONE,  NULL},

  /*  events */

  { {0,0}, SCSIM_REALTIME_END,    SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,  SCSIM_SCANF_NONE,  NULL},
  { {0,0}, SCSIM_REALTIME_END,    SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,  SCSIM_SCANF_NONE,  NULL},
  { {0,0}, SCSIM_REALTIME_END,    SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,  SCSIM_SCANF_NONE,  NULL},
  { {0,0}, SCSIM_REALTIME_END,    SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,  SCSIM_SCANF_NONE,  NULL},
  { {0,0}, SCSIM_REALTIME_END,    SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,  SCSIM_SCANF_NONE,  NULL},
  { {0,0}, SCSIM_REALTIME_END,    SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,  SCSIM_SCANF_NONE,  NULL},
  { {0,0}, SCSIM_REALTIME_END,    SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,  SCSIM_SCANF_NONE,  NULL}
  
};

static SCSIM_EvtCmd SimScenario2[SCSIM_EVT_CMD_MAX] = {

  /* SCSIM_PHASE_INIT */
  
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_ADCS, ADCS_EVT_SET_MODE,        SCSIM_SCANF_1_INT, "3"},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_ADCS, ADCS_EVT_ENTER_ECLIPSE,   SCSIM_SCANF_NONE,  NULL},
  
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_FSW,  FSW_EVT_SET_REC_FILE_CNT, SCSIM_SCANF_1_INT, "10"},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_FSW,  FSW_EVT_SET_REC_PCT_USED, SCSIM_SCANF_1_FLT, "5"},

  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_COMM, COMM_EVT_SET_DATA_RATE,   SCSIM_SCANF_1_INT, "1024"},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_COMM, COMM_EVT_SET_TDRS_ID,     SCSIM_SCANF_1_FLT,  "2"},

  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_INSTR, INSTR_EVT_ENA_POWER,     SCSIM_SCANF_NONE,  NULL},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_INSTR, INSTR_EVT_ENA_SCIENCE,   SCSIM_SCANF_NONE,  NULL},

  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_PWR,  PWR_EVT_SET_BATT_SOC,     SCSIM_SCANF_1_FLT,  "50"},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_PWR,  PWR_EVT_SET_SA_CURRENT,   SCSIM_SCANF_1_FLT,  "0"},

  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_THRM, THRM_EVT_ENA_HEATER_1,    SCSIM_SCANF_1_INT,  "1"},
  { {0,0}, SCSIM_INIT_TIME, SCSIM_SUBSYS_THRM, THRM_EVT_ENA_HEATER_2,    SCSIM_SCANF_1_INT,  "1"},

  /* SCSIM_PHASE_TIME_LAPSE */

  { {0,0}, (SCSIM_REALTIME_EPOCH-3500), SCSIM_SUBSYS_FSW,   FSW_EVT_CLR_EVT_LOG,   SCSIM_SCANF_NONE,  NULL},
  { {0,0}, (SCSIM_REALTIME_EPOCH-2400), SCSIM_SUBSYS_ADCS,  ADCS_EVT_EXIT_ECLIPSE, SCSIM_SCANF_NONE,  NULL},
  { {0,0}, (SCSIM_REALTIME_EPOCH-1400), SCSIM_SUBSYS_CDH,   CDH_EVT_WATCHDOG_RST,  SCSIM_SCANF_NONE,  NULL},
  { {0,0}, (SCSIM_REALTIME_EPOCH-1398), SCSIM_SUBSYS_ADCS,  ADCS_EVT_SET_MODE,     SCSIM_SCANF_1_INT, "1"},
  { {0,0}, (SCSIM_REALTIME_EPOCH-1396), SCSIM_SUBSYS_INSTR, INSTR_EVT_DIS_POWER,     SCSIM_SCANF_NONE,  NULL},
  { {0,0}, (SCSIM_REALTIME_EPOCH-1394), SCSIM_SUBSYS_INSTR, INSTR_EVT_DIS_SCIENCE,   SCSIM_SCANF_NONE,  NULL},

  /* SCSIM_PHASE_REALTIME */
  
  { {0,0}, SCSIM_REALTIME_EPOCH,        SCSIM_SUBSYS_COMM, COMM_EVT_SCH_AOS,       SCSIM_SCANF_3_INT, "30 240 1"},
  { {0,0}, SCSIM_REALTIME_EPOCH,        SCSIM_SUBSYS_COMM, COMM_EVT_SET_TDRS_ID,   SCSIM_SCANF_1_INT, "1"},  
  { {0,0}, (SCSIM_REALTIME_EPOCH+120),  SCSIM_SUBSYS_ADCS, ADCS_EVT_ENTER_ECLIPSE, SCSIM_SCANF_NONE,  NULL},
  { {0,0}, (SCSIM_REALTIME_EPOCH+300),  SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,       SCSIM_SCANF_NONE,  NULL},

  /*  events */

  { {0,0}, SCSIM_REALTIME_END,    SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,  SCSIM_SCANF_NONE,  NULL},
  { {0,0}, SCSIM_REALTIME_END,    SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,  SCSIM_SCANF_NONE,  NULL},
  { {0,0}, SCSIM_REALTIME_END,    SCSIM_SUBSYS_SIM,  SIM_EVT_STOP_SIM,  SCSIM_SCANF_NONE,  NULL}
  
};

/* 
** Subsystem strings
*/
static const char* SubSysStr[] = {
   
   "UNDEF",  /* SCSIM_SUBSYS_UNDEF  = 0 */   
   "SIM",    /* SCSIM_SUBSYS_SYS    = 1 */
   "ADCS",   /* SCSIM_SUBSYS_ADCS   = 2 */
   "CDH",    /* SCSIM_SUBSYS_CDH    = 3 */
   "COMM",   /* SCSIM_SUBSYS_COMM   = 4 */
   "FSW",    /* SCSIM_SUBSYS_FSW    = 5 */
   "INSTR",  /* SCSIM_SUBSYS_FSW    = 6 */
   "PWR",    /* SCSIM_SUBSYS_PWR    = 7 */
   "THRM"    /* SCSIM_SUBSYS_THRM   = 8 */

};


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static void SIM_AddEvtCmd(SCSIM_EvtCmd* NewRunTimeCmd);
static void SIM_DumpScenario(uint8 StartIdx, uint8 EndIdx);
static void SIM_ExecuteEvtCmd(void);
static void SIM_SetTime(uint32 NewSeconds);
static void SIM_StopSim(void);
static boolean SIM_ProcessEvtCmd(SCSIM_EvtCmd* EvtCmd);

static void ADCS_Init(ADCS_Model* Adcs);
static void ADCS_Execute(ADCS_Model* Adcs);
static boolean ADCS_ProcessEvtCmd(ADCS_Model* Adcs, SCSIM_EvtCmd* EvtCmd);

static void CDH_Init(CDH_Model* Cdh);
static void CDH_Execute(CDH_Model* Cdh);
static boolean CDH_ProcessEvtCmd(CDH_Model* Cdh, SCSIM_EvtCmd* EvtCmd);

static void COMM_Init(COMM_Model* Comm);
static void COMM_Execute(COMM_Model* Comm);
static boolean COMM_ProcessEvtCmd(COMM_Model* Comm, SCSIM_EvtCmd* EvtCmd);

static void FSW_Init(FSW_Model* Fsw);
static void FSW_Execute(FSW_Model* Fsw);
static boolean FSW_ProcessEvtCmd(FSW_Model* Fsw, SCSIM_EvtCmd* EvtCmd);

static void INSTR_Init(INSTR_Model* Instr);
static void INSTR_Execute(INSTR_Model* Instr);
static boolean INSTR_ProcessEvtCmd(INSTR_Model* Instr, SCSIM_EvtCmd* EvtCmd);

static void PWR_Init(PWR_Model* Pwr);
static void PWR_Execute(PWR_Model* Pwr);
static boolean PWR_ProcessEvtCmd(PWR_Model* Pwr, SCSIM_EvtCmd* EvtCmd);

static void THRM_Init(THRM_Model* Thrm);
static void THRM_Execute(THRM_Model* Thrm);
static boolean THRM_ProcessEvtCmd(THRM_Model* Thrm, SCSIM_EvtCmd* EvtCmd);

static void SCSIM_SendMgmtPkt(void);
static void SCSIM_SendModelPkt(void);


/******************************************************************************
** Function: SCSIM_Constructor
**
*/
void SCSIM_Constructor(SCSIM_Class *ScSimPtr, CFE_SB_MsgId_t MgmtPktMsgId,
                       CFE_SB_MsgId_t ModelPktMsgId)
{
 
   ScSim = ScSimPtr;

   CFE_PSP_MemSet((void*)ScSim, 0, sizeof(SCSIM_Class));

   ScSim->Time.Seconds = SCSIM_IDLE_TIME;
   ScSim->Phase = SCSIM_PHASE_IDLE;
   ScSim->LastEvtCmd = &SimIdleCmd;
   ScSim->NextEvtCmd = &SimIdleCmd;
   
   ADCS_Init(ADCS);
   CDH_Init(CDH);
   COMM_Init(COMM);
   FSW_Init(FSW);
   INSTR_Init(INSTR);
   PWR_Init(PWR);
   THRM_Init(THRM);

   CFE_SB_InitMsg(&ScSim->MgmtPkt,  MgmtPktMsgId,  SCSIM_MGMT_PKT_LEN,  TRUE);
   CFE_SB_InitMsg(&ScSim->ModelPkt, ModelPktMsgId, SCSIM_MODEL_PKT_LEN, TRUE);

   CFE_SB_InitMsg (&CfeSetTimeCmd, CFE_TIME_CMD_MID, sizeof(CFE_TIME_SetTime_t), TRUE);
   CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CfeSetTimeCmd, CFE_TIME_SET_TIME_CC);
    
   CFE_SB_InitMsg (&CfeClrEvtLogCmd, CFE_EVS_CMD_MID, sizeof(CFE_TIME_NoArgsCmd_t), TRUE);
   CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CfeClrEvtLogCmd, CFE_EVS_CLEAR_LOG_CC);
   CFE_SB_GenerateChecksum((CFE_SB_Msg_t *) &CfeClrEvtLogCmd);

   CFE_SB_InitMsg (&CfeEnaAppEvtsCmd, CFE_EVS_CMD_MID, sizeof(CFE_EVS_EnableAppEvents_t), TRUE);
   CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CfeEnaAppEvtsCmd, CFE_EVS_ENABLE_APP_EVENTS_CC);
   
   CFE_SB_InitMsg (&CfeDisAppEvtsCmd, CFE_EVS_CMD_MID, sizeof(CFE_EVS_DisableAppEvents_t), TRUE);
   CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CfeDisAppEvtsCmd, CFE_EVS_DISABLE_APP_EVENTS_CC);

} /* End SCSIM_Constructor() */


/******************************************************************************
** Function: SCSIM_Execute
**
** Execute spacecraft simulation cycle. When to execute the next command is
** in this function rather than ExecuteCmd() because time is managed 
** differently depending on the sim phase.
**
** ScSim time is course with subsecond second accuracy. Since the SCSIM
** app is driven by the scheduler, it's already synched to the cFS at the
** seconds level of resolution so when seconds are increment nothing is done
** WRT to cFE Time. When time jumps occur cFE Time is synchonrized.
**
*/
boolean SCSIM_Execute(void)
{

   boolean RetStatus = TRUE;
   uint16 TimeLapseExeCnt = 0;
   
   if (ScSim->Active) {

      switch (ScSim->Phase) {
         
      /*
      ** SimTime stays fixed during init. 
      ** All init commands executed in one execution cycle
      ** One telemetry packet sent that reflects final init state      
      */
      case SCSIM_PHASE_INIT:
      
         CFE_EVS_SendEvent(SCSIM_EXECUTE_EID, CFE_EVS_DEBUG, "SCSIM_PHASE_INIT: Enter");
         while (ScSim->NextEvtCmd->Time == SCSIM_INIT_TIME) {
        
            SIM_ExecuteEvtCmd();
            
         }
         
         if (ScSim->NextEvtCmd->Time < SCSIM_REALTIME_EPOCH) {
         
            /* Time lapse starts with the first time lapse command */
            ScSim->Phase = SCSIM_PHASE_TIME_LAPSE;
            SIM_SetTime(ScSim->NextEvtCmd->Time);
            
         }
         else {
            
            ScSim->Phase = SCSIM_PHASE_REALTIME;
            SIM_SetTime(SCSIM_REALTIME_EPOCH);
         
         }

         CFE_EVS_SendEvent(SCSIM_EXECUTE_EID, CFE_EVS_DEBUG, "SCSIM_PHASE_INIT: Exit with next phase %d at time %d", ScSim->Phase, ScSim->Time.Seconds);
         break;   
   
      case SCSIM_PHASE_TIME_LAPSE:

         CFE_EVS_SendEvent(SCSIM_EXECUTE_EID, CFE_EVS_DEBUG, "SCSIM_PHASE_TIME_LAPSE: Enter at SimTime %d, Next Cmd Time %d", ScSim->Time.Seconds, ScSim->NextEvtCmd->Time);
         while (ScSim->Time.Seconds < SCSIM_REALTIME_EPOCH && TimeLapseExeCnt < SCSIM_TIME_LAPSE_EXE_CNT) {
         
            if (ScSim->Time.Seconds >= (ScSim->NextEvtCmd->Time)) {
               SIM_ExecuteEvtCmd();
            }
            
            ADCS_Execute(ADCS);
            CDH_Execute(CDH);
            COMM_Execute(COMM);
            FSW_Execute(FSW);
            INSTR_Execute(INSTR);
            PWR_Execute(PWR);
            THRM_Execute(THRM);    
   
            ScSim->Time.Seconds++;
            TimeLapseExeCnt++;
         
         } /* End while loop */

         SIM_SetTime(ScSim->Time.Seconds);
         if (ScSim->Time.Seconds >= SCSIM_REALTIME_EPOCH) ScSim->Phase = SCSIM_PHASE_REALTIME;
         
         CFE_EVS_SendEvent(SCSIM_EXECUTE_EID, CFE_EVS_DEBUG, "SCSIM_PHASE_TIME_LAPSE: Exit with next phase %d at time %d", ScSim->Phase, ScSim->Time.Seconds);
         break;   
         
      case SCSIM_PHASE_REALTIME:

         if (ScSim->Time.Seconds >= (ScSim->NextEvtCmd->Time)) {
            SIM_ExecuteEvtCmd();
         }

         ADCS_Execute(ADCS);
         CDH_Execute(CDH);
         COMM_Execute(COMM);
         FSW_Execute(FSW);
         INSTR_Execute(INSTR);
         PWR_Execute(PWR);
         THRM_Execute(THRM);

         ScSim->Time.Seconds++;
         if (ScSim->Time.Seconds >= SCSIM_REALTIME_END) SIM_StopSim();
            
         break;   

      default:
         break;   

         
      } /* End phase switch */
   
   
   } /* End if Sim Active */
   
   
   /* 
   ** Manage Telemetry
   ** - Alway send sim management data
   ** - Only send model state data during a ground contact except
   **   during development need to be able to verify time-lapse
   **   model behavior 
   */
   
   SCSIM_SendMgmtPkt();
   
   if (ScSim->Comm.InContact || SCSIM_DEBUG) {      
      SCSIM_SendModelPkt();
   }

   return RetStatus;
      
} /* SCSIM_Execute() */


/******************************************************************************
** Function:  SCSIM_ResetStatus
**
*/
void SCSIM_ResetStatus(void)
{

   /* No counters to be reset that wouldn't effect the simulation state */
   
} /* End SCSIM_ResetStatus() */


/******************************************************************************
** Function: SCSIM_GetTblPtr
**
*/
const SCSIMTBL_Struct* SCSIM_GetTblPtr(void)
{

   return &(ScSim->Tbl);

} /* End SCSIM_GetTblPtr() */


/******************************************************************************
** Function: SCSIM_LoadTbl
**
*/
boolean SCSIM_LoadTbl(SCSIMTBL_Struct* NewTbl)
{

   boolean  RetStatus = TRUE;

   /*
   ** This is a simple table copy. More complex table loads may have pass/fail 
   ** criteria.
   */

   CFE_PSP_MemCpy(&(ScSim->Tbl), NewTbl, sizeof(SCSIMTBL_Struct));

   CFE_EVS_SendEvent(SCSIM_LOAD_TBL_EID, CFE_EVS_INFORMATION, "SCSIM table loaded.");

   return RetStatus;

} /* End SCSIM_LoadTbl() */


/******************************************************************************
** Function: SCSIM_LoadTblEntry
**
*/
boolean SCSIM_LoadTblEntry (uint16 ObjId, void* ObjData)
{

   boolean RetStatus = TRUE;
   
   switch (ObjId) {
      
	   case SCSIMTBL_OBJ_ACS:
         CFE_PSP_MemCpy(&(ScSim->Tbl.Acs), ObjData, sizeof(SCSIMTBL_Acs));
         break;
		 
	   case SCSIMTBL_OBJ_CDH:
         CFE_PSP_MemCpy(&(ScSim->Tbl.Cdh), ObjData, sizeof(SCSIMTBL_Cdh));
         break;
		 
	   case SCSIMTBL_OBJ_COMM:
         CFE_PSP_MemCpy(&(ScSim->Tbl.Comm), ObjData, sizeof(SCSIMTBL_Comm));
         break;
		 
	   case SCSIMTBL_OBJ_FSW:
         CFE_PSP_MemCpy(&(ScSim->Tbl.Fsw), ObjData, sizeof(SCSIMTBL_Fsw));
         break;
		 
	   case SCSIMTBL_OBJ_PWR:
         CFE_PSP_MemCpy(&(ScSim->Tbl.Pwr), ObjData, sizeof(SCSIMTBL_Pwr));
         break;
		 
	   case SCSIMTBL_OBJ_THRM:
         CFE_PSP_MemCpy(&(ScSim->Tbl.Thrm), ObjData, sizeof(SCSIMTBL_Thrm));
         break;
   
      default:
	      RetStatus = FALSE;
   
   } /* End ObjId switch */
   
   CFE_EVS_SendEvent(SCSIM_LOAD_TBL_OBJ_EID, CFE_EVS_INFORMATION, "SCSIM table JSON object %d loaded.", ObjId);
   
   return RetStatus;

} /* End SCSIM_LoadTblEntry() */


/******************************************************************************
** Functions: SCSIM_StartSimCmd
**
** Start a simulation
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean SCSIM_StartSimCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   int i;
   boolean SimEndCmdFound = FALSE;
   boolean RetStatus = TRUE;
   
   SCSIM_StartSimCmdMsg*  StartSimCmd = (SCSIM_StartSimCmdMsg*)MsgPtr;
   
   SIM_SetTime(SCSIM_INIT_TIME);
   ScSim->Active = TRUE;
   ScSim->Phase  = SCSIM_PHASE_INIT;
   
   /* 
   ** Only allow first sim set time to generated an event message and then disable time
   ** events. 
   */
   strcpy(CfeDisAppEvtsCmd.Payload.AppName,"CFE_SB");
   CFE_SB_GenerateChecksum((CFE_SB_Msg_t *) &CfeDisAppEvtsCmd);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &CfeDisAppEvtsCmd); 

   strcpy(CfeDisAppEvtsCmd.Payload.AppName,"CFE_TIME");
   CFE_SB_GenerateChecksum((CFE_SB_Msg_t *) &CfeDisAppEvtsCmd);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &CfeDisAppEvtsCmd); 
   
   strcpy(CfeDisAppEvtsCmd.Payload.AppName,"KIT_SCH");
   CFE_SB_GenerateChecksum((CFE_SB_Msg_t *) &CfeDisAppEvtsCmd);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &CfeDisAppEvtsCmd); 

   if (StartSimCmd->Id == 1) {
      ScSim->Scenario = SimScenario1;
   } 
   else if (StartSimCmd->Id == 2) {
      ScSim->Scenario = SimScenario2;
   }
   else {
      
      CFE_EVS_SendEvent(SCSIM_START_SIM_ERR_EID, CFE_EVS_ERROR, 
                        "Start Sim command rejected. Invalid scenario identifier %d. Must be between 1 and 2",
                        StartSimCmd->Id);
   
      RetStatus = FALSE;
   }

   
   if (RetStatus == TRUE) {
      
      ScSim->ScenarioId = StartSimCmd->Id;
      ScSim->LastEvtCmd = &SimIdleCmd;
      ScSim->NextEvtCmd = &ScSim->Scenario[0];
      
      /* 
      ** Configure time sorted linked list and setup so cmds can be 
      ** added at runtime. Assumes scenario is time sorted but no
      ** assumptions about the link indices. 
      ** Last entry in sim has next link set to SCSIM_EVT_CMD_NULL_IDX.
      */

      ScSim->RunTimeCmdIdx = SCSIM_EVT_CMD_NULL_IDX;

      ScSim->Scenario[0].Link.Prev = SCSIM_EVT_CMD_NULL_IDX;
      ScSim->Scenario[0].Link.Next = 1;
      
      for (i=1; i < SCSIM_EVT_CMD_MAX; i++) {
      
         ScSim->Scenario[i].Link.Prev = (i-1);
         ScSim->Scenario[i].Link.Next = (i+1);
         
         /* Ensure unused entries have time set to end time */
         if (SimEndCmdFound) {
         
            ScSim->Scenario[i].Time      = SCSIM_REALTIME_END;
            ScSim->Scenario[i].Link.Prev = SCSIM_EVT_CMD_NULL_IDX;
            ScSim->Scenario[i].Link.Next = SCSIM_EVT_CMD_NULL_IDX;

         } 
         else {
            
            /* Full table so set last index */
            if (i == (SCSIM_EVT_CMD_MAX-1)) {
            
               ScSim->Scenario[i].Link.Next = SCSIM_EVT_CMD_NULL_IDX;
            
            }
            else {
            
               if ( ScSim->Scenario[i].Time == SCSIM_REALTIME_END) {
                  ScSim->RunTimeCmdIdx = i+2;
                  SimEndCmdFound = TRUE;
               }
            
            }
            
         } /* End if !SimEndCmdFound */
            
      } /* End scenario loop */

      CFE_EVS_SendEvent(SCSIM_START_SIM_EID, CFE_EVS_INFORMATION, "Start Simulation using scenario %d with %d available runtime cmd entries starting at index %d",
                        StartSimCmd->Id, (SCSIM_EVT_CMD_MAX-ScSim->RunTimeCmdIdx), ScSim->RunTimeCmdIdx);

      #if (SCSIM_DEBUG == 1)
         SIM_DumpScenario(0,14);
      #endif

   } /* End if valid command parameters */
   
   return RetStatus;

} /* End SCSIM_StartSimCmd() */


/******************************************************************************
** Functions: SCSIM_StopSimCmd
**
** Stop a simulation
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean SCSIM_StopSimCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = TRUE;
  
   SIM_StopSim();
   
   return RetStatus;

} /* End SCSIM_StopSimCmd() */


/******************************************************************************
** Functions: SCSIM_StartPlbkCmd
**
** Stop a recorder playback.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**
*/
boolean SCSIM_StartPlbkCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = TRUE;
  
   ScSim->Fsw.Recorder.PlaybackEna = TRUE;
   CFE_EVS_SendEvent(SCSIM_START_REC_PLBK_EID, CFE_EVS_INFORMATION,"FSW recorder playback started"); 
   
   return RetStatus;

} /* End SCSIM_StartPlbkCmd() */


/******************************************************************************
** Functions: SCSIM_StopPlbkCmd
**
** Stop a recorder playback.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**
*/
boolean SCSIM_StopPlbkCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = TRUE;
  
   ScSim->Fsw.Recorder.PlaybackEna = FALSE;
   CFE_EVS_SendEvent(SCSIM_STOP_REC_PLBK_EID, CFE_EVS_INFORMATION,"FSW recorder playback stopped"); 
         
   return RetStatus;

} /* End SCSIM_StopPlbkCmd() */


/******************************************************************************
** Function: SCSIM_SendMgmtPkt
**
*/
static void SCSIM_SendMgmtPkt(void)
{


   ScSim->MgmtPkt.SimTime    = ScSim->Time.Seconds;
   ScSim->MgmtPkt.SimActive  = ScSim->Active;
   ScSim->MgmtPkt.SimPhase   = ScSim->Phase;
   
   ScSim->MgmtPkt.ContactTimePending   = ScSim->Comm.Contact.TimePending;
   ScSim->MgmtPkt.ContactLength        = ScSim->Comm.Contact.Length;   
   ScSim->MgmtPkt.ContactTimeConsumed  = ScSim->Comm.Contact.TimeConsumed;
   ScSim->MgmtPkt.ContactTimeRemaining = ScSim->Comm.Contact.TimeRemaining;
   
   if (ScSim->LastEvtCmd != NULL) {
      ScSim->MgmtPkt.LastEvtSubSysId  = ScSim->LastEvtCmd->SubSys;
      ScSim->MgmtPkt.LastEvtCmdId     = ScSim->LastEvtCmd->Id; 
   }
   else {
      ScSim->MgmtPkt.LastEvtSubSysId  = SCSIM_SUBSYS_SIM;
      ScSim->MgmtPkt.LastEvtCmdId     = SIM_EVT_UNDEF;

   }
   
   if (ScSim->NextEvtCmd != NULL) {
      ScSim->MgmtPkt.NextEvtSubSysId  = ScSim->NextEvtCmd->SubSys;
      ScSim->MgmtPkt.NextEvtCmdId     = ScSim->NextEvtCmd->Id; 
   }
   else {
      ScSim->MgmtPkt.NextEvtSubSysId  = SCSIM_SUBSYS_SIM;
      ScSim->MgmtPkt.NextEvtCmdId     = SIM_EVT_UNDEF; 
   }
      
   ScSim->MgmtPkt.AdcsLastEvtCmd.Time = ScSim->Adcs.LastEvtCmd.Time;
   ScSim->MgmtPkt.AdcsLastEvtCmd.Id   = ScSim->Adcs.LastEvtCmd.Id;

   ScSim->MgmtPkt.CdhLastEvtCmd.Time  = ScSim->Cdh.LastEvtCmd.Time;
   ScSim->MgmtPkt.CdhLastEvtCmd.Id    = ScSim->Cdh.LastEvtCmd.Id;

   ScSim->MgmtPkt.CommLastEvtCmd.Time = ScSim->Comm.LastEvtCmd.Time;
   ScSim->MgmtPkt.CommLastEvtCmd.Id   = ScSim->Comm.LastEvtCmd.Id;

   ScSim->MgmtPkt.PwrLastEvtCmd.Time  = ScSim->Pwr.LastEvtCmd.Time;
   ScSim->MgmtPkt.PwrLastEvtCmd.Id    = ScSim->Pwr.LastEvtCmd.Id;

   ScSim->MgmtPkt.ThrmLastEvtCmd.Time = ScSim->Thrm.LastEvtCmd.Time;
   ScSim->MgmtPkt.ThrmLastEvtCmd.Id   = ScSim->Thrm.LastEvtCmd.Id;

      
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &ScSim->MgmtPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &ScSim->MgmtPkt);

} /* End SCSIM_SendMgmtPkt() */


/******************************************************************************
** Function: SCSIM_SendModelPkt
**
*/
static void SCSIM_SendModelPkt(void)
{

    /*
   ** ADCS
   */

   ScSim->ModelPkt.Eclipse  = ScSim->Adcs.Eclipse;
   ScSim->ModelPkt.AdcsMode = ScSim->Adcs.Mode;

   /*
   ** C&DH
   */
   
   ScSim->ModelPkt.SbcRstCnt = ScSim->Cdh.SbcRstCnt;
   ScSim->ModelPkt.HwCmdCnt  = ScSim->Cdh.HwCmdCnt;
   ScSim->ModelPkt.LastHwCmd = ScSim->Cdh.LastHwCmd;

   /*
   ** Comm
   */
   
   ScSim->ModelPkt.InContact            = ScSim->Comm.InContact;
   ScSim->ModelPkt.ContactTimePending   = ScSim->Comm.Contact.TimePending;
   ScSim->ModelPkt.ContactTimeConsumed  = ScSim->Comm.Contact.TimeConsumed;
   ScSim->ModelPkt.ContactTimeRemaining = ScSim->Comm.Contact.TimeRemaining;
   ScSim->ModelPkt.ContactLink          = ScSim->Comm.Contact.Link;
   ScSim->ModelPkt.ContactTdrsId        = ScSim->Comm.Contact.TdrsId;
   ScSim->ModelPkt.ContactDataRate      = ScSim->Comm.Contact.DataRate;
   
   /*
   ** FSW
   */
   
   ScSim->ModelPkt.RecPctUsed     = ScSim->Fsw.Recorder.PctUsed;
   ScSim->ModelPkt.RecFileCnt     = ScSim->Fsw.Recorder.FileCnt;
   ScSim->ModelPkt.RecPlaybackEna = ScSim->Fsw.Recorder.PlaybackEna;
   ScSim->ModelPkt.FswSpare       = 0;

   /*
   ** Instrument
   */

   ScSim->ModelPkt.InstrPwrEna = ScSim->Instr.PwrEna;
   ScSim->ModelPkt.InstrSciEna = ScSim->Instr.SciEna;

   ScSim->ModelPkt.InstrFileCnt    = ScSim->Instr.FileCnt;
   ScSim->ModelPkt.InstrFileCycCnt = ScSim->Instr.FileCycCnt;

   /*
   ** Power
   */
   
   ScSim->ModelPkt.BattSoc   = ScSim->Pwr.BattSoc;
   ScSim->ModelPkt.SaCurrent = ScSim->Pwr.SaCurrent;

   
   /*
   ** Thermal
   */
   
   ScSim->ModelPkt.Heater1Ena = ScSim->Thrm.Heater1Ena;
   ScSim->ModelPkt.Heater2Ena = ScSim->Thrm.Heater2Ena;
   
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &ScSim->ModelPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &ScSim->ModelPkt);

} /* End SCSIM_SendModelPkt() */


/**************************/
/**************************/
/****                  ****/
/****    SIM OBJECT    ****/
/****                  ****/
/**************************/
/**************************/

/*
** The simulation object is a different from the model pbjects because it is
** not modelling any behavior. Instead it is being used to help control the
**
*/


/******************************************************************************
** Function: SIM_AddEvtCmd
**
** TODO - Add array bound check
*/
static void SIM_AddEvtCmd(SCSIM_EvtCmd* NewRunTimeCmd)
{
   
   boolean NewCmdInserted = FALSE;
   boolean EndOfBuffer    = FALSE;
   SCSIM_EvtCmd* EvtCmd   = ScSim->NextEvtCmd;
   SCSIM_EvtCmd* NewEvtCmd;
   
   CFE_EVS_SendEvent(SCSIM_ADD_EVENT_EID, CFE_EVS_DEBUG, 
                           "Enter SIM_AddEvtCmd() for subsystem %d cmd %d added at scenario index %d",
                           NewRunTimeCmd->SubSys, NewRunTimeCmd->Id, ScSim->RunTimeCmdIdx);
   CFE_EVS_SendEvent(SCSIM_ADD_EVENT_EID, CFE_EVS_DEBUG, 
                           "NextCmd: (%d,%d) %d, %d, %d",
                           EvtCmd->Link.Prev, EvtCmd->Link.Next, EvtCmd->Time, EvtCmd->SubSys, EvtCmd->Id);
                           
   if (ScSim->RunTimeCmdIdx == SCSIM_EVT_CMD_NULL_IDX) {
      
      CFE_EVS_SendEvent(SCSIM_EVENT_ERR_EID, CFE_EVS_ERROR, 
                        "Aborting sim due to event cmd buffer overflow while loading new subsystem %d cmd %d",
                        NewRunTimeCmd->SubSys, NewRunTimeCmd->Id);
   
      SIM_StopSim();
      
   }
   else {
   
      ScSim->Scenario[ScSim->RunTimeCmdIdx] = *NewRunTimeCmd;

      NewEvtCmd = &(ScSim->Scenario[ScSim->RunTimeCmdIdx]);

      while (!NewCmdInserted && !EndOfBuffer) {
   
         if (NewEvtCmd->Time <= EvtCmd->Time) {
   
            CFE_EVS_SendEvent(SCSIM_ADD_EVENT_EID, CFE_EVS_DEBUG, 
                              "Adding command before: (%d,%d) %d, %d, %d",
                              EvtCmd->Link.Prev, EvtCmd->Link.Next, EvtCmd->Time, EvtCmd->SubSys, EvtCmd->Id);
  
            NewEvtCmd->Link.Prev = EvtCmd->Link.Prev;
            NewEvtCmd->Link.Next = ScSim->Scenario[EvtCmd->Link.Prev].Link.Next;

            ScSim->Scenario[EvtCmd->Link.Prev].Link.Next = ScSim->RunTimeCmdIdx;  
            EvtCmd->Link.Prev = ScSim->RunTimeCmdIdx;

            NewCmdInserted = TRUE;
         
         }
         
         if (EvtCmd->Link.Next == SCSIM_EVT_CMD_NULL_IDX || EvtCmd->Time == SCSIM_REALTIME_END) {
         
            EndOfBuffer = TRUE;
            
         }
         else {
            
            EvtCmd = &(ScSim->Scenario[EvtCmd->Link.Next]);
         }
         
      } /* End while loop */
   
      if (NewCmdInserted) {
         
         CFE_EVS_SendEvent(SCSIM_ADD_EVENT_EID, CFE_EVS_DEBUG, 
                           "New subsystem %d cmd %d added at scenario index %d",
                           NewEvtCmd->SubSys, NewEvtCmd->Id, ScSim->RunTimeCmdIdx);

         ScSim->RunTimeCmdIdx++;
         if (ScSim->RunTimeCmdIdx == SCSIM_EVT_CMD_MAX) ScSim->RunTimeCmdIdx = SCSIM_EVT_CMD_NULL_IDX;
      
      }
      else {

         CFE_EVS_SendEvent(SCSIM_EVENT_ERR_EID, CFE_EVS_ERROR, 
                           "Aborting sim due to failure to insert new runtime cmd for subsystem %d cmd %d at index %d",
                           NewRunTimeCmd->SubSys, NewRunTimeCmd->Id, ScSim->RunTimeCmdIdx);
         SIM_StopSim();        
      
      }
   
   } /* End if room for new cmd */

   #if (SCSIM_DEBUG == 1)
      SIM_DumpScenario(8,14);
   #endif

} /* End SIM_AddEvtCmd() */


/******************************************************************************
** Function:  SIM_DumpScenario
**
*/
static void SIM_DumpScenario(uint8 StartIdx, uint8 EndIdx)
{
   int i;

   for (i=StartIdx; i <= EndIdx; i++) {
      OS_printf ("SimScenario[%d]: (%d,%d) %d: %d, %d, %d\n",
                 i, ScSim->Scenario[i].Link.Prev, ScSim->Scenario[i].Link.Next, ScSim->Scenario[i].Time,  
                 ScSim->Scenario[i].Time, ScSim->Scenario[i].SubSys, ScSim->Scenario[i].Id);
   }
   
} /* End SIM_DumpScenario() */


/******************************************************************************
** Function:  SIM_ExecuteEvtCmd
**
*/
static void SIM_ExecuteEvtCmd(void)
{
     
   CFE_EVS_SendEvent(SCSIM_EXECUTE_EVENT_EID, CFE_EVS_DEBUG, "Executing %s cmd %d at time %d",
                     SubSysStr[ScSim->NextEvtCmd->SubSys],ScSim->NextEvtCmd->Id,ScSim->NextEvtCmd->Time);
   
   switch (ScSim->NextEvtCmd->ScanfType) {
   case SCSIM_SCANF_1_INT:
      sscanf(ScSim->NextEvtCmd->Param, ScanfStr[SCSIM_SCANF_1_INT], &(ScSim->EvtCmdParam.OneInt));
      break;

   case SCSIM_SCANF_2_INT:
      sscanf(ScSim->NextEvtCmd->Param, ScanfStr[SCSIM_SCANF_2_INT], &(ScSim->EvtCmdParam.TwoInt[0]), &(ScSim->EvtCmdParam.TwoInt[1]));
      break;
      
   case SCSIM_SCANF_3_INT:
      sscanf(ScSim->NextEvtCmd->Param, ScanfStr[SCSIM_SCANF_3_INT], &(ScSim->EvtCmdParam.ThreeInt[0]), &(ScSim->EvtCmdParam.ThreeInt[1]), &(ScSim->EvtCmdParam.ThreeInt[2]));
      break;
      
   case SCSIM_SCANF_1_FLT:
      sscanf(ScSim->NextEvtCmd->Param, ScanfStr[SCSIM_SCANF_1_FLT], &(ScSim->EvtCmdParam.OneFlt));
      break;

   case SCSIM_SCANF_3_FLT:
      sscanf(ScSim->NextEvtCmd->Param, ScanfStr[SCSIM_SCANF_3_FLT], &(ScSim->EvtCmdParam.ThreeFlt[0]), &(ScSim->EvtCmdParam.ThreeFlt[1]), &(ScSim->EvtCmdParam.ThreeFlt[2]));
      break;
      
   case SCSIM_SCANF_4_FLT:
      sscanf(ScSim->NextEvtCmd->Param, ScanfStr[SCSIM_SCANF_3_FLT], &(ScSim->EvtCmdParam.FourFlt[0]), &(ScSim->EvtCmdParam.FourFlt[1]), &(ScSim->EvtCmdParam.FourFlt[2]), &(ScSim->EvtCmdParam.FourFlt[3]));
      break;
      
   case SCSIM_SCANF_NONE:
      break;

   default:
      break;

   } /* End scanf switch */

   switch (ScSim->NextEvtCmd->SubSys) {

      case SCSIM_SUBSYS_SIM:
         SIM_ProcessEvtCmd(ScSim->NextEvtCmd);
         break;

      case SCSIM_SUBSYS_ADCS:
         ADCS_ProcessEvtCmd(ADCS, ScSim->NextEvtCmd);
         break;
      
      case SCSIM_SUBSYS_CDH:
         CDH_ProcessEvtCmd(CDH, ScSim->NextEvtCmd);
         break;
      
      case SCSIM_SUBSYS_COMM:
         COMM_ProcessEvtCmd(COMM, ScSim->NextEvtCmd);
         break;
      
      case SCSIM_SUBSYS_FSW:
         FSW_ProcessEvtCmd(FSW, ScSim->NextEvtCmd);
         break;
      
      case SCSIM_SUBSYS_INSTR:
         INSTR_ProcessEvtCmd(INSTR, ScSim->NextEvtCmd);
         break;

      case SCSIM_SUBSYS_PWR:
         PWR_ProcessEvtCmd(PWR, ScSim->NextEvtCmd);
         break;
      
      case SCSIM_SUBSYS_THRM:
         THRM_ProcessEvtCmd(THRM, ScSim->NextEvtCmd);
         break;

      default:
         break;
   
   } /* End subsystem switch */
 
   
   ScSim->LastEvtCmd = ScSim->NextEvtCmd;
   
   ScSim->NextEvtCmd = &(ScSim->Scenario[ScSim->NextEvtCmd->Link.Next]);
       
   CFE_EVS_SendEvent(SCSIM_EXECUTE_EVENT_EID, CFE_EVS_DEBUG, 
                     "Exit SIM_ExecuteEvtCmd(): Next Cmd link (%d,%d), time %d, susbsy %d, cmd %d",
                     ScSim->NextEvtCmd->Link.Prev, ScSim->NextEvtCmd->Link.Next, ScSim->NextEvtCmd->Time, ScSim->NextEvtCmd->SubSys, ScSim->NextEvtCmd->Id);
   
} /* End SIM_ExecuteEvtCmd() */


/******************************************************************************
** Function:  SIM_SetTime
**
** Use CFE_TIME command messsage interface rather than CFE_TIME_ExternalTime() 
** because CFE_TIME_ExternalTime() requires compiling CFE_TIME in a configuration
** I didn't want to use.  
*/
static void SIM_SetTime(uint32 NewSeconds)
{
   

   ScSim->Time.Seconds = NewSeconds;
   
   CfeSetTimeCmd.Payload.Seconds = NewSeconds;
   
   CFE_SB_GenerateChecksum((CFE_SB_Msg_t *) &CfeSetTimeCmd);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &CfeSetTimeCmd);

} /* SIM_SetTime() */

/******************************************************************************
** Function: SIM_StopSim
**
*/
static void SIM_StopSim(void)
{
   
   CFE_EVS_SendEvent(SCSIM_STOP_SIM_EID, CFE_EVS_INFORMATION, "SCSIM stopped at %d seconds", ScSim->Time.Seconds);

   ScSim->Time.Seconds = 0;
   ScSim->Active       = FALSE;
   ScSim->Phase        = SCSIM_PHASE_IDLE;
   
   ScSim->LastEvtCmd   = &SimIdleCmd;
   ScSim->NextEvtCmd   = &SimIdleCmd;

   strcpy(CfeEnaAppEvtsCmd.Payload.AppName,"CFE_SB");
   CFE_SB_GenerateChecksum((CFE_SB_Msg_t *) &CfeEnaAppEvtsCmd);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &CfeEnaAppEvtsCmd); 

   strcpy(CfeEnaAppEvtsCmd.Payload.AppName,"CFE_TIME");
   CFE_SB_GenerateChecksum((CFE_SB_Msg_t *) &CfeEnaAppEvtsCmd);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &CfeEnaAppEvtsCmd); 

   strcpy(CfeEnaAppEvtsCmd.Payload.AppName,"KIT_SCH");
   CFE_SB_GenerateChecksum((CFE_SB_Msg_t *) &CfeEnaAppEvtsCmd);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &CfeEnaAppEvtsCmd); 

} /* End SIM_StopSim() */

/******************************************************************************
** Functions: SIM_ProcessEvtCmd
**
** Notes:
**   None
*/
static boolean SIM_ProcessEvtCmd(SCSIM_EvtCmd* EvtCmd)
{
   
   boolean RetStatus = TRUE;
   
   switch ((SIM_EvtCmd)EvtCmd->Id) {
      
   case SIM_EVT_STOP_SIM:
      SIM_StopSim();
      break;

   default:
	   RetStatus = FALSE;
      break;
      
   } /* End Cmd Id switch */
   
   return RetStatus;
   
} /* End SIM_ProcessEvtCmd() */


/********************************/
/********************************/
/****                        ****/
/****    ADCS MODEL OBJECT   ****/
/****                        ****/
/********************************/
/********************************/


static SCSIM_EvtCmd AdcsIdleCmd = { {0,0}, SCSIM_IDLE_TIME, SCSIM_SUBSYS_ADCS,  ADCS_EVT_UNDEF,  SCSIM_SCANF_NONE,  NULL};

static const char* AdcsModeStr[] = {
   
   "UNDEF",     /* ADCS_MODE_UNDEF      = 0 */
   "SAFEHOLD",  /* ADCS_MODE_SAFEHOLD   = 1 */
   "SUN_POINT", /* ADCS_MODE_SUN_POINT  = 2 */
   "INTERIAL",  /* ADCS_MODE_INERTIAL   = 3 */
   "SLEW"       /* ADCS_MODE_SLEW       = 4 */

};

/******************************************************************************
** Functions: ADCS_Init
**
** Initialize the ADCS model to a known state.
**
** Notes:
**   None
*/
static void ADCS_Init(ADCS_Model* Adcs)
{

   CFE_PSP_MemSet((void*)Adcs, 0, sizeof(ADCS_Model));
   
   Adcs->LastEvtCmd = AdcsIdleCmd;
 
   Adcs->Eclipse = TRUE;
   Adcs->Mode    = ADCS_MODE_UNDEF;

} /* ADCS_Init() */

/******************************************************************************
** Functions: ADCS_Execute
**
** Update ADCS model state.
**
** Notes:
**   None
*/
static void ADCS_Execute(ADCS_Model* Adcs)
{
   
   /* TODO - Implement model */

} /* ADCS_Execute() */


/******************************************************************************
** Functions: ADCS_ProcessEvtCmd
**
** Notes:
**   None
*/
static boolean ADCS_ProcessEvtCmd(ADCS_Model* Adcs, SCSIM_EvtCmd* EvtCmd)
{
   
   boolean RetStatus = TRUE;
   
   switch ((ADCS_EvtCmd)EvtCmd->Id) {
      
   case ADCS_EVT_SET_MODE:
      CFE_EVS_SendEvent(ADCS_CHANGE_MODE_EID, CFE_EVS_INFORMATION,"ADCS: Control mode changed from %s to %s",
      AdcsModeStr[Adcs->Mode], AdcsModeStr[ScSim->EvtCmdParam.OneInt]); 
      Adcs->Mode = ScSim->EvtCmdParam.OneInt;
      break;

   case ADCS_EVT_ENTER_ECLIPSE:
      Adcs->Eclipse = TRUE;
      CFE_EVS_SendEvent(ADCS_ENTER_ECLIPSE_EID, CFE_EVS_INFORMATION,"ADCS: Enter eclipse"); 
      break;

   case ADCS_EVT_EXIT_ECLIPSE:
      Adcs->Eclipse = FALSE;
      CFE_EVS_SendEvent(ADCS_EXIT_ECLIPSE_EID, CFE_EVS_INFORMATION,"ADCS: Exit eclipse"); 
      break;

   default:
	   RetStatus = FALSE;
      break;
      
   } /* End Cmd Id switch */
   
   Adcs->LastEvtCmd = *EvtCmd;
   
   return RetStatus;
   
} /* ADCS_ProcessEvtCmd() */


/**************************/
/**************************/
/****                  ****/
/****    CDH MODEL     ****/
/****                  ****/
/**************************/
/**************************/

static const char* HwCmdStr[] = {

   "UNDEF",       /* CDH_HW_CMD_UNDEF       = 0 */
   "RST_SBC",     /* CDH_HW_CMD_RST_SBC     = 1 */
   "PWR_CYC_SBC", /* CDH_HW_CMD_PWR_CYC_SBC = 2 */
   "SEL_BOOT_A",  /* CDH_HW_CMD_SEL_BOOT_A  = 3 */
   "SEL_BOOT_B"   /* CDH_HW_CMD_SEL_BOOT_B  = 4 */

};

/******************************************************************************
** Functions: CDH_Init
**
** Initialize the CDH model to a known state.
**
** Notes:
**   None
*/
static void CDH_Init(CDH_Model* Cdh)
{

   CFE_PSP_MemSet((void*)Cdh, 0, sizeof(CDH_Model));

} /* CDH_Init() */


/******************************************************************************
** Functions: CDH_Execute
**
** Update CDH model state.
**
** Notes:
**   None
*/
static void CDH_Execute(CDH_Model* Cdh)
{
   
   /* TODO - Implement model */

} /* CDH_Execute() */


/******************************************************************************
** Functions: CDH_ProcessEvtCmd
**
** Notes:
**   None
*/
static boolean CDH_ProcessEvtCmd(CDH_Model* Cdh, SCSIM_EvtCmd* EvtCmd)
{
   
   boolean RetStatus = TRUE;
   uint16 HwCmdStrIdx = 0;
   
   switch ((CDH_EvtCmd)EvtCmd->Id) {
   
   case CDH_EVT_WATCHDOG_RST:
      CFE_EVS_SendEvent(CDH_WATCHDOG_RESET_EID, CFE_EVS_ERROR, "C&DH: Watchdog Reset. Call Professor Wildermann for assistance!!!!");       
      Cdh->SbcRstCnt++;
      break;

   case CDH_EVT_SEND_HW_CMD:
      Cdh->HwCmdCnt++;
      Cdh->LastHwCmd = (CDH_HardwareCmd)ScSim->EvtCmdParam.OneInt;
      switch (Cdh->LastHwCmd) {
      case CDH_HW_CMD_RST_SBC:
         Cdh->SbcRstCnt++;
         break;
      case CDH_HW_CMD_PWR_CYC_SBC:
         break;
      case CDH_HW_CMD_SEL_BOOT_A:
         break;
      case CDH_HW_CMD_SEL_BOOT_B:
         break;
      default:
	      RetStatus = FALSE;
         break;
      }

      HwCmdStrIdx = (RetStatus == TRUE)? Cdh->LastHwCmd : 0;
      CFE_EVS_SendEvent(998, CFE_EVS_INFORMATION, "CDH: Received hardware command %s", HwCmdStr[HwCmdStrIdx]);
   
   default:
	   RetStatus = FALSE;
      break;
      
   } /* End Cmd Id switch */
      
   Cdh->LastEvtCmd = *EvtCmd;
      
   return RetStatus;
   
} /* CDH_ProcessEvtCmd() */



/*********************************/
/*********************************/
/****                         ****/
/****    COMM MODEL OBJECT    ****/
/****                         ****/
/*********************************/
/*********************************/

static SCSIM_EvtCmd CommIdleCmd = { {0,0}, SCSIM_IDLE_TIME, SCSIM_SUBSYS_COMM,  COMM_EVT_UNDEF,  SCSIM_SCANF_NONE,  NULL};

/******************************************************************************
** Functions: COMM_Init
**
** Initialize the Comm model to a known state.
**
** Notes:
**   None
*/
static void COMM_Init(COMM_Model* Comm)
{

   CFE_PSP_MemSet((void*)Comm, 0, sizeof(COMM_Model));
   
   Comm->LastEvtCmd = CommIdleCmd;
   
   Comm->InContact = FALSE;
   Comm->Contact.Link = COMM_LINK_UNDEF;
   Comm->Contact.TimePending = -1;

} /* COMM_Init() */

/******************************************************************************
** Functions: COMM_EndContact
**
*/
static void COMM_EndContact(COMM_Model* Comm)
{

   Comm->InContact = FALSE;
   Comm->Contact.Link          = COMM_LINK_UNDEF;
   Comm->Contact.Length        = 0;
   Comm->Contact.TimePending   = -1;
   Comm->Contact.TimeConsumed  = 0;
   Comm->Contact.TimeRemaining = 0;

} /* COMM_EndContact() */


/******************************************************************************
** Functions: COMM_Execute
**
** Update Comm model state.
**
** Notes:
**   None
*/
static void COMM_Execute(COMM_Model* Comm)
{
   if (Comm->InContact) {
   
      Comm->Contact.TimeConsumed++;
      Comm->Contact.TimeRemaining--;
      
      if (Comm->Contact.TimeRemaining <= 0) {
         
         COMM_EndContact(Comm);
         
      }
         
   } /* End if in contact */
   else {
      
      if (Comm->Contact.TimePending > 0) {
      
         Comm->Contact.TimePending--;
         if (Comm->Contact.TimePending == 0) {
            
            Comm->InContact = TRUE;
            Comm->Contact.TimeConsumed  = 0;
            Comm->Contact.TimeRemaining = Comm->Contact.Length;
            
            CFE_EVS_SendEvent(COMM_START_CONTACT_EID, CFE_EVS_INFORMATION, "Started contact with length of %d seconds", Comm->Contact.Length);
         }  
      
      } /* End if pending contact */
            
   } /* End if not in contact */
   
} /* COMM_Execute() */


/******************************************************************************
** Functions: COMM_ProcessEvtCmd
**
** Notes:
**   None
*/
static boolean COMM_ProcessEvtCmd(COMM_Model* Comm, SCSIM_EvtCmd* EvtCmd)
{
   
   boolean RetStatus = TRUE;
   SCSIM_EvtCmd LosEvtCmd;
         
   CFE_EVS_SendEvent(COMM_PROCESS_EVENT_EID, CFE_EVS_DEBUG, "Executing COMM cmd %d", EvtCmd->Id);
   
   switch ((COMM_EvtCmd)EvtCmd->Id) {
   
   case COMM_EVT_SCH_AOS:
 
      Comm->Contact.TimePending = ScSim->EvtCmdParam.ThreeInt[0];
      Comm->Contact.Length      = ScSim->EvtCmdParam.ThreeInt[1];
      Comm->Contact.Link        = ScSim->EvtCmdParam.ThreeInt[2];

      Comm->InContact = FALSE;
      Comm->Contact.TimeConsumed  = 0;
      Comm->Contact.TimeRemaining = 0;
      CFE_EVS_SendEvent(COMM_PROCESS_EVENT_EID, CFE_EVS_DEBUG, 
                        "Scheduled AOS in %ds, for %ds with link type %d", 
                        Comm->Contact.TimePending, Comm->Contact.Length, Comm->Contact.Link);
 
      LosEvtCmd.Link.Prev = SCSIM_EVT_CMD_NULL_IDX;
      LosEvtCmd.Link.Next = SCSIM_EVT_CMD_NULL_IDX;
      LosEvtCmd.Time      = EvtCmd->Time + Comm->Contact.Length;
      LosEvtCmd.SubSys    = SCSIM_SUBSYS_COMM;
      LosEvtCmd.Id        = COMM_EVT_LOS;
      LosEvtCmd.ScanfType = SCSIM_SCANF_NONE;
      LosEvtCmd.Param     = NULL;
      
      SIM_AddEvtCmd(&LosEvtCmd);
      
      break;
   
   case COMM_EVT_ABORT_CONTACT:
      COMM_EndContact(Comm);  
      break;

   case COMM_EVT_SET_DATA_RATE:
      Comm->Contact.DataRate = ScSim->EvtCmdParam.OneInt;
      break;

   case COMM_EVT_SET_TDRS_ID:
      Comm->Contact.TdrsId = ScSim->EvtCmdParam.OneInt;
      break;
   
   default:
	   RetStatus = FALSE;
      break;
      
   } /* End Cmd Id switch */

   Comm->LastEvtCmd = *EvtCmd;
   
   return RetStatus;
   
} /* COMM_ProcessEvtCmd() */


/********************************************/
/********************************************/
/****                                    ****/
/****    FLIGHT SOFTWARE MODEL OBJECT    ****/
/****                                    ****/
/********************************************/
/********************************************/

/******************************************************************************
** Functions: FSW_Init
**
** Initialize the Flight Software model to a known state.
**
** Notes:
**   None
*/
static void FSW_Init(FSW_Model* Fsw)
{

   CFE_PSP_MemSet((void*)Fsw, 0, sizeof(FSW_Model));
   
} /* FSW_Init() */


/******************************************************************************
** Functions: FSW_Execute
**
** Update Flight Software model state.
**
** Notes:
**   None
*/
static void FSW_Execute(FSW_Model* Fsw)
{
   
   /* TODO - Sim CDH & FSW files */
   //Fsw->Recorder.FileCnt = INSTR->FileCnt;

   if (Fsw->Recorder.PlaybackEna) {
   
      if (Fsw->Recorder.FileCnt == 0) Fsw->Recorder.PlaybackEna = FALSE;
      if (Fsw->Recorder.FileCnt > 0)  Fsw->Recorder.FileCnt--;

   }

} /* FSW_Execute() */


/******************************************************************************
** Functions: FSW_ProcessEvtCmd
**
** Notes:
**   None
*/
static boolean FSW_ProcessEvtCmd(FSW_Model* Fsw, SCSIM_EvtCmd* EvtCmd)
{
   
   boolean RetStatus = TRUE;
   
   switch ((FSW_EvtCmd)EvtCmd->Id) {

   case FSW_EVT_SET_REC_FILE_CNT:
      Fsw->Recorder.FileCnt = ScSim->EvtCmdParam.OneInt;
      break;

   case FSW_EVT_SET_REC_PCT_USED:
      Fsw->Recorder.PctUsed = ScSim->EvtCmdParam.OneFlt;
      break;
      
   case FSW_EVT_START_REC_PLBK:
      Fsw->Recorder.PlaybackEna = TRUE;
      break;

   case FSW_EVT_STOP_REC_PLBK:
      Fsw->Recorder.PlaybackEna = FALSE;
      break;

   case FSW_EVT_CLR_EVT_LOG:
      CFE_SB_SendMsg((CFE_SB_Msg_t *) &CfeClrEvtLogCmd);
      break;

   default:
	   RetStatus = FALSE;
      break;
      
   } /* End Cmd Id switch */
   
   
   Fsw->LastEvtCmd = *EvtCmd;
   
   return RetStatus;
   
} /* FSW_ProcessEvtCmd() */


/***************************************/
/***************************************/
/****                               ****/
/****    INSTRUMENT MODEL OBJECT    ****/
/****                               ****/
/***************************************/
/***************************************/

/******************************************************************************
** Functions: INSTR_Init
**
** Initialize the Instrument model to a known state.
**
** Notes:
**   None
*/
static void INSTR_Init(INSTR_Model* Instr)
{

   CFE_PSP_MemSet((void*)Instr, 0, sizeof(INSTR_Model));

} /* INSTR_Init() */


/******************************************************************************
** Functions: INSTR_Execute
**
** Update Instrument model state.
**
** Notes:
**   None
*/
static void INSTR_Execute(INSTR_Model* Instr)
{

  if (Instr->PwrEna && Instr->SciEna) {
  
     Instr->FileCycCnt++;
     if (Instr->FileCycCnt >= INSTR_CYCLES_PER_FILE) {
        
        //Instr->FileCnt++;
        FSW->Recorder.FileCnt++;
        Instr->FileCycCnt = 0;
     
     }
  }
  else {

     Instr->FileCycCnt = 0;

  }
   
} /* INSTR_Execute() */


/******************************************************************************
** Functions: INSTR_ProcessEvtCmd
**
** Notes:
**   None
*/
static boolean INSTR_ProcessEvtCmd(INSTR_Model* Instr, SCSIM_EvtCmd* EvtCmd)
{
   
   boolean RetStatus = TRUE;
   
   switch ((INSTR_EvtCmd)EvtCmd->Id) {
   case INSTR_EVT_ENA_POWER:
      Instr->PwrEna = TRUE;
      break;

   case INSTR_EVT_DIS_POWER:
      CFE_EVS_SendEvent(INSTR_DIS_POWER_EID, CFE_EVS_INFORMATION, "INSTR: Science intrument powered off");       
      Instr->PwrEna = FALSE;
      break;

   case INSTR_EVT_ENA_SCIENCE:
      Instr->SciEna = TRUE;
      break;

   case INSTR_EVT_DIS_SCIENCE:
      Instr->SciEna = FALSE;
      break;

   default:
	   RetStatus = FALSE;
      break;
      
   } /* End Cmd Id switch */
      
   Instr->LastEvtCmd = *EvtCmd;
      
   return RetStatus;
   
} /* INSTR_ProcessEvtCmd() */


/**********************************/
/**********************************/
/****                          ****/
/****    POWER MODEL OBJECT    ****/
/****                          ****/
/**********************************/
/**********************************/

/******************************************************************************
** Functions: PWR_Init
**
** Initialize the Power model to a known state.
**
** Notes:
**   None
*/
static void PWR_Init(PWR_Model* Pwr)
{

   CFE_PSP_MemSet((void*)Pwr, 0, sizeof(PWR_Model));

} /* PWR_Init() */


/******************************************************************************
** Functions: PWR_Execute
**
** Update Power model state.
**
** Notes:
**   None
*/
static void PWR_Execute(PWR_Model* Pwr)
{

   /*
   ** Simple linear model not based on any physics. 
   ** +/- 1% change for every minute charging/discharging   
   */
   
   if (ADCS->Eclipse == TRUE) {

     Pwr->SaCurrent = 0.0;

     Pwr->BattSoc -= 1.0/50.0;
     if (Pwr->BattSoc > 100.0) Pwr->BattSoc = 100.0;
     
   }
   else {

     Pwr->SaCurrent = 10.0;

     Pwr->BattSoc += 1.0/50.0;
     if (Pwr->BattSoc < 0.0) Pwr->BattSoc = 0.0;
   
   }
   
} /* PWR_Execute() */


/******************************************************************************
** Functions: PWR_ProcessEvtCmd
**
** Notes:
**   None
*/
static boolean PWR_ProcessEvtCmd(PWR_Model* Pwr, SCSIM_EvtCmd* EvtCmd)
{
   
   boolean RetStatus = TRUE;
   
   switch ((PWR_EvtCmd)EvtCmd->Id) {
      
   case PWR_EVT_SET_BATT_SOC:
      Pwr->BattSoc = ScSim->EvtCmdParam.OneFlt;
      break;

   case PWR_EVT_SET_SA_CURRENT:
      Pwr->SaCurrent = ScSim->EvtCmdParam.OneFlt;
      break;

   default:
	   RetStatus = FALSE;
      break;
      
   } /* End Cmd Id switch */
      
   Pwr->LastEvtCmd = *EvtCmd;
      
   return RetStatus;
   
} /* PWR_ProcessEvtCmd() */


/************************************/
/************************************/
/****                            ****/
/****    THERMAL MODEL OBJECT    ****/
/****                            ****/
/************************************/
/************************************/

/******************************************************************************
** Functions: THRM_Init
**
** Initialize the Thermal model to a known state.
**
** Notes:
**   None
*/
static void THRM_Init(THRM_Model* Thrm)
{

   CFE_PSP_MemSet((void*)Thrm, 0, sizeof(THRM_Model));
   
} /* THRM_Init() */


/******************************************************************************
** Functions: Thermal_Execute
**
** Update Thermal model state.
**
** Notes:
**   None
*/
static void THRM_Execute(THRM_Model* Thrm)
{
   
   if (ADCS->Eclipse == TRUE) {

      Thrm->Heater1Ena = TRUE;
      Thrm->Heater2Ena = TRUE;
      
   }
   else{

      Thrm->Heater1Ena = FALSE;
      Thrm->Heater2Ena = FALSE;

   }
   
} /* THRM_Execute() */


/******************************************************************************
** Functions: THRM_ProcessEvtCmd
**
** Notes:
**   None
*/
static boolean THRM_ProcessEvtCmd(THRM_Model* Thrm, SCSIM_EvtCmd* EvtCmd)
{
   
   boolean RetStatus = TRUE;
   
   switch ((THRM_EvtCmd)EvtCmd->Id) {

   case THRM_EVT_ENA_HEATER_1:
      Thrm->Heater1Ena = ScSim->EvtCmdParam.OneInt;
      break;

   case THRM_EVT_ENA_HEATER_2:
      Thrm->Heater2Ena = ScSim->EvtCmdParam.OneInt;
      break;

   default:
	   RetStatus = FALSE;
      break;
      
   } /* End Cmd Id switch */
   
   
   Thrm->LastEvtCmd = *EvtCmd;
   
   return RetStatus;
   
} /* THRM_ProcessEvtCmd() */
