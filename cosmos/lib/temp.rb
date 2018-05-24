require 'osk_global'

TUTORIAL_SCR_HEADER = "
###############################################################################
# OSK Tutorial Screen
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

SCREEN AUTO AUTO 0.5
GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
  
TITLE \"Tutorials\"
SETTING BACKCOLOR 162 181 205
SETTING TEXTCOLOR black
      
VERTICALBOX \"\" 10
"

TUTORIAL_SCR_TRAILER = "
END # Vertical Box
"

  
   tutorial_def_file = "#{Osk::TUTORIAL_DIR}/#{Osk::TUTORIAL_DEF_FILE}"
   tutorial_scr_file = "#{Osk::SCR_DIR}/#{Osk::TUTORIAL_SCR_FILE}"
   
   puts "#{tutorial_scr_file}"
   
   t = Time.new 
   file_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"
   puts file_stamp
   
   begin
      
      json_file = File.read(tutorial_def_file)
      json_hash = JSON.parse(json_file)
    
      #puts json_hash
      #puts json_hash["tutorials"]
      #puts json_hash["tutorials"][0]["name"]
      
      if File.exists? tutorial_scr_file
         puts "Renaming #{tutorial_scr_file}"
         filename = File.basename(tutorial_scr_file, File.extname(tutorial_scr_file))
         new_filename =  "#{Osk::SCR_DIR}/#{filename}#{file_stamp}"+File.extname(tutorial_scr_file)
         puts "#{new_filename}" 
         File.rename(tutorial_scr_file, new_filename)
      end
      
      File.open("#{tutorial_scr_file}","w") do |f| 
	     
         f.write ("#{Osk::TUTORIAL_SCR_HEADER}")
         
         json_hash["tutorials"].each do |tutorial|
            
            lesson_str = "#{tutorial["lessons"]}"
            lesson_str = lesson_str[1,(lesson_str.length-2)]  # Remove brackets [] and keep quotes around elements
            
            tutorial_menu = "
               HORIZONTAL 10
               BUTTON '#{tutorial["button"]}' 'require \"#{Cosmos::USERPATH}/config/targets/CFS_KIT/lib/tutorial_screen.rb\"; tutorial = combo_box(\"#{tutorial["user-prompt"]}\",#{lesson_str}); launch_tutorial(self, \"#{tutorial["directory"]}\", tutorial)'
               SPACER 20 0 MAXIMUM FIXED          
               LABEL \"#{tutorial["description"]}\"
               SPACER 50 0 MINIMUMEXPANDING FIXED          
               END # Horizontal"
            
            f.write (tutorial_menu)
         
         end # Tutorial
         
         f.write ("#{Osk::TUTORIAL_SCR_TRAILER}")

      end # File
      
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

