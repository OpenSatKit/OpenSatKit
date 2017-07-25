#========================================================================================
# File:    app_settings.py
# Author:  Tam Ngo/JSC
# Date:    2012-02-22
# Modification History:
#   Date | Author | Description
#   ---------------------------
#   06/29/16 | Jose F Martinez | Modified to be used to generate cosmos config files
#========================================================================================

import os, sys, time, datetime, app_utils

#========================================================================================

# Global variables - only global to this file
g_Date    = ""

g_OutDir = "."

g_Apps = []

#========================================================================================

def parse_configs():
    global g_OutDir, g_Date, g_Apps

    if len(sys.argv) <= 1:
        print "\nUsage:  gen_app_code.py <input-file>\t/* See SAMPLE_APPS.in for example */"
        print "\n        gen_app_code.py [OUTDIR <output-dir>]\t    /* No spaces in path */"
        print "                        [APPS <app-list>]\t    /* List of app names, \
separated by spaces; No spaces in name */"
        print "\nEx:  "
        print "     $ python gen_app_code.py OUTDIR /vagrant/cosmos/config/targets APPS appname"
        print "     $ gen_app_code.py OUTDIR /path/to/cosmos/config/targets APPS appname"
        print "     $ gen_app_code.py SAMPLE_APPS.in"
        sys.exit()
    else:
        tmpList = []
        tmpList.extend(sys.argv)
        # Remove script name
        tmpList.pop(0)

        curOpt  = ""
        optList = ['OUTDIR', 'APPS']

        for itm in tmpList:
            if itm in optList:
                curOpt = itm
            elif curOpt == "":
                parse_input_file(itm)
                break
            elif curOpt == "OUTDIR":
                g_OutDir = itm
            elif curOpt == "APPS":
                g_Apps.append(itm)
            else:
                pass

    g_Date = datetime.date.today()

#========================================================================================

def parse_input_file(fileName):
    global g_OutDir, g_Apps

    # Open file for reading, if exists and is valid; else exit
    iFileHdl = app_utils.open_file_for_reading(fileName)

    # Parse the input file 
    for line in iFileHdl.readlines():
        if not line[0] == '#':
            words = line.split()
            if len(words) > 0:
                key = words[0]
                value = " ".join(words[1:])
                if key == "OUTDIR":
                    g_OutDir = value
                elif key == "APPS":
                    g_Apps.extend(value.split())
                else:
                    pass

    iFileHdl.close()

#========================================================================================

def print_configs():
    global g_OutDir, g_Date, g_Apps

    print """
DATE - %s

OUTDIR - %s

APPS - %s
    """ % (g_Date, g_OutDir, g_Apps)

#========================================================================================

def get_date():
    global g_Date

    return g_Date

#========================================================================================

def get_outdir():
    global g_OutDir

    return g_OutDir

#========================================================================================

def get_apps():
    global g_Apps

    return g_Apps

#========================================================================================
# End of app_settings.py
#========================================================================================

