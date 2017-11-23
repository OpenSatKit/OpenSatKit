###############################################################################
# Heater Control Screen Scripts
#
# Author: Christel Gesterling
#
# This is governed by the NASA Open Source Agreement and may be used,
# distributed and modified only pursuant to the terms of that agreement.
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

LABEL_1 = ["This demo shows how three apps work together to autonomously control ", 
           "the spacecraft's heaters. Click... ",
           "",
           "  <Enable HC> to start the demo ",
           "  <Disable HC> to stop the demo ",
           "  <Next> to read what each app does"]

LABEL_2 = ["Heater Control (HC) monitors the temperature data from the thermistors ",
           "on each heater and calculates the average temperature for each heater. ",
           "When heater control is ENABLED, it will turn the heaters ON or OFF as ",
           "appropriate to stay within optimal range (shown in blue).",
           "",
           " "]

LABEL_3 = ["Limit Checker (LC) monitors the temperature data and whether or not HC ",
           "is ENABLED.  If it is, but the temperature gets too far out of range, ",
           "LC will send a command to reset the heaters for the component whose ",
           "heaters are not responding to HC.",
           "",
           " "]

LABEL_4 = ["Stored Command (SC) holds the sequences of commands necessary to ",
           "restart the heaters, stored in a Relative Time Sequence (RTS). This is ",
           "what is activated by LC when temperatures get too far out of range. ",
           "RTS 3 is used to initiate this demo.",
           "",
           " "]
           
def heater_demo(screen, button)

  line1 = screen.get_named_widget("Label1")
  line2 = screen.get_named_widget("Label2")
  line3 = screen.get_named_widget("Label3")
  line4 = screen.get_named_widget("Label4")
  line5 = screen.get_named_widget("Label5")
  line6 = screen.get_named_widget("Label6")

  if (button == "NEXT")
    if (line1.text == LABEL_1[0])
      display("CFS_KIT AUTONOMY_MGMT_SCREEN",50,50)
      line1.text = LABEL_2[0]
      line2.text = LABEL_2[1]
      line3.text = LABEL_2[2]
      line4.text = LABEL_2[3]
      line5.text = LABEL_2[4]
      line6.text = LABEL_2[5]
    elsif (line1.text == LABEL_2[0])
      line1.text = LABEL_3[0]
      line2.text = LABEL_3[1]
      line3.text = LABEL_3[2]
      line4.text = LABEL_3[3]
      line5.text = LABEL_3[4]
      line6.text = LABEL_3[5]
    elsif (line1.text == LABEL_3[0])
      line1.text = LABEL_4[0]
      line2.text = LABEL_4[1]
      line3.text = LABEL_4[2]
      line4.text = LABEL_4[3]
      line5.text = LABEL_4[4]
      line6.text = LABEL_4[5]
    end
  elsif (button == "BACK")
    if (line1.text == LABEL_2[0])
      line1.text = LABEL_1[0]
      line2.text = LABEL_1[1]
      line3.text = LABEL_1[2]
      line4.text = LABEL_1[3]
      line5.text = LABEL_1[4]
      line6.text = LABEL_1[5]
    elsif (line1.text == LABEL_3[0])
      line1.text = LABEL_2[0]
      line2.text = LABEL_2[1]
      line3.text = LABEL_2[2]
      line4.text = LABEL_2[3]
      line5.text = LABEL_2[4]
      line6.text = LABEL_2[5]
    elsif (line1.text == LABEL_4[0])
      line1.text = LABEL_3[0]
      line2.text = LABEL_3[1]
      line3.text = LABEL_3[2]
      line4.text = LABEL_3[3]
      line5.text = LABEL_3[4]
      line6.text = LABEL_3[5]
    end
  elsif (button == "ENABLE")
    cmd("CFE_EVS ENA_APP_EVENT_TYPE with APPNAME LC, BITMASK 0x01") # Enable debug events
    cmd("CFE_EVS ENA_APP_EVENT_TYPE with APPNAME SC, BITMASK 0x01") # Enable debug events
    cmd("HC HEATER_CONTROL with HCTYPE BATTERY, HCSTATUS ENABLED")
    cmd("HC HEATER_CONTROL with HCTYPE PROPULSION, HCSTATUS ENABLED")
    cmd("SC ENABLE_RTS with RTS_ID 3")
    cmd("SC START_RTS with RTS_ID 3")
  elsif (button == "DISABLE")
    cmd("CFE_EVS DIS_APP_EVENT_TYPE with APPNAME LC, BITMASK 0x01") # Disable debug events
    cmd("CFE_EVS DIS_APP_EVENT_TYPE with APPNAME SC, BITMASK 0x01") # Disable debug events
    cmd("HC HEATER_CONTROL with HCTYPE BATTERY, HCSTATUS DISABLED")
    cmd("HC HEATER_CONTROL with HCTYPE PROPULSION, HCSTATUS DISABLED")
    #cmd("SC DISABLE_RTS with RTS_ID 3")
  end
end # heater_demo()