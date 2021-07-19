###############################################################################
# OSK Flight Module
#
# Notes:
#   1. The OSK module defines the namespace that contains constants, methods,
#      and classes used to created the OSK. This flight module is limited to
#      just the Flight software. If a function is required that requires
#      multiple OSK components such as transferring a file between the ground
#      and flight then it will be defined in another file within the OSK
#      module.
#   2. Transitioning from hardcoded app initialization to local JSON defined
#      apps. Eventually would like to use remote repo.  
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'cosmos'

require 'fsw_msg_id'
require 'fsw_app'
require 'osk_global'
require 'osk_targets'

module Osk
   
   #
   # Constants that characterize the flight configuration. They 
   # are not FSW configuration parameters
   #
   CFE_EVS_APP_ID  = 0
   CFE_SB_APP_ID   = 1
   CFE_ES_APP_ID   = 2
   CFE_TIME_APP_ID = 3
   CFE_TBL_APP_ID  = 4
   
   #
   # This class serves as a container for all of the FSW apps.
   #
   # OSK is evolving/refactoring from hardcoded apps to a modular
   # architecture where apps can be dynamically added/removed
   #
   class Flight
   
      @@instance = nil
      
      # Hash containing FswApps
      attr_reader :targets
      
      # Hash containing FswApps
      attr_reader :app
      
      # cFE Apps
      attr_reader :cfe_es
      attr_reader :cfe_evs
      attr_reader :cfe_sb
      attr_reader :cfe_tbl
      attr_reader :cfe_time
      
      def self.instance
         @@instance
      end
    
      def initialize
         raise "Osk::Flight created twice" unless @@instance.nil?
         @@instance = self
         create_apps()
         Osk::create_json_table_mgmt_scr(@app)
      end # End initialize
      
      def create_apps
 
         
         # cFE Apps      
         @cfe_es   = FswApp.new("CFE_ES",  "CFE_ES",   Osk::TLM_STR_HK_PKT, Fsw::MsgId::CFE_ES_CMD_MID)
         @cfe_evs  = FswApp.new("CFE_EVS", "CFE_EVS",  Osk::TLM_STR_HK_PKT, Fsw::MsgId::CFE_EVS_CMD_MID)
         @cfe_sb   = FswApp.new("CFE_SB",  "CFE_SB",   Osk::TLM_STR_HK_PKT, Fsw::MsgId::CFE_SB_CMD_MID)
         @cfe_tbl  = FswApp.new("CFE_TBL", "CFE_TBL",  Osk::TLM_STR_HK_PKT, Fsw::MsgId::CFE_TBL_CMD_MID)
         @cfe_time = FswApp.new("CFE_TIME","CFE_TIME", Osk::TLM_STR_HK_PKT, Fsw::MsgId::CFE_TIME_CMD_MID)
         
         @app = Hash.new
         @app["CFE_ES"]   = @cfe_es
         @app["CFE_EVS"]  = @cfe_evs
         @app["CFE_SB"]   = @cfe_sb
         @app["CFE_TBL"]  = @cfe_tbl
         @app["CFE_TIME"] = @cfe_time
         
         osk = Osk::read_target_json("CFS_KIT")
         
         @targets = osk["cfs-targets"]
         @targets.each do |cfs_target|
            target = Osk::read_target_json(cfs_target)
            apps = target["apps"]
            apps.each do |app|
               @app["CFE_ES"].add_cfs_target(cfs_target)
               @app["CFE_EVS"].add_cfs_target(cfs_target)
               @app["CFE_SB"].add_cfs_target(cfs_target)
               @app["CFE_TBL"].add_cfs_target(cfs_target)
               @app["CFE_TIME"].add_cfs_target(cfs_target)
               app_name = app.upcase
               if @app.has_key?(app_name)
                  @app[app_name].add_cfs_target(cfs_target)
               else
                  app_json  = Osk::read_target_json(app_name)
                  @app[app_name] = FswApp.new(app_name, app_name, Osk::TLM_STR_HK_PKT, Fsw::MsgId::UNUSED_MID, app_json)
                  @app[app_name].add_cfs_target(cfs_target)
               end     
            end # apps
         end # cfs-targets

         cfe_ver = osk["cfe"]["version"]
         @cfe_es.set_version(cfe_ver)
         @cfe_evs.set_version(cfe_ver)
         @cfe_sb.set_version(cfe_ver)
         @cfe_tbl.set_version(cfe_ver)
         @cfe_time.set_version(cfe_ver)

         
      end # End create_apps()
             
      # cmd_str           - Contains the command name followed by optional command parameters
      # override_validate - Allows caller to override the global command counter validate check
      def send_cmd(target, cmd_str, override_validate=Osk::OVERRIDE_OFF)

         cmd_valid = false
         
         if @app.has_key?(target)
            cmd_valid = @app[target].send_cmd(cmd_str, override_validate)
         else
            cfs_kit_def_file = File.join(Osk::COSMOS_CFG_TARGET_DIR, 'CFS_KIT','osk','cfs_kit.json') #~FIXME - This duplicates code in osk_targets
            Osk::Ops::raise_exception "Undefined application #{target}. Verify COSMOS target defined and target defined in #{cfs_kit_def_file}"
         end
         
         return cmd_valid
       
      end # send_cmd()
          
   end # Flight class
   
   # An instance of flight is created outside of the module
   class << self; attr_accessor :flight; end

end # Module Osk

