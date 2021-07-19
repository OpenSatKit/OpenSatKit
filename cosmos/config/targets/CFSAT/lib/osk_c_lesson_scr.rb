###############################################################################
# OSK C Tutor Screen
#
# Notes:
#   None
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'cosmos'
require 'osk_ops'
require 'osk_system'
require 'osk_education'
require 'fsw_const'
require 'fsw_config_param'
require 'osk_c_demo_const'
require 'cfsat_const'

################################################################################
## GUI Commands
################################################################################

def osk_c_lesson_cmd(screen, cmd)
 
   lesson_id     = screen.get_named_widget("title").text[/\d+/]
   
   case cmd 
   
   when "REF_SLIDES"
      lesson_slide_file = File.join(CfSat::TUTOR_DOC_DIR,"#{CfSat::TUTOR_APP_NAME}_lesson_#{lesson_id}.pdf")
      Osk::System.display_pdf(lesson_slide_file)
   
   when "REF_OSK_APP_DEV"
      osk_app_dev_file = File.join(Osk::OSK_DOCS_DIR,Osk::OSK_APP_DEV_FILE)
      Osk::System.display_pdf(osk_app_dev_file)
   
   when "REF_TUTORIAL"
      tutorial_file = File.join(CfSat::TUTOR_DOC_DIR,CfSat::APP_TUTOR_GUIDE_FILE)
      Osk::System.display_pdf(tutorial_file)
   
   when "LESSON_SAVE_STATUS"
   
      lesson_dir     = File.join(CfSat::TUTOR_LESSON_DIR,lesson_id.to_s)
      user_json_file = File.join(lesson_dir,CfSat::LESSON_JSON_USER_FILE)
      
      if (File.exists?(user_json_file))
   
         lesson_json = File.read(user_json_file)
         lesson_info = JSON.parse(lesson_json)
         
         lesson_info["time-stamp"] = Osk::time_stamp

         lesson_src_file = lesson_info["src-file"]
         
         # source file name is the index
         lesson_src_file.each_with_index do |src_file, idx|
            src_filename = src_file[0]
            status       = src_file[1]
            prev_completed = status["completed"]
            status["completed"] = screen.get_named_widget("#{src_filename.sub('.','_')}").checked
            if (prev_completed == false and status["completed"] == true)
               status["completed-time"] = Osk::time_stamp
            end            
         end 
          
         File.open("#{user_json_file}","w") do |f| 
            f.write(JSON.pretty_generate(lesson_info))
         end

      end

   when "LESSON_COMPLETE"
      clear("CFSAT OSK_C_LESSON_SCR")
      user_tutor_info = osk_c_tutor_read_user_json
      if (user_tutor_info.nil?)
         raise "Tutor/lesson JSON files corrupted."
      else
         lesson = user_tutor_info["lesson"]
         id = lesson_id.to_i
         (lesson[id-1])["completed"] = true
         if (id < lesson.length)
            user_tutor_info["current-lesson"] = id+1
         end
         osk_c_tutor_write_user_json(user_tutor_info)
         
         clear("CFSAT OSK_C_TUTOR_SCR")
         osk_c_tutor_start
         display("CFSAT OSK_C_TUTOR_SCR")
      end
      
   when "APP_BUILD"
      Osk::System.build_cfs("Building App Tutor Lesson #{lesson_id}")
   
   when "APP_RUN"
      #
      if (not Osk::System.cfs_running?)
         Osk::System.start_cfs('cfsat')
         wait 4  # Give the cFS time to start
      end
      # If Send App Info succeeds then OSK_C_DEMO is running and the cFS must be restarted
      # because reloading an app on Linux (OSAL 5.0.1) doesn't work
      begin
         Osk::flight.cfe_evs.send_cmd("DIS_APP_EVENT_TYPE with APP_NAME CFE_ES, BITMASK #{Fsw::Const::CFE_EVS_ERROR_MASK}")
         if (Osk::flight.cfe_es.send_cmd("SEND_APP_INFO with APP_NAME OSK_C_DEMO",Osk::OVERRIDE_TO_TRUE))         
            #Osk::flight.cfe_es.send_cmd("STOP_APP with APP_NAME OSK_C_DEMO")
            #wait 5  # Give time for app to stop before starting new version
            Osk::System.stop_n_start_cfs('cfsat')
            wait 4  # Give the cFS time to start
         end         
      rescue
         # Silent rescue because okay for app not to be running
      end
      Osk::flight.cfe_evs.send_cmd("ENA_APP_EVENT_TYPE with APP_NAME CFE_ES, BITMASK #{Fsw::Const::CFE_EVS_ERROR_MASK}")
      app = Osk::flight.app["OSK_C_DEMO"]
      Osk::flight.cfe_es.send_cmd("START_APP with APP_NAME #{app.fsw_name}, APP_ENTRY_POINT #{app.entry_symbol}, APP_FILENAME #{app.obj_path_filename}, STACK_SIZE #{app.stack_size}, EXCEPTION_ACTION 0, PRIORITY #{app.priority}")
      
   when "APP_SCREEN"
      display("OSK_C_DEMO DEMO_OPS_SCREEN")
  
   else
      raise "Error in screen definition file. Undefined cmd '#{cmd}' sent to osk_c_lesson_cmd()"
   end
  
end # osk_c_lesson_cmd()

################################################################################
## Lesson Source Code 
################################################################################

def osk_c_lesson_src(screen, location)
 
   lesson_id = screen.get_named_widget("title").text[/\d+/]

   # Use COSMOS editor for editing/updating current source files   
   if (location == "SRC_EDIT_CURRENT")
   
      src_file = screen.get_named_widget("current_src_file").text
      src_pathfile = File.join(CfSat::TUTOR_FSW_SRC_DIR,src_file)
      Cosmos.open_in_text_editor(src_pathfile)
   
   else 
      
      case location
      when "SRC_VIEW_LESSON"
         src_dir  = File.join(CfSat::TUTOR_APP_DIR,'lesson',lesson_id.to_s,'manual')
         src_file = screen.get_named_widget("lesson_src_file").text
      when "SRC_VIEW_SOLUTION"
         src_dir  = File.join(CfSat::TUTOR_APP_DIR,'lesson',lesson_id.to_s,'solution')
         src_file = screen.get_named_widget("solution_src_file").text
      else
         raise "Error in screen definition file. Undefined cmd '#{cmd}' sent to osk_c_lesson_cmd()"
      end

      src_pathfile = File.join(src_dir,src_file)
      Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{src_pathfile}'")

   end # End 
   
end # osk_c_lesson_src()


################################################################################
## Create Lesson Screen
################################################################################

def osk_c_lesson_create_scr(lesson_id, lesson_info)

   # Had trouble with quotes in scr_header & cleaner to do outside of string
   lesson_time_stamp = lesson_info["time-stamp"] 
   lesson_title      = lesson_info["title"] 
   lesson_objective  = lesson_info["objective"]
   lesson_src_file   = lesson_info["src-file"]

   # Build LABEL statements with JSON defined objective text
   objective_str = ""
   lesson_objective.each do |objective|
      objective_str << "LABEL \"#{objective}\"\n      "
   end

   # Build view lesson/solution COMBOBOX dropdown lists
   # based on the lesson definition  JSON file not the
   # user's current status JSON file

   lesson_base_dir  = File.join(CfSat::TUTOR_LESSON_DIR,lesson_id.to_s)
   base_json_file   = File.join(lesson_base_dir,CfSat::LESSON_JSON_BASE_FILE)
   base_lesson_json = File.read(base_json_file)
   base_lesson_info = JSON.parse(base_lesson_json)
   base_src_file    = base_lesson_info["src-file"]
   
   lesson_src_dropdown = ""
   base_src_file.each do |src_file|
      src_filename = src_file[0]
      status       = src_file[1]
      if (status["completed"] == false)
        lesson_src_dropdown << "'#{src_filename}' "
      end
   end
   
   scr_header = "
   
   ###############################################################################
   # OSK C Lesson Screen
   #
   # Notes:
   #   1. Do not edit this file because it is automatically generated and your
   #      changes will not be saved.
   #   2. File created by osk_c_tutor_scr.rb on #{Osk::time_stamp}
   #
   # License:
   #   Written by David McComas, licensed under the copyleft GNU General Public
   #   License (GPL). 
   #
   ###############################################################################
   <% 
      require 'osk_c_lesson_scr' 
   %>
   SCREEN AUTO AUTO 0.5
   GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
  
   NAMED_WIDGET title TITLE \"Lesson #{lesson_id} - #{lesson_title}\"
     SETTING BACKCOLOR 162 181 205
     SETTING TEXTCOLOR black

   VERTICALBOX \"Objective\"
     #{objective_str}
   END # Objectives

   VERTICALBOX \"References\"

      MATRIXBYCOLUMNS 4
         BUTTON 'Lesson Slides'  'osk_c_lesson_cmd(self,\"REF_SLIDES\")'
         BUTTON 'OSK App Dev'    'osk_c_lesson_cmd(self,\"REF_OSK_APP_DEV\")'
         BUTTON 'Tutorial Guide' 'osk_c_lesson_cmd(self,\"REF_TUTORIAL\")'
         LABEL  '        '
      END # Matrix

   END # References
       
   VERTICALBOX \"Update Source Files\"

      MATRIXBYCOLUMNS 3 5 5
         BUTTON 'View Lesson'   'osk_c_lesson_src(self,\"SRC_VIEW_LESSON\")'
         NAMED_WIDGET lesson_src_file COMBOBOX #{lesson_src_dropdown}
         LABEL  '        '

         BUTTON 'View Solution' 'osk_c_lesson_src(self,\"SRC_VIEW_SOLUTION\")'
         NAMED_WIDGET solution_src_file COMBOBOX #{lesson_src_dropdown}
         LABEL  '        '

         BUTTON 'Edit Current FSW'  'osk_c_lesson_src(self,\"SRC_EDIT_CURRENT\")'
         NAMED_WIDGET current_src_file COMBOBOX 'app_cfg.h' 'osk_c_demo_app.h' 'osk_c_demo_app.c' 'msglog.h' 'msglog.c' 'msglogtbl.h' 'msglogtbl.c'
         LABEL  '        '
      END
      
   END

   VERTICALBOX \"Completed\"

      MATRIXBYCOLUMNS 3 5 5
   "
   
   scr_trailer = "
      END # Matrix
   END # Completed


   VERTICALBOX \"Application\"
      MATRIXBYCOLUMNS 4 5
         
         BUTTON 'Build'  'osk_c_lesson_cmd(self,\"APP_BUILD\")'
         BUTTON 'Run'    'osk_c_lesson_cmd(self,\"APP_RUN\")'
         BUTTON 'Screen' 'osk_c_lesson_cmd(self,\"APP_SCREEN\")'
         LABEL '        '
         
      END
   END # Application
   "

   scr_file = Osk::cfg_target_dir_file("CFSAT","screens","osk_c_lesson_scr.txt")
   
   begin
         
      # Always overwrite the temp file      
      File.open(scr_file,"w") do |f| 
           
         f.write (scr_header)

         # Even thought loop didn't work out, I left common screen pattern of dynamic text
         # between header and footer strings
         scr_text = "
            NAMED_WIDGET app_cfg_h CHECKBUTTON 'app_cfg.h' #{((lesson_src_file["app_cfg.h"])["completed"] == true)? "CHECKED" : "UNCHECKED"}
            LABEL '        '
            LABEL '        '

            NAMED_WIDGET osk_c_demo_app_h CHECKBUTTON 'osk_c_demo_app.h' #{((lesson_src_file["osk_c_demo_app.h"])["completed"] == true)? "CHECKED" : "UNCHECKED"}
            NAMED_WIDGET osk_c_demo_app_c CHECKBUTTON 'osk_c_demo_app.c' #{((lesson_src_file["osk_c_demo_app.c"])["completed"] == true)? "CHECKED" : "UNCHECKED"}
            LABEL '        '
      
            NAMED_WIDGET msglog_h CHECKBUTTON 'msglog.h' #{((lesson_src_file["msglog.h"])["completed"] == true)? "CHECKED" : "UNCHECKED"}
            NAMED_WIDGET msglog_c CHECKBUTTON 'msglog.c' #{((lesson_src_file["msglog.c"])["completed"] == true)? "CHECKED" : "UNCHECKED"}
            LABEL '        '

            NAMED_WIDGET msglogtbl_h CHECKBUTTON 'msglogtbl.h' #{((lesson_src_file["msglogtbl.h"])["completed"] == true)? "CHECKED" : "UNCHECKED"}
            NAMED_WIDGET msglogtbl_c CHECKBUTTON 'msglogtbl.c' #{((lesson_src_file["msglogtbl.c"])["completed"] == true)? "CHECKED" : "UNCHECKED"}
            LABEL '        '

            BUTTON 'Save Status'      'osk_c_lesson_cmd(self,\"LESSON_SAVE_STATUS\")'
            BUTTON 'Lesson Complete'  'osk_c_lesson_cmd(self,\"LESSON_COMPLETE\")'
            LABEL '        '
         "
         
         f.write (scr_text)
         
         f.write (scr_trailer)

      end # File
         
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

end # osk_c_tutor_create_scr()

