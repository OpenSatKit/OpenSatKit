###############################################################################
# cFE Event Service Configure Event Message Screen
#
# Notes:
#   None 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_ops'

################################################################################
## GUI Send Commands
################################################################################

def cfg_app_event_cmd(screen, cmd)

   case cmd   
   when "CONFIG"
   
      debug    = 0b0001
      info     = 0b0010
      error    = 0b0100
      critical = 0b1000
      
      bit_mask = 0
      if get_named_widget("cfg_debug").checked?    then bit_mask = bit_mask | debug    end
      if get_named_widget("cfg_info").checked?     then bit_mask = bit_mask | info     end
      if get_named_widget("cfg_error").checked?    then bit_mask = bit_mask | error    end
      if get_named_widget("cfg_critical").checked? then bit_mask = bit_mask | critical end

      cmd_params = "APP_NAME #{get_named_widget("app_name").text}, BITMASK #{bit_mask}"

      if get_named_widget("ena_dis").text == "Enable" then
         Osk::flight.cfe_evs.send_cmd("ENA_APP_EVENT_TYPE with #{cmd_params}")
      else
         Osk::flight.cfe_evs.send_cmd("DIS_APP_EVENT_TYPE with #{cmd_params}")
      end       
     
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_evs_config_cmd()"
   end
   clear ("CFE_EVS CFG_APP_EVENT_SCREEN")
end # cfe_evs_config_cmd()
