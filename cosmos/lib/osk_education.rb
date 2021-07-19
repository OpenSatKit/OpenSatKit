###############################################################################
# OpenSatKit Education Manager 
#
# Notes:
#   1. Manges the education tools
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
################################################################################

require 'osk_global'
require 'osk_system'


module Osk

   ###########################
   ## Tutorial Format Types ## 
   ###########################

   TUTORIAL_HTML   = "HTML"
   TUTORIAL_PDF    = "PDF"
   TUTORIAL_SCRIPT = "SCRIPT"

   ################################################################################
   ## Launch Tutorial
   ################################################################################

   def self.education_tutorial(format, tutor_group, lesson)

      tutor_file = "#{Osk::TUTORIAL_DIR}/#{tutor_group}/#{lesson}"
      
      begin
         case format
         when Osk::TUTORIAL_HTML
            tutor_file = tutor_file + ".html"
            raise "Tutorial file '#{tutor_file}' does not exist" unless File.exists? tutor_file 
            Cosmos.open_in_web_browser(tutor_file)      
         when Osk::TUTORIAL_PDF
            tutor_file = tutor_file + ".pdf"
            raise "Tutorial file '#{tutor_file}' does not exist" unless File.exists? tutor_file 
            Osk::System.display_pdf(tutor_file)
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

   end # education_tutorial()

   def self.education_video(video_link)
      
      Cosmos.open_in_web_browser(video_link)

   end # education_video()
   
end # Module Osk
