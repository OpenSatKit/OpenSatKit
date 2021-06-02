###############################################################################
# Create Application
#
# Notes:
#   1. The order of template_var's case-sensitive definitions are assumed
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'fileutils'
require 'json'

require 'osk_global'


#
# Process application template files
# - Standard JSON file name used that contains meta-data fro processing template files
#

class AppTemplate


   TEMPLATE_COMMENT_START  = "/*##"
   TEMPLATE_COMMENT_END    = "##*/"
   TEMPLATE_KEYWORD_START  = "Start"
   TEMPLATE_KEYWORD_END    = "End"

   JSON_TITLE            = "title"
   JSON_VERSION          = "version"
   JSON_COSMOS           = "cosmos"
   JSON_DESCRIPTION      = "description"
   JSON_CFS_PREFIX       = "cfsapp-"
   JSON_TABLES_PREFIX    = "tables-"
   JSON_COSMOS_PREFIX    = "cosmos-"
   JSON_PREFIX_SEPERATOR = '-'

   CFS_APS_DIR = File.join(Osk::OSK_CFS_DIR,'apps')
   
   COSMOS_CONFIG_TARGETS_DIR  = File.join(Cosmos::USERPATH,'config','targets')
   COSMOS_CMD_TLM_SERVER_FILE = File.join(Cosmos::USERPATH,'config','tools','cmd_tlm_server','cmd_tlm_server.txt')
   
   CMD_TLM_TAG_START = "<cfs_kit>"
   CMD_TLM_TAG_END   = "</cfs_kit>"

   attr_reader :app_name
   attr_reader :app_name_map
   attr_reader :include_cosmos

   def initialize(app_name, template_var)
   
      create_app_name_map(app_name, template_var)
      @include_cosmos = false

   end # initialize()
      
   #
   # Maps information between the new app name and the case sensitive 
   # template variables. Assumes the app name is a single word and only
   # the first character is upper case for mixed case.
   # 
   # app_name      = Lowercase name of the application
   # template_vars = Array of strings  
   #
   def create_app_name_map(app_name, template_var)
   
      @app_name = Hash.new
      @app_name = {'UPPER': app_name.upcase, 'LOWER': app_name.downcase, 'MIXED': app_name.capitalize}
            
      @app_name_map = Hash.new
      @app_name_map = {template_var[0] => @app_name[:'UPPER'], template_var[1] => @app_name[:'MIXED'], template_var[2] => @app_name[:'LOWER']}
      
   end # initialize()

   #
   # Use the Osk::CREATE_APP_TEMPLATE_FILE in the template_dir to create
   # a new app in the cFS & COSMOS destination directories
   #
   def create_app(template_dir, cfs_dst_dir, cosmos_dst_dir)
      
      status = false

      begin
      
         ###template_dir   = File.join(Osk::CREATE_APP_DIR,'templates',template_dir)
         template_file  = File.join(template_dir,Osk::CREATE_APP_TEMPLATE_FILE)
         cfs_dst_dir    = File.join(cfs_dst_dir,@app_name[:"LOWER"])
         cosmos_dst_dir = File.join(cosmos_dst_dir,@app_name[:"UPPER"])

         # Should never happen since menu is created based on existing templates
         raise IOError "Template file #{template_file} does not exist" unless File.exist?(template_file)
      
         Dir.mkdir(cfs_dst_dir) unless Dir.exist?(cfs_dst_dir)

         template_json = File.read(template_file)
         template = JSON.parse(template_json)

         if template.key?(JSON_COSMOS)
            @include_cosmos = template[JSON_COSMOS]
         end
         
         template.each do |key,value|
            
            if key.start_with? JSON_CFS_PREFIX
               instantiate_template(key, value, JSON_CFS_PREFIX, template_dir, cfs_dst_dir)
            elsif key.start_with? JSON_COSMOS_PREFIX
               instantiate_template(key, value, JSON_COSMOS_PREFIX, template_dir, cosmos_dst_dir)
            elsif key.start_with? JSON_TABLES_PREFIX
               instantiate_template(key, value, JSON_TABLES_PREFIX, template_dir, Osk::CFS_CMAKE_DIR)
            end
      
         end # template loop
         
         status = true
      
      rescue

      end

      return status
      
   end # create_app()
   
   #
   # This method creates a directory for the template key and instantiates 
   # each file specified in the template entry. 
   #
   def instantiate_template(key, value, key_prefix, template_dir, dst_dir)

      #
      # Extract string after directory keyword & replace "-" with path 
      # separator. '/' which is the standard separator in Ruby.
      # Complete subdirectory path is specified in the key. Create an
      # absoute path using the dst_dir and the subdirectory in the key.  
      # - mkdir_p creates parent directories if needed
      #

      
      sub_dir = (key[key_prefix.length..key.length]).gsub(JSON_PREFIX_SEPERATOR,"/")

      if sub_dir.length > 0
         template_dir = File.join(template_dir,sub_dir)
         dst_dir      = File.join(dst_dir,sub_dir)
         FileUtils.mkdir_p(dst_dir) unless Dir.exist?(dst_dir)
      end

      files = value.split(',')
      
      files.each do |file|
      
         instantiate_file(template_dir, dst_dir, file)
         
      end
            
   end # instantiate_template()
   
   #
   # Since template files should be short no need for fancy optimized 
   # algorithms. Keep it simple and readable. 
   #
   def instantiate_file(template_dir, dst_dir, file)
	
      skip_comment = false;

      instantiated_text = ""
      File.open(File.join(template_dir,file), "r") do |src_file|
         src_file.each_line do |line|

         if (skip_comment)

            # Continue skipping until end of comment. Assume nothing else on last comment line
            skip_comment = (line.index(TEMPLATE_COMMENT_END).nil? or line.index(TEMPLATE_KEYWORD_END).nil?)

         else

            if (!line.index(TEMPLATE_COMMENT_START).nil? and !line.index(TEMPLATE_KEYWORD_START).nil?)
               skip_comment = true
            else
			    	   
               # Replace all occurrences for each case
               @app_name_map.each { |key, value| line = line.gsub(key, value) }

               instantiated_text << line

            end # if not starting a template comment block 
			       
         end # if not skipping comment block

         end # src_file lines
      end # src_file open
      
      # Replace template variable in filename 
      @app_name_map.each { |key, value| file = file.gsub(key, value) }
      
      File.open(File.join(dst_dir,file), "w") { |dst_file| dst_file.write(instantiated_text) }
			
   end # instantiate_file()
	

   #
   # Modify the existing COSMOS cmd_tlm_server definition file.
   # Return status array: [pass/fail, "text"]
   #
   def update_cosmos_cmd_tlm_server()

      status = [false,"error"]
      updated_text = ""
      add_new_target = true
      between_tags   = false
      
      File.open(COSMOS_CMD_TLM_SERVER_FILE, "r") do |cosmos_file|

         cosmos_file.each_line do |line|
      
         #
         # Simple logic with many assumptions:
         #   User doesn't duplicate an existing target name
         #   One set of start/end tags in proper order in the file
         # Logic won't duplicate new target if it exists between tags. 
         #
         
         if (between_tags and add_new_target)
            if (line.index(@app_name[:"UPPER"]).nil?)
               if (!line.index(CMD_TLM_TAG_END).nil?)
                  # Only get here if still need to add new target and finished with cfs_kit block
                  line.prepend("  TARGET  #{@app_name[:"UPPER"]}\n\n")
                  status[0] = true
                  status[1] = "added new entry"
                  add_new_target = false
               end
            else
               status[0] = true
               status[1] = "entry exists"
               add_new_target = false
            end
         else
            between_tags = !line.index(CMD_TLM_TAG_START).nil?
         end
            
         updated_text << line
      
         end # COSMOS file line
      end # COSMOS file
      
      File.open(COSMOS_CMD_TLM_SERVER_FILE, "w") { |cosmos_file| cosmos_file.write(updated_text) }
      
      return status

   end # update_cosmos_cmd_tlm_server()

   #
   # Read and return the description array in a template file
   #
   def self.get_descr(template_dir)
   
      template_file = File.join(template_dir,Osk::CREATE_APP_TEMPLATE_FILE)

      # If called from screen should never happen since screen menu is created based on
      # existing templates
      raise IOError "Missing application template file for #{template_file}" unless File.exist?(template_file)

      template_json = File.read(template_file)
      template = JSON.parse(template_json)

      if template.key? (JSON_DESCRIPTION)
         descr_array = template[JSON_DESCRIPTION]
      else
         descr_array = ['No description provided']
      end

      return descr_array

   end # get_descr()

   #
   # Return a template's title defined in its JSON file
   #
   def self.get_title(template_dir)
   
      template_file = File.join(template_dir,Osk::CREATE_APP_TEMPLATE_FILE)

      raise IOError "Missing application template file for #{template_file}" unless File.exist?(template_file)

      template_json = File.read(template_file)
      template = JSON.parse(template_json)

      title =""
      if template.key? (JSON_TITLE)
         title = template[JSON_TITLE]
      end

      return title

   end # get_title()

   #
   # 
   #
   def self.get_title(template_dir)
   
      template_file = File.join(template_dir,Osk::CREATE_APP_TEMPLATE_FILE)

      raise IOError "Missing application template file for #{template_file}" unless File.exist?(template_file)

      template_json = File.read(template_file)
      template = JSON.parse(template_json)

      title =""
      if template.key? (JSON_TITLE)
         title = template[JSON_TITLE]
      end

      return title

   end # get_title()

end # Class AppTemplate



