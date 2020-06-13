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
         @cfe_es   = FswApp.new("CFE_ES","CFE_ES",   Osk::TLM_STR_HK_PKT, Fsw::MsgId::CFE_ES_CMD_MID)
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

         # cFS Apps
         #~@cs = FswApp.new("CS","CS",Osk::TLM_STR_HK_PKT,Fsw::MsgId::CS_CMD_MID)
         #~@ds = FswApp.new("DS","DS",Osk::TLM_STR_HK_PKT,Fsw::MsgId::DS_CMD_MID)
         #~@fm = FswApp.new("FM","FM",Osk::TLM_STR_HK_PKT,Fsw::MsgId::FM_CMD_MID)
         #~@hk = FswApp.new("HK","HK",Osk::TLM_STR_HK_PKT,Fsw::MsgId::HK_CMD_MID)
         #~@hs = FswApp.new("HS","HS",Osk::TLM_STR_HK_PKT,Fsw::MsgId::HS_CMD_MID)
         #~@lc = FswApp.new("LC","LC",Osk::TLM_STR_HK_PKT,Fsw::MsgId::LC_CMD_MID)
         #~@md = FswApp.new("MD","MD",Osk::TLM_STR_HK_PKT,Fsw::MsgId::MD_CMD_MID)
         #~@mm = FswApp.new("MM","MM",Osk::TLM_STR_HK_PKT,Fsw::MsgId::MM_CMD_MID)
         #~@sc = FswApp.new("SC","SC",Osk::TLM_STR_HK_PKT,Fsw::MsgId::SC_CMD_MID)
      
         #~@app["CS"] = @cs
         #~@app["DS"] = @ds
         #~@app["FM"] = @fm
         #~@app["HK"] = @hk
         #~@app["HS"] = @hs
         #~@app["LC"] = @lc
         #~@app["MD"] = @md
         #~@app["MM"] = @mm
         #~@app["SC"] = @sc


         osk = Osk::read_target_json("CFS_KIT")
         osk["targets"].each do |target|
         
            app_name = target.keys[0].to_s.upcase
            config   = target.values[0]["load-on-init"]

            app_json  = Osk::read_target_json(app_name)
            @app[app_name] = FswApp.new(app_name, app_name,  Osk::TLM_STR_HK_PKT, Fsw::MsgId::UNUSED_MID, app_json)

         end

         # Kit Apps
         #~@f42  = FswApp.new("F42",  "F42",  Osk::TLM_STR_HK_PKT, Fsw::MsgId::F42_CMD_MID)
         #~@i42  = FswApp.new("I42",  "I42",  Osk::TLM_STR_HK_PKT, Fsw::MsgId::I42_CMD_MID)
         #~@isim = FswApp.new("ISIM", "ISIM", Osk::TLM_STR_HK_PKT, Fsw::MsgId::ISIM_CMD_MID)
         #~@tftp = FswApp.new("TFTP", "TFTP", Osk::TLM_STR_HK_PKT, Fsw::MsgId::TFTP_CMD_MID)
        
         #~app_json = Osk::System.read_target_json("BM")
         #~@bm      = FswApp.new("BM",  "BM",  Osk::TLM_STR_HK_PKT, Fsw::MsgId::BM_CMD_MID, app_json)

         #~app_json = Osk::System.read_target_json("HC")
         #~@hc      = FswApp.new("HC",  "HC",  Osk::TLM_STR_HK_PKT, Fsw::MsgId::HC_CMD_MID, app_json)

         #~app_json = Osk::System.read_target_json("HSIM")
         #~@hsim    = FswApp.new("HSIM",  "HSIM",  Osk::TLM_STR_HK_PKT, Fsw::MsgId::HSIM_CMD_MID, app_json)
         
         #~#app_json = read_app_json("kit_ci.json")
         #~app_json = Osk::System.read_target_json("KIT_CI")
         #~@kit_ci  = FswApp.new("KIT_CI",  "KIT_CI",  Osk::TLM_STR_HK_PKT, Fsw::MsgId::KIT_CI_CMD_MID, app_json)

         #app_json = read_app_json("kit_sch.json")
         #~app_json = Osk::System.read_target_json("KIT_SCH")
         #~@kit_sch = FswApp.new("KIT_SCH", "KIT_SCH", Osk::TLM_STR_HK_PKT, Fsw::MsgId::KIT_SCH_CMD_MID, app_json)

         #app_json = read_app_json("kit_to.json")
         #~app_json = Osk::System.read_target_json("KIT_TO")
         #~@kit_to  = FswApp.new("KIT_TO",  "KIT_TO",  Osk::TLM_STR_HK_PKT, Fsw::MsgId::KIT_TO_CMD_MID, app_json)

         #~app_json  = Osk::System.read_target_json("OSK_DEMO")
         #~@osk_demo = FswApp.new("OSK_DEMO",  "OSK_DEMO",  Osk::TLM_STR_HK_PKT, Fsw::MsgId::OSK_DEMO_CMD_MID, app_json)

         #~@app["BM"]       = @bm
         #~@app["F42"]      = @f42
         #~@app["HC"]       = @hc
         #~@app["HSIM"]     = @hsim
         #~@app["I42"]      = @i42
         #~@app["ISIM"]     = @isim
         #~@app["KIT_CI"]   = @kit_ci
         #~@app["KIT_SCH"]  = @kit_sch
         #~@app["KIT_TO"]   = @kit_to
         #~@app["OSK_DEMO"] = @osk_demo
         #~@app["TFTP"]     = @tftp

         
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

