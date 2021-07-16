/* LESSON 5 - Manual Updates
** 
** Make the changes below to osk_c_tutor/fsw/src/msglog.h created
** in a previous lesson.
**
*/


/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** In the "Telemetry Packets" section under the "Playback ..." comment add the
** definition of the playback packet. 
*/

typedef struct {

   uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
   uint16  LogFileEntry;                     /* Log file entry being telemetered  */
   char    HdrTxt[MSGLOG_PRI_HDR_HEX_CHAR];  /* CCSDS v1.0 primary header content */

} MSGLOG_PlaybkPkt;

   
/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** In the "MSGLOG_Class" section under the "Class State Data" comment under
** the "PlaybkEna" line add the following definitions
*/

   uint16   PlaybkCnt;
   uint16   PlaybkDelay;
   
    
   
/* >>>>>>>>>>>>>>> STEP 3 <<<<<<<<<<<<<<<
**
** In the "MSGLOG_Class" section under the "Telemetry Packets" comment add the
** following definitions
*/
   
   MSGLOG_PlaybkPkt  PlaybkPkt;

