###############################################################################
# cFS Kit Create App Scr
#
# Notes:
#   1. This provides functions for the auto generated screen create_app_scr.txt
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'fileutils'
require 'json'

require 'osk_global'
require 'app_template'



JSON_CONFIG_FILE = File.join(Osk::TOOLS_DIR,'create-app',Osk::CREATE_APP_JSON_FILE)

# JSON config file labels 
JSON_VERSION           = "version"
JSON_DIR               = "dir"
JSON_DIR_SRC_TEMPLATE  = "src-template"
JSON_DIR_DST_CFS       = "dst-cfs"
JSON_DIR_DST_CFS_BUILD = "dst-cfs-build"
JSON_DIR_DST_COSMOS    = "dst-cosmos"
JSON_TEMPLATE_VAR      = "template-var"

$template_dir = {}

def create_app_scr_set_template_dir(template_dir)

   #puts "create_app_set_template_dir(#{template_dir})\n"
   $template_dir = template_dir

end

def create_app_scr_display_template_info(screen)
 
   template_sel = screen.get_named_widget("template").text

   template_info = AppTemplate.get_descr($template_dir[template_sel])

   create_app_create_template_info_screen(template_sel, template_info)

   display("CFS_KIT #{File.basename(Osk::TEMPLATE_INFO_SCR_FILE,'.txt')}",50,50)

end # create_app_display_template_info()


#
# Create an application/library based on the user template selection
#
def create_app_scr_execute(screen)
 
   # COSMOS seems to enforce some input, but verify return just in case
   app_name = ask_string("Enter app/lib name")
   return unless (!app_name.nil? or app_name != "")

   template_sel      = screen.get_named_widget("template")
   cfs_target_dir    = screen.get_named_widget("cfs_target_dir")
   cosmos_target_dir = screen.get_named_widget("cosmos_target_dir")

   config = read_config_file(JSON_CONFIG_FILE)  
   dirs = get_default_dirs(config)
   
   if (cfs_target_dir.text.nil? or cfs_target_dir.text == "")
      cfs_target_dir.text = dirs['CFS']
   end

   if (cosmos_target_dir.text.nil? or cosmos_target_dir.text == "")
      cosmos_target_dir.text = dirs['COSMOS']
   end

   begin

      app_template = AppTemplate.new(app_name, config[JSON_TEMPLATE_VAR])
      
      status = app_template.create_app($template_dir[template_sel.text], cfs_target_dir.text, cosmos_target_dir.text)

      if status 
          if app_template.include_cosmos
             app_template.update_cosmos_cmd_tlm_server
          end
          prompt "Successfully created <b>#{app_name}</b> in<br>cFS directory:<br>#{cfs_target_dir.text}<br>COSMOS directory:<br>#{cosmos_target_dir.text}<br><br>Proceed with step 2..."
      else
          prompt "Error creating #{app_name}"
      end
   
   rescue Exception => e

      prompt e.message
      #puts e.backtrace.inpsect
	   
   end

     
end # create_app_execute()

def create_app_scr_manage_dir(screen, cmd)
 

   begin
 
      if cmd.include? "SHOW_DEFAULT"

         config = read_config_file(JSON_CONFIG_FILE)  
         dirs   = get_default_dirs(config)

         if cmd.include? "CFS"
            cfs_target_dir = screen.get_named_widget("cfs_target_dir")
            cfs_target_dir.text = dirs['CFS']
         else
            cosmos_target_dir = screen.get_named_widget("cosmos_target_dir")
            cosmos_target_dir.text = dirs['COSMOS']
         end

      elsif (cmd == "BROWSE_CFS")
   
         path_filename = open_directory_dialog(File.join(Osk::OSK_CFS_DIR,'apps'))
         if (path_filename != "" and !path_filename.nil?)
            cfs_target_dir = screen.get_named_widget("cfs_target_dir")
            cfs_target_dir.text = path_filename
         end

      elsif (cmd == "BROWSE_COSMOS")
   
         path_filename = open_directory_dialog(File.join(Cosmos::USERPATH,'config','targets'))
         if (path_filename != "" and !path_filename.nil?)
            cosmos_target_dir = screen.get_named_widget("cosmos_target_dir")
            cosmos_target_dir.text = path_filename
         end
      elsif (cmd == "TODO")
         prompt(Osk::MSG_TBD_FEATURE)
      else
         raise "Error in screen definition file. Undefined command sent to create_app_manage_dir()"
      end

   rescue Exception => e

      prompt("Exception in create_app_manage_directories()\n#{e.message}\n #{e.backtrace.inpsect}")
	   
   end
     
end # create_app_manage_directories()

#
# Get the default directories from the JSON configuration file. All of the 
# directories should be defined in the config file so the config definitions
# are verified regardles of whether the user needs it.
#
def get_default_dirs(config)

   dir_config = config[JSON_DIR]

   template_dir      = File.join(Osk::TOOLS_DIR,'create-app',dir_config[JSON_DIR_SRC_TEMPLATE])
   cfs_dst_dir       = File.join(Osk::OSK_CFS_DIR,dir_config[JSON_DIR_DST_CFS])
   cfs_build_dst_dir = File.join(Osk::OSK_CFS_DIR,dir_config[JSON_DIR_DST_CFS_BUILD])
   cosmos_dst_dir    = File.join(Cosmos::USERPATH,dir_config[JSON_DIR_DST_COSMOS])
         
   raise IOError "Configuration file #{JSON_CONFIG_FILE} relative template directory resolved to a non-existant directory: #{template_dir}."  unless Dir.exist?(template_dir)
   raise IOError "Configuration file #{JSON_CONFIG_FILE} relative cFS directory resolved to a non-existant directory: #{cfs_dst_dir}."        unless Dir.exist?(cfs_dst_dir)
   raise IOError "Configuration file #{JSON_CONFIG_FILE} relative cFS directory resolved to a non-existant directory: #{cfs_build_dst_dir}."  unless Dir.exist?(cfs_build_dst_dir)
   raise IOError "Configuration file #{JSON_CONFIG_FILE} relative COSMOS directory resolved to a non-existant directory: #{cosmos_dst_dir}."  unless Dir.exist?(cosmos_dst_dir)
         
   return {'TEMPLATE' => template_dir, 'CFS' => cfs_dst_dir,  'CFS_BUILD' => cfs_build_dst_dir,  'COSMOS' => cosmos_dst_dir}

end # get_default_dirs()


#
# Read the JSON configuration file and verify the required fields
#
def read_config_file(config_file)

   raise IOError "Configuration file #{config_file} does not exist" unless File.exist?(config_file)
   config_json = File.read(config_file)
   config = JSON.parse(config_json)
      
   raise NameError "Configuration file #{config_file} missing #{JSON_TEMPLATE_VAR} definition"  unless config.key?(JSON_TEMPLATE_VAR)

   dir_config = config[JSON_DIR]

   raise NameError "Configuration file #{config_file} missing #{JSON_DIR} => #{JSON_DIR_SRC_TEMPLATE} definition"  unless dir_config.key?(JSON_DIR_SRC_TEMPLATE)
   raise NameError "Configuration file #{config_file} missing #{JSON_DIR} => #{JSON_DIR_DST_CFS} definition"       unless dir_config.key?(JSON_DIR_DST_CFS)
   raise NameError "Configuration file #{config_file} missing #{JSON_DIR} => #{JSON_DIR_DST_CFS_BUILD} definition" unless dir_config.key?(JSON_DIR_DST_CFS_BUILD)
   raise NameError "Configuration file #{config_file} missing #{JSON_DIR} => #{JSON_DIR_DST_COSMOS} definition"    unless dir_config.key?(JSON_DIR_DST_COSMOS)

   return config
      
end # read_config_file()


################################################################################
## Create Template Info Screen
################################################################################


def create_app_create_template_info_screen(template_title, template_info)

   t = Time.new 
   time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"

   template_info_scr_header = "
   ###############################################################################
   # cfs_kit Template Info Screen
   #
   # Notes:
   #   1. Do not edit this file because it is automatically generated and your
   #      changes will not be saved.
   #   2. File created by create_app_screen.rb on #{time_stamp}
   #
   # License:
   #   Written by David McComas, licensed under the copyleft GNU General Public
   #   License (GPL).
   #
   ###############################################################################

   SCREEN AUTO AUTO 0.5
   GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
  
   TITLE \"#{template_title}\"
   SETTING BACKCOLOR 162 181 205
   SETTING TEXTCOLOR black
      
   VERTICALBOX \"\" 10
   "

   template_info_scr_trailer = "
   END # Vertical Box
   "
   
   template_scr_file = File.join(Osk::CFS_KIT_SCR_DIR,Osk::TEMPLATE_INFO_SCR_FILE)

   begin
         
      # Always overwrite the temp file      
      File.open(template_scr_file,"w") do |f| 
           
         f.write (template_info_scr_header)

         f.write ("\n   LABEL \"  \"\n")
  
         info_line = 1
         info_line_str = ""
         template_info.each do |line|
            info_line_str << "   NAMED_WIDGET line_#{info_line} LABEL \"#{line}\"\n"         
            info_line_str << "   SETTING TEXTCOLOR 0 0 153\n"
            info_line += 1
         end
            
         f.write (info_line_str)
         f.write ("\n   LABEL \"  \"\n")
         f.write (template_info_scr_trailer)

      end # File
         
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

end # create_app_create_template_info_screen()


