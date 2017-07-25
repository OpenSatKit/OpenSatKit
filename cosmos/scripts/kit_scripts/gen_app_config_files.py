#!/usr/bin/env python
#========================================================================================
# File:    gen_app_config_files.py
# Author:  Jose F Martinez/GSFC
# Date:    2016-06-29
#
# Modification History:
#   Date | Author | Description
#   ---------------------------
#   06/29/16 | Jose F Martinez | Created files
#========================================================================================

import os, sys, app_utils, app_settings, app_setup
import app_config_files

#========================================================================================

# Parse for the app's settings
app_settings.parse_configs()
app_settings.print_configs()

print ""
# Call the module's init() to grab configuration values
app_config_files.inits()
app_setup.inits()


g_Apps = []
g_Apps.extend(app_settings.get_apps())

# Generate code for a list of application names
for itm in g_Apps:
    print ""
    # Create the necessary directory and sub-directories
    app_utils.create_target_dirs(itm)
    app_config_files.generate_cmd_file(itm)
    app_config_files.generate_tlm_file(itm)
    app_config_files.generate_screen_file(itm)
    app_setup.modifyCmdTlmServer(itm)
    print ""

#========================================================================================
# End of gen_app_config_files.py
#========================================================================================

