/* LESSON 6 - Manual Updates
** 
** Make the changes below to osk_c_tutor/fsw/src/app_cfg.h created
** in the previous lesson.
**
*/

/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** Update minor version to the lesson number. Helps confirm lesson updates
** sucessfully built and loaded.  
*/

#define  OSK_C_DEMO_MINOR_VER   6  /* Lesson ID */


/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** Add the following definitons to the group of "CFG_*" definitions  
*/

#define CFG_CHILD_NAME          CHILD_NAME
#define CFG_CHILD_PERF_ID       CHILD_PERF_ID
#define CFG_CHILD_STACK_SIZE    CHILD_STACK_SIZE
#define CFG_CHILD_PRIORITY      CHILD_PRIORITY


/* >>>>>>>>>>>>>>> STEP 3 <<<<<<<<<<<<<<<
**
** Add the following definitons to the "#define APP_CONFIG(XX) \" definition
*/

   XX(CHILD_NAME,char*) \
   XX(CHILD_PERF_ID,uint32) \
   XX(CHILD_STACK_SIZE,uint32) \
   XX(CHILD_PRIORITY,uint32) \


/* >>>>>>>>>>>>>>> STEP 4 <<<<<<<<<<<<<<<
**
** Add the following 'MSGLOG_RUN_CHILD_ALT_CMD_FC' definiton to the
** "Command Macros" section. I like to annotate which functions execute
** within a child task so you can annotate the start/stop log/playback
** commands with a comment if you'd like. 
*/


#define MSGLOG_RUN_CHILD_ALT_CMD_FC (CMDMGR_APP_START_FC + 6) /* Child */   


