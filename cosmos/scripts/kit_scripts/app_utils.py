#========================================================================================
# File:    app_utils.py
# Author:  Tam Ngo/JSC
# Date:    2012-02-22
# Modification History:
#   Date | Author | Description
#   ---------------------------
#   06/29/16 | Jose F Martinez | Modified to be used to generate cosmos config files
#========================================================================================

import os, sys, shutil, app_settings

#========================================================================================

def create_a_dir(dirName):
    if not os.path.exists(dirName):
        os.mkdir(dirName, 0755)
        print "Created directory ", dirName

#========================================================================================

def create_target_dirs(tgtApp):
    OutputDir = app_settings.get_outdir()

    # Create the app's top-level directory
    level0 = os.path.join(OutputDir, tgtApp.upper())
    create_a_dir(level0)

    # Create "fsw" sub-directory
    create_a_dir(os.path.join(level0, "cmd_tlm"))
    create_a_dir(os.path.join(level0, "screens"))
    

#========================================================================================

def remove_dir_tree(dirName):
    if os.path.exists(dirName):
        shutil.rmtree(dirName)
        print "Removed   ", dirName, "and its sub-directories"

#========================================================================================

def open_file_for_reading(fileName):
    # First, check for existence of input; else exit
    if not os.path.exists(fileName):
        print "\nError:", fileName, "does not exist!\n"
        sys.exit()

    # Second, check for valid file type; else exit
    if os.path.isdir(fileName):
        print "\nError:", fileName, " is a directory!\n"
        sys.exit()

    # Third, check for file's read access; else exit
    if not os.access(fileName, os.R_OK):
        print "\nError:", fileName, "is not readable - check file permission!\n"
        sys.exit()

    # When all checks are valid, open file for reading; return file handle
    return open(fileName, 'r')

#========================================================================================

def open_file_for_writing(fileName, mustExistFlg):
    if mustExistFlg:
        # First, check for existence of input; else exit
        if not os.path.exists(fileName):
            print "\nError:", fileName, "does not exist!\n"
            sys.exit()

        # Second, check for valid file type; else exit
        if os.path.isdir(fileName):
            print "\nError:", fileName, "is a directory!\n"
            sys.exit()

        # Third, check for file's write access; else exit
        if not os.access(fileName, os.W_OK):
            print "\nError:", fileName, " not writable - check file permission!\n"
            sys.exit()

    # Open file for writing; return file handle
    return open(fileName, 'w')

#========================================================================================

def close_file_from_writing(fileHdl, fileName, fileExistFlg):
    fileHdl.close()
    if fileExistFlg:
        print "Over-written  ", fileName
    else:
        print "Created  ", fileName

#========================================================================================

def remove_file(fileName):
    if os.path.exists(fileName):
        os.remove(fileName)
        print "Removed  ", fileName

#========================================================================================
# End of app_utils.py
#========================================================================================

