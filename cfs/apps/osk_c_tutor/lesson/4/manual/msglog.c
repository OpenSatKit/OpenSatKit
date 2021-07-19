/* LESSON 4 - Manual Updates
** 
** Make the changes below to osk_c_tutor/fsw/src/msglog.c created
** in lesson 3.
**
*/


/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** In the "Macro Definitions" remove the macros defined in the 
** previous lesson:
**
**   #define PARAM_FILE_PATH_BASE_NAME  "/cf/msg_"
**   #define PARAM_FILE_EXTENSION       ".txt"
**   #define PARAM_FILE_ENTRY_CNT       5
*/


/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** In the "Macro Definitions" section under "Convenience Macro" add
** the following definition.
*/

#define TBL_OBJ (&(MsgLog->Tbl))



/* >>>>>>>>>>>>>>> STEP 3 <<<<<<<<<<<<<<<
**
** In MSGLOG_Constructor() add the following at the end of the function
*/

   MSGLOGTBL_Constructor(TBL_OBJ, IniTbl);


/* >>>>>>>>>>>>>>> STEP 4 <<<<<<<<<<<<<<<
**
** Search and replace all occurrences of the macros used in the
** previous lesson as follows:
**
**   PARAM_FILE_PATH_BASE_NAME   =>  MsgLog->Tbl.Data.File.PathBaseName
**   PARAM_FILE_EXTENSION        =>  MsgLog->Tbl.Data.File.Extension
**   PARAM_FILE_ENTRY_CNT        =>  MsgLog->Tbl.Data.File.EntryCnt
*/



