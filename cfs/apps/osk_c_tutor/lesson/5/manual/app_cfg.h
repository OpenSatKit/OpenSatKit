/* LESSON 5 - Manual Updates
** 
** Make the changes below to osk_c_tutor/fsw/src/app_cfg.h created
** in teh previous lesson.
**
*/

/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** Update minor version to the lesson number. Helps confirm lesson updates
** sucessfully built and loaded.  
*/

#define  OSK_C_DEMO_MINOR_VER   5  /* Lesson ID */


/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** Add the following definitons to the group of "CFG_*" definitions  
*/

#define CFG_PLAYBK_TLM_MID      PLAYBK_TLM_MID


/* >>>>>>>>>>>>>>> STEP 3 <<<<<<<<<<<<<<<
**
** Add the following definitons to the "#define APP_CONFIG(XX) \" definition
*/

   XX(PLAYBK_TLM_MID,uint32) \


