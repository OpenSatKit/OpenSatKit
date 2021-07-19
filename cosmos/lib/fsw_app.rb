###############################################################################
# Defines a base class for defining a FSW application within OSK. It includes
# both ground and FSW information.
#
# Notes:
#   1. This class supports both cFS and OSK applications. Tables are the main
#      architectural difference. If the need arises this will be refactored to
#      use subclasses
#   2. Refer to osk_global.rb for JSON definitions that are used to initialize
#      most of the attributes
#   3. Eventually an OSK toolchain will manage processing app definition files
#      and generating flight/ground artifacts. This is an imperfect stop gap
#      measure that has duplication of definitions.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'ccsds'
require 'osk_global'
require 'osk_system'
require 'osk_ops'


class Table

   attr_reader :name
   attr_reader :id        # Unused by cFS apps
   attr_reader :filename

   def initialize(name,id,filename)
     
      @name     = name
      @id       = id
      @filename = filename
   
   end
   
end # Class Table

class FswApp
   
   @@validate_cmd     = false     # Is command counter increment validated after command sent?
   @@validate_timeout = 7         # Timeout(secs) to wait for telemetry verification

   ### Overview ###
   attr_reader :title             # Brief app title, not the acronym
   attr_reader :version           # Release version
   attr_reader :owner             # Organization that controls/releases the app
   attr_reader :url               # URL to access the app
   attr_reader :description       # Brief description of application's purpose 

   ### Targets ###
   attr_reader :cfs_targets

   ### cFS/FSW ###
   attr_reader :app_framework     # Application framework: 'cfs', 'osk_c_fw' or 'osk_cpp_fw'
   attr_reader :cfe_type          # cFE supports apps and libraries: 'app' or 'lib' These should be apps
   attr_reader :obj_path_filename # Path/Filename of object loaded during FSW startup
   attr_reader :entry_symbol
   attr_reader :fsw_name          # FSW app name also used in ground references
   attr_reader :priority
   attr_reader :stack_size        # Size in bytes
   attr_reader :exception_action  # 0 = Reset app, Non-zero = reset processor
   attr_reader :tables            # 
   
   ### COSMOS/Ops ###
   attr_reader :target, :hk_pkt   # COSMOS definitions
   attr_reader :cmd_mid
   attr_reader :cmd_valid         # Status of last command sent
   attr_reader :target_hk_str     # Convenient string that combines target and HK packet name

   #
   # fsw_name - Name used in cfe_es_startup.screen
   # target   - COSMOS target name
   # hk_pkt   - Housekeeping packet name defined in COSMOS target. Should be standard osk_global name
   # cmd_mid  - Command message identifier
   # app_json - This is a temporary mod during transition to using JSON files to create apps. Eventually
   #            the entire app will be defined by a JSON. The transition problem is that a lot of scripts
   #            call an app directly
   #
   def initialize(fsw_name,target,hk_pkt,cmd_mid,app_json=nil)
   
      @fsw_name = fsw_name
      @target   = target
      @hk_pkt   = hk_pkt
      @cmd_mid  = cmd_mid
      
      # JSON definitions will override if defined
      @cfs_targets = []
      @description = []
      @tables = []
      @app_framework     = nil 
      @cfe_type          = nil
      @obj_path_filename = nil
      @entry_symbol      = nil
      @priority          = nil
      @stack_size        = nil
      @exception_action  = nil
            
      @target_hk_str = "#{@target} #{@hk_pkt}"
      
      #~puts "FswApp: #{fsw_name}, #{@fsw_name}\n"
      begin
         if (not app_json.nil?)
         
            @title   = app_json["app"]["title"]
            @version = app_json["app"]["version"]
            @owner   = app_json["app"]["owner"]
            @url     = app_json["app"]["url"]
            
            @description = app_json["app"]["description"]

            sys = app_json["app"]["system"]
            
            app = app_json["app"]["fsw"]
            @app_framework     = app["app-framework"] 
            @cfe_type          = app["cfe-type"]
            @obj_path_filename = app["obj-path-file"]
            @entry_symbol      = app["entry-symbol"]
            @fsw_name          = app["name"]
            @priority          = app["priority"]
            @stack_size        = app["stack"]
            @exception_action  = app["exception-action"]
            @tables = []
            app["tables"].each do |tbl|
               ##puts tbl["name"]
               @tables << Table.new(tbl["name"],tbl["id"],tbl["filename"])
            end
            ##puts "Tables length = " + tables.length.to_s + "\n"
            @cmd_mid = Fsw::MsgId.get_val(app["cmd-mid"])
         
         end # If JSON
      rescue Exception => e
         puts e.message
         puts e.backtrace.inspect  
      end
      
   end

   def init_with_hash(fsw_name,target,hk_pkt,cmd_mid)
   
      @fsw_name = fsw_name
      @target   = target
      @hk_pkt   = hk_pkt
      @cmd_mid  = cmd_mid
      
      @target_hk_str = "#{@target} #{@hk_pkt}"
     
   end

   # This should only be used by the cFE apps since they're JSON definitions are managed 
   # differently than user apps
   def set_version(version)
   
      @version = version
      
   end
   
   # Add a cFS target to the app's target list
   def add_cfs_target(target)
   
      @cfs_targets << target
      
   end

   # Check if receiving telemetry packet without stopping the script, i.e. don't
   # use COSMOS wait/check methods.
   def tlm_active?(tlm_pkt: Osk::TLM_STR_HK_PKT, pkt_rate: 3)
   
      target_tlm_pkt_str = "#{@target} #{tlm_pkt}"
      
      seq_cnt1 = tlm("#{target_tlm_pkt_str} #{Ccsds::PRI_HDR_SEQUENCE}")
      wait pkt_rate + 2
      seq_cnt2 = tlm("#{target_tlm_pkt_str} #{Ccsds::PRI_HDR_SEQUENCE}")
      
      return_value = (seq_cnt1 != seq_cnt2)
            
   end # tlm_active?

   def self.validate_cmd(boolean)
   
      @@validate_cmd = boolean
   
   end # validate_cmd()

   def self.validate_cmd?
   
      return @@validate_cmd
   
   end # validate_cmd?()
   
   # cmd_str - Contains the command name followed by optional command parameters 
   def send_cmd(cmd_str, override_validate=Osk::OVERRIDE_OFF)
  
      cmd_valid = false
	  
      cmd_valid_cnt = tlm("#{@target_hk_str} #{Osk::TLM_STR_CMD_VLD}")
      cmd_error_cnt = tlm("#{@target_hk_str} #{Osk::TLM_STR_CMD_ERR}")
      seq_cnt = tlm("#{@target_hk_str} #{Ccsds::PRI_HDR_SEQUENCE}")
      ##puts "FswApp.send_cmd #{@target} before cmd(): cmd_valid_cnt=#{cmd_valid_cnt}\n"
      cmd("#{@target} #{cmd_str}")
      
      # The logic below doesn't work for the app reset cmd. For now ignore the check if it's a reset cmd
      validate_cmd_cnt = @@validate_cmd
      case override_validate
      when Osk::OVERRIDE_OFF
         validate_cmd_cnt = @@validate_cmd
      when Osk::OVERRIDE_TO_TRUE
         validate_cmd_cnt = true
      when Osk::OVERRIDE_TO_FALSE
         validate_cmd_cnt = false
      else
         Osk::Ops::raise_exception "Invalid app command count validate override option #{override_opt}. Must be a vlaue in #{Osk::OVERRIDE_OPT}"
      end
      
      if (validate_cmd_cnt and !(cmd_str.include? Osk::CMD_STR_RESET))
         
         wait("#{@target_hk_str} #{Osk::TLM_STR_CMD_VLD} == #{cmd_valid_cnt}+1", @@validate_timeout)  # Delay until updated cmd valid count or timeout 
	
         ##puts tlm("#{@target_hk_str} CMD_VALID_COUNT")
         if ( (tlm("#{@target_hk_str} #{Osk::TLM_STR_CMD_VLD}") == (cmd_valid_cnt + 1)) && 
              (tlm("#{@target_hk_str} #{Osk::TLM_STR_CMD_ERR}") ==  cmd_error_cnt))
            cmd_valid = true
         else
            # Command may have executed but since it couldn't be verified, keep cmd_valid false
            if ( tlm("#{@target_hk_str} #{Ccsds::PRI_HDR_SEQUENCE}") == seq_cnt)
               Osk::Ops::raise_exception "Command execution not verified. #{@fsw_name} #{@hk_pkt} not received"
            else    
               Osk::Ops::raise_exception "Command execution failed for #{@fsw_name} #{cmd_str}"
            end
         end
	   else
         cmd_valid = true
      end # validate_cmd
      
      return cmd_valid
    
   end # send_cmd()
   
end # Class FswApp