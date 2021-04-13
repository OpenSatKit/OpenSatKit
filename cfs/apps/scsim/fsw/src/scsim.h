/*
** Purpose: Define Spacecraft Simulator Object
**
** Notes:
**   1. For the initial versions of the subsystem simulations I broke from
**      convention of defining one object per file. The simulations are
**      trivial and tightly coupled for hard coded scenarios.
**   2. The simulation models will evolve as will the combination of 
**      using models vs actual apps for function like C&DH recorder and also
**      the incorporation of 42. My goal is NOT to rewrite 42 but leverage
**      its capabilities.
**   3. Consider managing the sim with a state machine that has events
**      that define sim model behavior. Could be very generic with an
**      event-sim-model superclass. I would like the sim models to be driven
**      by 42 reality.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _scsim_
#define _scsim_

/*
** Includes
*/

#include "app_cfg.h"
#include "scsimtbl.h"

/***********************/
/** Macro Definitions **/
/***********************/


/*
** Event Message IDs
*/

#define SCSIM_START_SIM_EID      (SCSIM_BASE_EID +  0)
#define SCSIM_START_SIM_ERR_EID  (SCSIM_BASE_EID +  1)
#define SCSIM_STOP_SIM_EID       (SCSIM_BASE_EID +  2)
#define SCSIM_LOAD_TBL_EID       (SCSIM_BASE_EID +  3)
#define SCSIM_LOAD_TBL_OBJ_EID   (SCSIM_BASE_EID +  4)

/*
** Simulation management 
**
** Simulations have the following phases:
**   1. Initialization
**      - Each subsystem should have one or more commands to initialize the
**        subsystem to a known state
**   2. Time-lapse
**      - The simulation runs much faster than realtime to propagate each 
**        subsystem state to a point for the simulation to start
**      - Time-lapse starts with the first command scheduled prior to the
**        realtime epoch. 
**   3. Realtime
**      - The simulation progresses at realtime
**
** Sim event time tag conventions drive each simulation phase. 
** - 0: A single init time of zero is used to initialize models so all cmds
**   used to set initial model states should have the same time stamp.
** - 1..(Epoch-1) time is used for the time-lapse phase. This time is 
**   relative to the epoch.
** - Epoch defines when realtime begins. The epoch was defined so one
**   orbit can be performed during the time-lapsed phase.
*/

#define SCSIM_IDLE_TIME             (0)  /* No sim active */

#define SCSIM_INIT_TIME             (1)  /* Model initialization */  

#define SCSIM_TIME_LAPSE_EXE_CNT (1000)  /* Number of simulation seconds to perform in one SCSIM execution cycle */

#define SCSIM_REALTIME_EPOCH    (10000)  /* Time when realtime simulation starts */ 
#define SCSIM_REALTIME_END      (20000)  /* Sim doesn't execute until this time. Thsi time indicates sim is over */ 

#define SCSIM_EVT_CMD_MAX          (20)  /* Maximum number of event commands */
#define SCSIM_EVT_CMD_NULL_IDX     (99)  /* Maximum number of event commands */

/**********************/
/** Type Definitions **/
/**********************/

/* 
** An OO design would use constructs like abstract base classes
** for subsystems and events. This uses enums and structs that 
** need to be modified as subsystems and events evolve. 
*/

typedef enum {
   
   SCSIM_PHASE_UNDEF       = 0,
   SCSIM_PHASE_IDLE        = 1,
   SCSIM_PHASE_INIT        = 2,
   SCSIM_PHASE_TIME_LAPSE  = 3,
   SCSIM_PHASE_REALTIME    = 4
   
} SCSIM_Phase;

typedef enum {

   SCSIM_SUBSYS_UNDEF  = 0,
   SCSIM_SUBSYS_SIM    = 1,
   SCSIM_SUBSYS_ADCS   = 2,
   SCSIM_SUBSYS_CDH    = 3,
   SCSIM_SUBSYS_COMM   = 4,
   SCSIM_SUBSYS_PWR    = 5,
   SCSIM_SUBSYS_THRM   = 6,
   SCSIM_SUBSYS_COUNT  = 6
   
} SCSIM_SubSys;

typedef enum {

   SIM_EVT_UNDEF     = 0,
   SIM_EVT_IDLE      = 1,
   SIM_EVT_START_SIM = 2,
   SIM_EVT_STOP_SIM  = 3
   
} SIM_EvtCmd;

typedef enum {

   SCSIM_SCANF_UNDEF    = 0,
   SCSIM_SCANF_1_INT    = 1,
   SCSIM_SCANF_2_INT    = 2,
   SCSIM_SCANF_3_INT    = 3,
   SCSIM_SCANF_1_FLT    = 4,
   SCSIM_SCANF_3_FLT    = 5,
   SCSIM_SCANF_4_FLT    = 6,
   SCSIM_SCANF_NONE     = 7,
   SCSIM_SCANF_TYPE_CNT = 8
   
} SCSIM_SCANF_TYPE;

typedef struct {
   
   int32  OneInt;
   int32  TwoInt[2];
   int32  ThreeInt[3];
   float  OneFlt;
   float  ThreeFlt[3];
   float  FourFlt[4];
   
} SCSIM_EVT_CMD_PARAM;
/*
** Events have the general format of 
** - Time, Subsystem ID, Subsystem Event, Scanf Type, Parameters 
**
** See file prologue and macro section above for a simulation architecture
** overview.
*/

typedef struct {

   uint16  Prev;
   uint16  Next;
      
} SCSIM_EvtCmdLink;

typedef struct {

   SCSIM_EvtCmdLink Link;
   int32            Time;
   SCSIM_SubSys     SubSys;
   uint8            Id;
   SCSIM_SCANF_TYPE ScanfType;
   char*            Param;
      
} SCSIM_EvtCmd;


/*
** Subsystem telemetry on last event command sent.
*/ 
typedef struct {

   int32   Time;
   uint32  Id;    /* Keep 32-bit aligned */ 

} SCSIM_EvtCmdTlm;


/**********/
/** ADCS **/
/**********/

/* Preliminary events to get started. Need scenarios to determine what's needed */
typedef enum {

   ADCS_EVT_UNDEF          = 0,
   ADCS_EVT_SET_MODE       = 1,
   ADCS_EVT_ENTER_ECLIPSE  = 2,
   ADCS_EVT_EXIT_ECLIPSE   = 3,
   ADCS_EVT_SET_ATTITUDE   = 4,
   ADCS_EVT_SET_ORBIT      = 5

} ADCS_EvtCmd;

typedef enum {

   ADCS_MODE_UNDEF      = 0,
   ADCS_MODE_SAFEHOLD   = 1,
   ADCS_MODE_SUN_POINT  = 2,
   ADCS_MODE_INERTIAL   = 3,
   ADCS_MODE_SLEW       = 4

} ADCS_Mode;

typedef struct {


   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */
   
   boolean     Eclipse;
   ADCS_Mode   Mode;
   double      AttErr;
   
} ADCS_Model;


/*********/
/** CDH **/
/*********/

typedef enum {

   CDH_EVT_UNDEF             = 0,
   CDH_EVT_SET_REC_FILE_CNT  = 1, /* Percentage of recorder memory used */
   CDH_EVT_SET_REC_PCT_USED  = 2  /* Number of files in recorder */
   
} CDH_EvtCmd;


typedef struct {

   uint16  FileCnt;
   uint32  PctUsed;
   
} CDH_Recorder;

typedef struct {

   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */

   CDH_Recorder  Recorder;
   
} CDH_Model;

/**********/
/** COMM **/
/**********/

typedef enum {

   COMM_EVT_UNDEF         = 0,
   COMM_EVT_SCH_AOS       = 1,  /* Schedule relative start time, length, Link Type */
   COMM_EVT_LOS           = 2,  /* Schedule loss of signal */
   COMM_EVT_SET_DATA_RATE = 3,  /* Set rate for next contact */
   COMM_EVT_SET_TDRS_ID   = 4,
   COMM_EVT_ABORT_CONTACT = 5
   
} COMM_EvtCmd;


typedef enum {

   COMM_TDRS_UNDEF    = 0,
   COMM_TDRS_ATLANTIC = 1,
   COMM_TDRS_INDIAN   = 2,
   COMM_TDRS_PACIFIC  = 3
   
} COMM_TdrsId;


typedef enum {

   COMM_LINK_UNDEF    = 0,
   COMM_LINK_DUPLEX   = 1,
   COMM_LINK_SIMPLEX  = 2

} COMM_Link;

typedef struct {

   COMM_Link Link;
   uint16    TdrsId;
   uint16    DataRate;
   int16     TimePending;  /* Countdown timer until next contact. Negative value indicates no contect pending */ 
   uint16    Length;
   uint16    TimeConsumed;
   uint16    TimeRemaining;
        
} COMM_Contact;


typedef struct {

   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */

   boolean       InContact;
   COMM_Contact  Contact;
   
} COMM_Model;

/***********/
/** Power **/
/***********/

typedef enum {

   PWR_EVT_UNDEF          = 0,
   PWR_EVT_SET_BATT_SOC   = 1,
   PWR_EVT_SET_SA_CURRENT = 2

} PWR_EvtCmd;

typedef struct {

   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */

   float   BattSoc;
   float   SaCurrent;
   
} PWR_Model;

/*************/
/** Thermal **/
/*************/

typedef enum {

   THRM_EVT_UNDEF        = 0,
   THRM_EVT_ENA_HEATER_1 = 1,
   THRM_EVT_ENA_HEATER_2 = 2

} THRM_EvtCmd;

typedef struct {

   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */

   boolean   Heater1Ena;
   boolean   Heater2Ena;
        
} THRM_Model;


/******************************************************************************
** Command Packets
**
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   
   uint16  Id;

} SCSIM_StartSimCmdMsg;
#define SCSIM_START_SIM_CMD_DATA_LEN  (sizeof(SCSIM_StartSimCmdMsg) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

} SCSIM_StopSimCmdMsg;
#define SCSIM_STOP_SIM_CMD_DATA_LEN  (sizeof(SCSIM_StopSimCmdMsg) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
**
** - The simulation object telemetry must be separate from the HK packet that
**   provides simulation state information. The simulation status is sent and
**   displayed even when a simulated ground contact is not active. The 
**   subsystem sim model data is only sent when a ground contact is active.
**   
*/

typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   uint32   SimTime;
   boolean  SimActive;
   uint8    SimPhase;
   uint8    LastEvtSubSysId;   /* Last event command executed */
   uint8    LastEvtCmdId;
   uint8    NextEvtSubSysId;   /* Next command to be executed */
   uint8    NextEvtCmdId;
      
   SCSIM_EvtCmdTlm  AdcsLastEvtCmd;
   SCSIM_EvtCmdTlm  CdhLastEvtCmd;
   SCSIM_EvtCmdTlm  CommLastEvtCmd;
   SCSIM_EvtCmdTlm  PwrLastEvtCmd;
   SCSIM_EvtCmdTlm  ThrmLastEvtCmd;

} OS_PACK SCSIM_MgmtPkt;
#define SCSIM_MGMT_PKT_LEN sizeof (SCSIM_MgmtPkt)


typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];


   /*
   ** ADCS
   */

   boolean   Eclipse;
   uint8     AdcsMode;


   /*
   ** C&DH
   */

   uint16    RecFileCnt;
   float     RecPctUsed;

   /*
   ** Comm
   */
   
   boolean   InContact;
   uint8     Pad8;
   uint16    ContactTimePending;
   uint16    ContactTimeConsumed;
   uint16    ContactTimeRemaining;
   uint8     ContactLink;
   uint8     ContactTdrsId;
   uint16    ContactDataRate;
  
   /*
   ** Power
   */
   
   float   BattSoc;
   float   SaCurrent;
   
   /*
   ** Thermal
   */
      
   boolean   Heater1Ena;
   boolean   Heater2Ena;
   
} OS_PACK SCSIM_ModelPkt;
#define SCSIM_MODEL_PKT_LEN sizeof (SCSIM_ModelPkt)


/******************************************************************************
** SCSIM_Class
*/

typedef struct {

   /*
   ** Commands
   */
  
   /*
   ** Tables
   */
  
   SCSIMTBL_Struct Tbl;
   
   /*
   ** Telemetry Packets
   */
  
   SCSIM_MgmtPkt  MgmtPkt;
   SCSIM_ModelPkt ModelPkt;


   /*
   ** Objects
   */   


   /* Sim Management */
   
   boolean        Active;
   SCSIM_Phase    Phase;
   uint32         Time;
   
   SCSIM_EvtCmd*  LastEvtCmd;
   SCSIM_EvtCmd*  NextEvtCmd;   
   SCSIM_EVT_CMD_PARAM EvtCmdParam;

   SCSIM_EvtCmd*  Scenario;
   uint16         ScenarioId;
   uint16         RunTimeCmdIdx;
 
   /* Sim Models */
   
   ADCS_Model   Adcs;
   CDH_Model    Cdh;
   COMM_Model   Comm;
   PWR_Model    Pwr;
   THRM_Model   Thrm;
   

} SCSIM_Class;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: SCSIM_Constructor
**
** Initialize the intrument simulator to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**   2. The table values are not populated. This is done when the table is 
**      registered with the table manager.
**
*/
void SCSIM_Constructor(SCSIM_Class *ScSimPtr, CFE_SB_MsgId_t MgmtPktMsgId,
                       CFE_SB_MsgId_t ModelPktMsgId);


/******************************************************************************
** Function: SCSIM_Execute
**
** Execute a single simulation step.
**
*/
boolean SCSIM_Execute(void);


/******************************************************************************
** Function: SCSIM_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void SCSIM_ResetStatus(void);


/******************************************************************************
** Function: SCSIM_GetTblPtr
**
** Get pointer to SCSIM's table data
**
** Notes:
**  1. This function must match the SCSIMTBL_GetTblPtr definition.
**  2. Supplied as a callback to ScSsimTbl.
**
*/
const SCSIMTBL_Struct* SCSIM_GetTblPtr(void);


/******************************************************************************
** Function: SCSIM_LoadTbl
**
** Load data into SCSIM's table.
**
** Notes:
**  1. This function must match the SCSIMTBL_LoadTblFunc definition.
**  2. Supplied as a callback to ScSimTbl.
**
*/
boolean SCSIM_LoadTbl (SCSIMTBL_Struct* NewTbl);


/******************************************************************************
** Function: SCSIM_LoadEntry
**
** Load data into SCSIM's table.
**
** Notes:
**  1. This function must match the SCSIMTBL_LoadEntryFunc definition.
**  2. Supplied as a callback to ScSimTbl.
**
*/
boolean SCSIM_LoadTblEntry (uint16 ObjId, void* ObjData);


/******************************************************************************
** Functions: SCSIM_StartSimCmd
**
** Start a precanned simulation. Currently this is a placeholder.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**
*/
boolean SCSIM_StartSimCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);

/******************************************************************************
** Functions: SCSIM_StopSimCmd
**
** Stop the current simulation. Currently this is a placeholder.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**
*/
boolean SCSIM_StopSimCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _scsim_ */
