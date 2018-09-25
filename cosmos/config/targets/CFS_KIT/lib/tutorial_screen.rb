###############################################################################
# OpenSatKit Tutorials 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
################################################################################

require 'osk_global'

################################################################################
## Launch Tutorial
################################################################################

def launch_tutorial(screen, tutorial, format, lesson)

   tutor_file = "#{Osk::TUTORIAL_DIR}/#{tutorial}/#{lesson}"
   
   begin
      case format
      when Osk::TUTORIAL_HTML
         tutor_file = tutor_file + ".html"
         raise "Tutorial file '#{tutor_file}' does not exist" unless File.exists? tutor_file 
         Cosmos.open_in_web_browser("#{tutor_file}")      
      when Osk::TUTORIAL_PDF
         tutor_file = tutor_file + ".pdf"
         raise "Tutorial file '#{tutor_file}' does not exist" unless File.exists? tutor_file 
         spawn("evince #{tutor_file}")
      when Osk::TUTORIAL_SCRIPT
         tutor_file = tutor_file + ".rb"
         raise "Tutorial file '#{tutor_file}' does not exist" unless File.exists? tutor_file 
         spawn("ruby #{Osk::COSMOS_SCR_RUNNER} -r '#{tutor_file}'")
      else
         prompt("Invalid tutorial file format '#{format}' specified")
      end   
   
   rescue StandardError => e  
      prompt(e.message)
   end

end # launch_tutorial()

