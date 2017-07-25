#========================================================================================
# File:    app_config_files.py
# Author:  Jose F Martinez/GSFC
# Date:    2016-06-29
#========================================================================================

import os, app_settings, app_utils

#========================================================================================

# Global Variables - only global to this file
g_OutDir = "."

#========================================================================================

def inits():
    global g_OutDir

    g_OutDir = app_settings.get_outdir()

#========================================================================================

def construct_cmd_content(tgtApp):
    content = """\
COMMAND %s %s_NOOP LITTLE_ENDIAN "This command will increment the command execution counter"
  APPEND_ID_PARAMETER CCSDS_STREAMID 16 UINT MIN_UINT16 MAX_UINT16 0x18C0 "CCSDS Packet Identification" BIG_ENDIAN
  APPEND_PARAMETER CCSDS_SEQUENCE 16 UINT MIN_UINT16 MAX_UINT16 0xC000 "CCSDS Packet Sequence Control" BIG_ENDIAN
  APPEND_PARAMETER CCSDS_LENGTH 16 UINT MIN_UINT16 MAX_UINT16 1 "CCSDS Packet Data Length" BIG_ENDIAN
  APPEND_PARAMETER CCSDS_RESERVED 1 UINT 0x0000 0x0000 0x0000 "Researved CCSDS Bit" BIG_ENDIAN
  APPEND_PARAMETER CHECKSUM 8 UINT MIN_UINT8 MIN_UINT8 0x0000 "Checksum" BIG_ENDIAN
  APPEND_PARAMETER COMMAND_CODE 7 UINT MIN_UINT8 MAX_UINT8 0 "Command code" BIG_ENDIAN


COMMAND %s %s_RESET_COUNTER LITTLE_ENDIAN "Reset Counters"
  APPEND_ID_PARAMETER CCSDS_STREAMID 16 UINT MIN_UINT16 MAX_UINT16 0x18C0 "CCSDS Packet Identification" BIG_ENDIAN
  APPEND_PARAMETER CCSDS_SEQUENCE 16 UINT MIN_UINT16 MAX_UINT16 0xC000 "CCSDS Packet Sequence Control" BIG_ENDIAN
  APPEND_PARAMETER CCSDS_LENGTH 16 UINT MIN_UINT16 MAX_UINT16 1 "CCSDS Packet Data Length" BIG_ENDIAN
  APPEND_PARAMETER CCSDS_RESERVED 1 UINT 0x0000 0x0000 0x0000 "Researved CCSDS Bit" BIG_ENDIAN
  APPEND_PARAMETER CHECKSUM 8 UINT MIN_UINT8 MIN_UINT8 0x0000 "Checksum" BIG_ENDIAN
  APPEND_PARAMETER COMMAND_CODE 7 UINT MIN_UINT8 MAX_UINT8 1 "Command code" BIG_ENDIAN
""" % (tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper())

    return content
    
#========================================================================================

def construct_tlm_content(tgtApp):
    content = """\
TELEMETRY %s CFE_%s_HKTLMPKT LITTLE_ENDIAN "%s Housekeeping Packet"
  APPEND_ID_ITEM CCSDS_STREAMID 16 UINT 0x08BB "CCSDS Packet Identification" BIG_ENDIAN
  APPEND_ITEM CCSDS_SEQUENCE 16 UINT "CCSDS Packet Sequence Control" BIG_ENDIAN
  APPEND_ITEM CCSDS_LENGTH   16 UINT "CCSDS Packet Data Length" BIG_ENDIAN
  APPEND_ITEM CCSDS_SECONDS  32 UINT "CCSDS Telemetry Secondary Header (seconds)"
  APPEND_ITEM CCSDS_SUBSECS  16 UINT "CCSDS Telemetry Secondary Header (subseconds)"
  APPEND_ITEM COMMAND_COUNT  8  UINT "Command Count"
  APPEND_ITEM ERROR_COUNT    8  UINT "Error Count"

""" % (tgtApp.upper(), tgtApp.upper(), tgtApp.upper())

    return content
    
#========================================================================================

def construct_screen_content(tgtApp):
    content = """\
SCREEN AUTO AUTO 0.5
GLOBAL_SETTING LABELVALUELIMITSBAR COLORBLIND TRUE
VERTICAL
  TITLE "%s Screen"
    SETTING BACKCOLOR 162 181 205
    SETTING TEXTCOLOR black
  VERTICALBOX
    SECTIONHEADER "%s"
    LABELVALUE %s CFE_%s_HKTLMPKT COMMAND_COUNT WITH_UNITS 30
    LABELVALUE %s CFE_%s_HKTLMPKT ERROR_COUNT WITH_UNITS 30
    BUTTON '%s NOOP' 'target_name = get_target_name("%s"); cmd("%s %s_NOOP with CCSDS_STREAMID 6336, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CHECKSUM 0, COMMAND_CODE 0")'
    BUTTON '%s Erroneous Command' 'target_name = get_target_name("%s"); cmd("%s %s_RESET_COUNTER with CCSDS_STREAMID 6336, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CHECKSUM 0, COMMAND_CODE 20")'
    BUTTON '%s Reset Counters' 'target_name = get_target_name("%s"); cmd("%s %s_RESET_COUNTER with CCSDS_STREAMID 6336, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CHECKSUM 0, COMMAND_CODE 1")'
  END
  SETTING BACKCOLOR 163 185 163
  VERTICALBOX
    SECTIONHEADER "General Telemetry"
    BUTTON 'TO Enable Telemetry to localhost' 'target_name = get_target_name("TO"); cmd("TO TO_ENABLE_TELEMETRY with CMD_ID 6272, SEQUENCE 49152, SIZE 19, COMMAND_CODE 2, DATA \\'127.0.0.1\\', PORT 1235")'
    SETTING GRAY_TOLERANCE 0.1
  END
  SETTING BACKCOLOR 203 173 158
  VERTICALBOX
    SECTIONHEADER "Start New App while cFS is Running"
    BUTTON 'Start %s' 'target_name = get_target_name("CFE_CORE"); cmd("CFE_CORE CFE_ES_START_APP with CCSDS_STREAMID 6150, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 113, CCSDS_CHECKSUM 0, CCSDS_FC 4, APPLICATION \\'%s\\', APPENTRYPOINT \\'%s_AppMain\\', APPFILENAME \\'/cf/%s.so\\', STACKSIZE 8192, EXCEPTIONACTION 0, PRIORITY 0")'
    BUTTON 'Add %s entry to TO table' 'target_name = get_target_name("TO"); cmd("TO TO_ADD_TBL_ENTRY with CCSDS_STREAMID 6272, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 17, CCSDS_RESERVED 0, CHECKSUM 0, COMMAND_CODE 6, MSG_ID 2235, QOS 0, MSG_LIMIT 1, ROUTE_MASK 65535, GROUP_DATA 33554433, FLAG 0, STATE 1")'
    SETTING GRAY_TOLERANCE 0.1
  END
  SETTING BACKCOLOR 203 173 158
END
SETTING BACKCOLOR 162 181 205

""" % (tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.upper(), tgtApp.lower(), tgtApp.upper()) 

    return content
    
#========================================================================================

def generate_cmd_file(tgtApp):
    global g_OutDir

    # Open file
    filePath = os.path.join(g_OutDir, tgtApp.upper(), "cmd_tlm", tgtApp.upper()+"_CMD.txt")
    file_exist_flg = os.path.exists(filePath)
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_cmd_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, file_exist_flg)

def generate_tlm_file(tgtApp):
    global g_OutDir

    # Open file
    filePath = os.path.join(g_OutDir, tgtApp.upper(), "cmd_tlm", tgtApp.upper()+"_HKTLMPKT.txt")
    file_exist_flg = os.path.exists(filePath)
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_tlm_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, file_exist_flg)

def generate_screen_file(tgtApp):
    global g_OutDir

    # Open file
    filePath = os.path.join(g_OutDir, tgtApp.upper(), "screens", tgtApp.lower()+"_screen.txt")
    file_exist_flg = os.path.exists(filePath)
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_screen_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, file_exist_flg)

#========================================================================================
# End of app_config_files.py
#========================================================================================

