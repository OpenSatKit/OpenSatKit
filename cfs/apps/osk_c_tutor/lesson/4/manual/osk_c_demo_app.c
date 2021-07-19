/* LESSON 4 - Manual Updates
** 
** Make the changes below to teh current osk_c_tutor/fsw/src/osk_c_demo_app.c
** created fromprevious lessons.
** 
** Note that the code to load the table status in SendHousekeepingPkt() has 
** always been present so itdoesn't need to be updated in this lesson. The
** status has been meaningless since no tables have been defined.  
*/

/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** In InitApp() in the "Initialize app level interfaces" section add the following
** code to register the load/dump table commands. The code must be added after the
** CMDMGR_Constructor() called. I typically place it after the Reset Command registration.
*/

      CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_C_DEMO_TBL_LOAD_CMD_FC, TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_C_DEMO_TBL_DUMP_CMD_FC, TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);

/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** In InitApp() in the "Initialize app level interfaces" section add the following
** code to construct the table manager and register the load and dump functions. After
** the last command registration call is a good place. 
*/
	   TBLMGR_Constructor(TBLMGR_OBJ);
      TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, MSGLOGTBL_LoadCmd, MSGLOGTBL_DumpCmd, INITBL_GetStrConfig(INITBL_OBJ, CFG_TBL_LOAD_FILE));

