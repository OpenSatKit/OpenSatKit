#!/usr/bin/env python
#========================================================================================
# File:    app_setup.py
# Author:  Jose F Martinez/GSFC
# Date:    2016-06-28
#========================================================================================

import sys, fileinput

appname = ""

def parse_args():
    global appname
    if len(sys.argv) != 2:
        print "\nUsage:  cd into the apps directory and run this script"
        print "          app_setup.py <appname>\t/* See SAMPLE_APPS.in for example */"
        print "\nEx:  "
        print "     $ cd /path/to/cfs/apps"
        print "     $ python ../tools/app_setup_scr/app_setup.py foo"
        sys.exit()
    else:
        tmpList = []
        tmpList.extend(sys.argv)
        # Remove script name
        tmpList.pop(0)
        appname = tmpList.pop(0)

#========================================================================================

def getLineWithStr(filepath, lookup):
    with open(filepath, 'r') as myFile:
        for line in myFile:
            if lookup in line:
                myFile.close()
                return line
        myFile.close()
        return ""

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

def modifySchLab():

    # Modify cmake file
    filepath = 'sch_lab/CMakeLists.txt'
    line = 'include_directories(${%s_MISSION_DIR}/fsw/platform_inc)' % ( appname.lower() )
    lookup = 'include_directories('
    addLineToFile(filepath, line, lookup)

    # Modify sch_lab table. Add header file
    filepath = 'sch_lab/fsw/platform_inc/sch_lab_sched_tab.h'
    line = '#include "%s_msgids.h"' % ( appname.lower() )
    lookup = '"cfe_msgids.h"'
    addLineToFile(filepath, line, lookup)

    # Modify sch_lab table. Add HK_MID to table
    line = '     { %s_SEND_HK_MID,  4, 0 },' % ( appname.upper() )
    lookup = 'CFE_TBL_SEND_HK_MID'
    addLineToFile(filepath, line, lookup)

#========================================================================================

def modifyTO():

    # Modify cmake file
    filepath = 'to/CMakeLists.txt'
    line = 'include_directories(${%s_MISSION_DIR}/fsw/platform_inc)' % ( appname.lower() )
    lookup = 'include_directories('
    addLineToFile(filepath, line, lookup)

    # Modify sch_lab table. Add header file
    filepath = 'to/fsw/tables/to_config.c'
    line = '#include "%s_msgids.h"' % ( appname.lower() )
    lookup = '"cfe_msgids.h"'
    addLineToFile(filepath, line, lookup)

    # Modify sch_lab table. Add HK_MID to table
    line = '       {%s_HK_TLM_MID,                {0,0},  1,   0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},' % ( appname.upper() )
    lookup = 'CFE_TIME_HK_TLM_MID'
    addLineToFile(filepath, line, lookup)

#========================================================================================

def modifyCmakeTargets():

    # Modify cmake file
    filepath = '../sample_defs/targets.cmake'
    lookup = 'SET(TGT1_APPLIST'
    line = getLineWithStr(filepath, lookup)
    if line and not appname.lower() in line:
        line = line[:-2] + ' %s)' % ( appname.lower() )
        addLineToFile(filepath, line, lookup, True)
    else:
        print 'Not modified ' + filepath

#========================================================================================

def modifyStartupScr():

    # Modify cmake file
    filepath = '../sample_defs/cpu1_cfe_es_startup.scr'
    lookup = '!'
    if not getLineWithStr(filepath, '/%s.so'%(appname.lower())): # check if new app is already there
        line = getLineWithStr(filepath, lookup)
        if line:
            line = 'CFE_APP, /cf/%s.so,         %s_AppMain,     %s,          80,   16384, 0x0, 0;\n!' % ( appname.lower(), appname.upper(), appname.upper() )
            addLineToFile(filepath, line, lookup, True)
            return
    
    print 'Not modified ' + filepath

#========================================================================================

parse_args()
# modifySchLab() # Not needed if we use the same msgid as Executive services in new apps
modifyTO()
modifyCmakeTargets()
modifyStartupScr()

#========================================================================================
# End of app_setup.py
#========================================================================================
