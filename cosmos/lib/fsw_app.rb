###############################################################################
# Defines a base class for defining a FSW application within OSK. It includes
# both ground and FSW information.
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

class FswApp

   attr_reader :fsw_name          # FSW app name also used in ground references
   attr_reader :target, :hk_pkt   # COSMOS definitions
   attr_reader :cmd_mid
   attr_reader :cmd_valid         # Status of last command sent
   
   @@validate_cmd = false
      
   #
   # fsw_name - Name used in cfe_es_startup.screen
   # target   - COSMOS target name
   # hk_pkt   - Housekeeping packet name defined in COSMOS target. Should be standard osk_global name
   # cmd_mid  - Command message identifier
   #
   def initialize(fsw_name,target,hk_pkt,cmd_mid)
   
      @fsw_name = fsw_name
      @target   = target
      @hk_pkt   = hk_pkt
      @cmd_mid  = cmd_mid
      
      @target_hk_str = "#{@target} #{@hk_pkt}"
      
   end

   def self.validate_cmd(boolean)
   
      @@validate_cmd = boolean
   
   end # validate_cmd()
   
   # cmd_str - Contains the command name followed by optional command parameters 
   def send_cmd(cmd_str)
  
      cmd_valid = false
	  
      cmd_valid_cnt = tlm("#{@target_hk_str} #{Osk::TLM_STR_CMD_VLD}")
      cmd_error_cnt = tlm("#{@target_hk_str} #{Osk::TLM_STR_CMD_ERR}")
      seq_cnt = tlm("#{@target_hk_str} #{Ccsds::PRI_HDR_SEQUENCE}")
      ##puts cmd_valid_cnt
      cmd("#{@target} #{cmd_str}")
      
      # The logic below doesn't work for the app reset cmd. For now ignore the check if it's a reset cmd
      if (@@validate_cmd and !(cmd_str.include? Osk::CMD_STR_RESET))
         
         wait("#{@target_hk_str} #{Osk::TLM_STR_CMD_VLD} == #{cmd_valid_cnt}+1", 7)  # Delay until updated cmd valid count or timeout. 
	
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