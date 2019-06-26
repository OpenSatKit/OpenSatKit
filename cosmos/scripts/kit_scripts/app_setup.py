#!/usr/bin/env python
#========================================================================================
# File:    app_setup.py
# Author:  Jose F Martinez/GSFC
# Date:    2016-06-28
#========================================================================================

import sys, fileinput, os, app_settings

appname = ""
g_OutDir = "."

#========================================================================================

def inits():
    global g_OutDir

    g_OutDir = app_settings.get_outdir()

#========================================================================================

def getLineNumWithStr(filepath, lookup):
    with open(filepath, 'r') as myFile:
        for num, line in enumerate(myFile, 0):
            if lookup in line:
                myFile.close()
                return num
        myFile.close()
        return 0

#========================================================================================

def addLineToFile(filepath, line, lookup, replace=False):

    # Don't write to file is line is already in file
    alreadyInFile = getLineNumWithStr(filepath, line)
    if not alreadyInFile:
        linenum = getLineNumWithStr(filepath, lookup)
        lines = open(filepath, 'r').readlines()
        if not replace:
            lines.insert(linenum +  1, line + '\n')
        else:
            lines[linenum] = line + '\n'
        out = open(filepath, 'w')
        out.writelines(lines)
        out.close()
        print 'Added ' + line +' to '+ filepath
    else:
        print 'Not modified ' + filepath + ': File already contains ' + appname + ' definitions.'

#========================================================================================

def modifyCmdTlmServer(app_name):
    global appname
    appname = app_name
    if not appname:
        print 'Error: appname is empty string'
        return

    # Modify cmake file
    filepath = os.path.join(g_OutDir, "..", "tools", "cmd_tlm_server", "cmd_tlm_server.txt")
    line = '  TARGET %s' % ( appname.upper() )
    lookup = 'TARGET CFE_CORE'
    addLineToFile(filepath, line, lookup)




#========================================================================================
# End of app_setup.py
#========================================================================================