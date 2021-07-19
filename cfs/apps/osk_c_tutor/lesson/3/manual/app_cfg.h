/* LESSON 3 - Manual Updates
** 
** Make the changes below to osk_c_tutor/fsw/src/app_cfg.h created
** in lesson 1.
**
*/

/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** Update minor version to the lesson number. Helps confirm lesson updates
** sucessfully built and loaded.  
*/

#define  OSK_C_DEMO_MINOR_VER   3  /* Lesson ID */


/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** Add the following definitons to the group of "CFG_*" definitions  
*/

#define CFG_MSGLOG_PIPE_DEPTH   MSGLOG_PIPE_DEPTH
#define CFG_MSGLOG_PIPE_NAME    MSGLOG_PIPE_NAME


/* >>>>>>>>>>>>>>> STEP 3 <<<<<<<<<<<<<<<
**
** Add the following definitons to the "#define APP_CONFIG(XX) \" definition
*/

   XX(MSGLOG_PIPE_DEPTH,uint32) \
   XX(MSGLOG_PIPE_NAME,char*) \

