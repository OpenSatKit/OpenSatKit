################################################################################
# cFS Kit Recorder Management Demo (RMD)
#
# Notes:
#   1. RMD_INSTRUCT_x variables are used to put text on the demo screen's
#      instruction text box. The RMD_INFO_x variables are used for extra detailed
#      information that is displayed on a separate screen.
#   2. Debug events are enabled for the apps used during the demo.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
# 
################################################################################

require 'cfs_kit_global'
require 'file_transfer'

################################################################################
## Global Variables
################################################################################


RMD_INFO_DEF = "\n\nNo additional information for this demo step."

################################################################################
## Demo Narrative Text
################################################################################

#
# 1 - 
# 2 - 
# 3 - 
# 4 - 
#

# Instruct 0 text is in the file demo screen text file.  It's here to help with the info text.
RMD_INSTRUCT_0 = ["This demo illustrates some basic recorder management features. Click...",
                  " ",
                  " ",
                  "  <More Info> to obtain more information about the current step",
                  "  <Demo> to issue commands to demonstrate a feature in the current step",
                  "  <Next> to move to the next step",
                  " "]
              
RMD_INFO_0 = RMD_INFO_DEF

RMD_INSTRUCT_1 = ["TBD",
                  " ",
                  " ",
                  " ",
                  "<Demo> TBD",
                  " ",
                  ""]

RMD_INFO_1 = "\n\n\
TBD\n\n\
"


# 2 - Enable/disable app events
RMD_INSTRUCT_2 = ["TBD",
                  " ",
                  " ",
                  " ",
                  "<Demo> TBD",
                  " ",
                  ""]
               
RMD_INFO_2 = RMD_INFO_DEF

# 3 - Stop an app
RMD_INSTRUCT_3 = ["TBD",
                  "",
                  "",
                  "",
                  "<Demo> TBD",
                  "",
                  ""]
RMD_INFO_3 = RMD_INFO_DEF

                    
# 4 - Start an app
RMD_INSTRUCT_4 = ["TBD",
                  "",
                  "",
                  "",
                  "<Demo> TBD",
                  "",
                  ""]
RMD_INFO_4 = RMD_INFO_DEF


RMD_INSTRUCT_ARRAY = [RMD_INSTRUCT_0, RMD_INSTRUCT_1, RMD_INSTRUCT_2, RMD_INSTRUCT_3, RMD_INSTRUCT_4]
RMD_LAST_STEP = 4

def rmd_set_instruct_text(num)

  new_instruct = RMD_INSTRUCT_ARRAY[num]
  
  $instruct1.text = new_instruct[0]
  $instruct2.text = new_instruct[1]
  $instruct3.text = new_instruct[2]
  $instruct4.text = new_instruct[3]
  $instruct5.text = new_instruct[4]
  $instruct6.text = new_instruct[5]
  $instruct7.text = new_instruct[6]

end # rmd_set_instruct_text()

################################################################################
## Demo Flow Control
################################################################################

$file_xfer = FileTransfer.new()
$rmd_step = 0   # Demo step incremented by Next button
$rmd_demo = 0   # Issue command(s) to perform the current demo step

def recorder_mgmt_demo(screen, button)

  $instruct1  = screen.get_named_widget("Instruct1")
  $instruct2  = screen.get_named_widget("Instruct2")
  $instruct3  = screen.get_named_widget("Instruct3")
  $instruct4  = screen.get_named_widget("Instruct4")
  $instruct5  = screen.get_named_widget("Instruct5")
  $instruct6  = screen.get_named_widget("Instruct6")
  $instruct7  = screen.get_named_widget("Instruct7")

  if (button == "INFO")
  
    display("CFS_KIT RECORDER_MGMT_DEMO_INFO_SCREEN",500,50)    

  elsif (button == "NEXT")
  
    $rmd_step += 1
    $rmd_demo  = 0
    
    if ($rmd_step <= RMD_LAST_STEP)
      rmd_set_instruct_text($rmd_step)
    end

    case $rmd_step
      when 1
        display("CFS_KIT RECORDER_MGMT_SCREEN",500,50)    
        cmd("CFE_EVS ENA_APP_EVENT_TYPE with APPNAME DS, BITMASK 0x01") # Enable debug events
      when 2..RMD_LAST_STEP
        # Keep case statement for maintenance
      else
        cmd("CFE_EVS DIS_APP_EVENT_TYPE with APPNAME DS, BITMASK 0x01") # Disable debug events
        $rmd_step = 0
        clear("CFS_KIT RECORDER_MGMT_SCREEN")    
        clear("CFS_KIT RECORDER_MGMT_DEMO_SCREEN")
        clear("CFS_KIT RECORDER_MGMT_DEMO_INFO_SCREEN")
    end # Step Case
        
  elsif (button == "DEMO")
  
    case $rmd_step

      # 1- 
      when 1
        if ($rmd_demo == 0)
          # Don't increment rmd_demo; okay if user repeats cmd
        end
      
      # 2 - 
      when 2
        case $rmd_demo
          when 0 
            $rmd_demo += 1
          when 1 
            $rmd_demo += 1
          when 2 
            $rmd_demo += 1
          when 3
            $rmd_demo += 1
          else
            $rmd_demo = 0
          end # Case $rmd_demo

      # 3 - 
      when 3
        if ($rmd_demo == 0)
          $rmd_demo += 1
        elsif ($rmd_demo == 1)
          $rmd_demo += 1
        elsif ($rmd_demo == 2)
          # Don't increment rmd_demo; okay if user repeats cmd
        end

      # 4 - 
      when 4
        if ($rmd_demo == 0)
         $rmd_demo += 1
        elsif ($rmd_demo == 1)
          # Don't increment rmd_demo; okay to repeat last command
        end

    end # Step Case
  end # Demo
   
end # recorder_mgmt_demo()

def recorder_mgmt_demo_more_info(screen)
   
  msg_widget = screen.get_named_widget("msg_box")
  case $rmd_step
    when 0
      msg_widget.text = RMD_INFO_0
    when 1
      msg_widget.text = RMD_INFO_1
    when 2
      msg_widget.text = RMD_INFO_2
    when 3
      msg_widget.text = RMD_INFO_3
    when 4
      msg_widget.text = RMD_INFO_4
    else
      msg_widget.text = RMD_INFO_DEF
    end # Case
    
end # recorder_mgmt_demo_more_info()

