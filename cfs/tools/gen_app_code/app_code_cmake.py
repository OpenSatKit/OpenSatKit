#!/usr/bin/env python
#========================================================================================
# File:    app_code_cmake.py
# Author:  Jose F Martinez/GSFC
# Date:    2016-06-28
#========================================================================================

import os, app_code_configs, app_utils

#========================================================================================

# Global Variables - only global to this file
g_OutDir = "."

#========================================================================================

def inits():
    global g_OutDir

    g_OutDir = app_code_configs.get_outdir()

#========================================================================================

def construct_cmakelists_content(tgtApp):

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """\
cmake_minimum_required(VERSION 2.6.4)
project(CFE_%s C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(%s ${APP_SRC_FILES})

""" % (ucApp, lcApp)

    return content

#========================================================================================

def generate_cmakelists(tgtApp):
    global g_OutDir

    # Open file
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "CMakeLists.txt")
    file_exist_flg = os.path.exists(filePath)
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_cmakelists_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, file_exist_flg)

#========================================================================================
# End of app_code_cmake.py
#========================================================================================

