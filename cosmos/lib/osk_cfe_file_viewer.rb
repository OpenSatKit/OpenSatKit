# encoding: ascii-8bit
###############################################################################
# OpenSatKit cFE File Viewer
#
# License:
#
# Written by David McComas, licensed under the copyleft GNU General
# Public License (GPL). 
#
# The basis for code is 
#
# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt
# 
################################################################################


require 'cosmos'
Cosmos.catch_fatal_exception do
  require 'cosmos/script'
  require 'cosmos/gui/qt_tool'
  require 'cosmos/gui/dialogs/splash'
  require 'cosmos/packets/structure'
  require 'cosmos/packets/commands'
end
require 'osk_global'

module Cosmos

class OskCfeFileViewer < QtTool

   slots 'clear_txt_window()'
   
      
    # Contants 
    CFE_FILE_CONTENT_ID  = "cFE1" # All cFS generated files use this ID
    CFE_FILE_HDR_LEN     = 64     # Bytes
    CFE_FILE_DESCR_LEN   = 32     # Bytes

   @valid_tagets = []
       
   ##############################
   # 
   # initialize
   #
   def initialize(options)
      # MUST BE FIRST - All code before super is executed twice in RubyQt Based classes
      super(options)
      Cosmos.load_cosmos_icon("edit_tab.png")

      @filename = ""

      @argv_targets = []
      if ARGV.length != 0
        ARGV.each { |i| @argv_targets << i}
      end

      @valid_targets = @argv_targets

      initialize_actions()
      initialize_menus()
      initialize_central_widget()
      complete_initialize() # defined in qt_tool

      if options.file
         display_file(options.file)
      end

      # Bring up splash screen for long duration tasks after creation
      Splash.execute(self) do |splash|
         # Configure CosmosConfig to interact with splash screen
         ConfigParser.splash = splash

         #System.commands
         Qt.execute_in_main_thread(true) do
            #update_targets()
            #@target_select.setCurrentText(options.packet[0]) if options.packet
            #update_commands()
            #@cmd_select.setCurrentText(options.packet[1]) if options.packet
            #update_cmd_params()
         end

        # Unconfigure CosmosConfig to interact with splash screen
        ConfigParser.splash = nil
        
      end # Splash

   end # initialize()

   ##############################
   # 
   # initialize_actions
   #
   def initialize_actions
      
      super()

      # File actions
      @file_open = Qt::Action.new(Cosmos.get_icon('open.png'), tr('&Open'), self)
      @file_open_keyseq = Qt::KeySequence.new(tr('Ctrl+O'))
      @file_open.shortcut  = @file_open_keyseq
      @file_open.statusTip = tr('Open a file')
      @file_open.connect(SIGNAL('triggered()')) { file_open() }

      @file_save = Qt::Action.new(Cosmos.get_icon('save.png'), tr('&Save'), self)
      @file_save_keyseq = Qt::KeySequence.new(tr('Ctrl+S'))
      @file_save.shortcut  = @file_save_keyseq
      @file_save.statusTip = 'Save file'
      @file_save.connect(SIGNAL('triggered()')) { file_save() }
    
   end # initialize_actions()

   ##############################
   # 
   # initialize_menus
   #
   def initialize_menus
      
      # File Menu
      file_menu = menuBar.addMenu(tr('&File'))
      file_menu.addAction(@file_open)
      file_menu.addAction(@file_save)
    
   end # initialize_menus()

   ##############################
   # 
   # initialize_central_widget
   #
   def initialize_central_widget
      
      # Create the central widget
      central_widget = Qt::Widget.new
      setCentralWidget(central_widget)

      # Create the top level vertical layout
      # Set the size constraint to always respect the minimum sizes of the child widgets
      # If this is not set then when we refresh the command parameters they'll all be squished
      top_layout = Qt::VBoxLayout.new(central_widget)
      #top_layout.setSizeConstraint(Qt::Layout::SetMinimumSize)

      @file_label = Qt::Label.new("FILE: ")
      top_layout.addWidget(@file_label)

      # Separator Between Command Selection and Command Description
      sep1 = Qt::Frame.new(central_widget)
      sep1.setFrameStyle(Qt::Frame::HLine | Qt::Frame::Sunken)
      top_layout.addWidget(sep1)

      # Create the text edit where previously issued commands go and where
      # commands can be manually typed in. 
      @txt_window = Qt::TextEdit.new(statusBar)
      @txt_window.setFocus()
      top_layout.addWidget(@txt_window)

      # Button to clear the window
      clear_button = Qt::PushButton.new("Clear")
      connect(clear_button, SIGNAL('clicked()'), self, SLOT('clear_txt_window()'))
      top_layout.addWidget(clear_button)

   end # init_central_widget()
    
   ##############################
   # 
   # file_open
   #
   def file_open

      if not clear_window_if_ok()
        return
      end

      filename = Qt::FileDialog.getOpenFileName(self, "Select File", Osk::GND_SRV_DIR)

      display_file(filename) unless filename.nil?
          
   end #file_open()
    
   ##############################
   # 
   # file_save
   #
   def file_save

      filename = nil

      # Warn the user if the window is empty.
      if @txt_window.toPlainText.size() == 0 
         Qt::MessageBox.information(self, 'Warning', 'Text window is empty') 
      else

         filename = Qt::FileDialog.getSaveFileName(self, "Save file name", Osk::GND_SRV_DIR);

         if not filename.nil? 
         begin 
             
            outfile = File.open(filename,'w')
            outfile.write(@txt_window.toPlainText)
            outfile.close
         
         rescue => exception
            Qt::MessageBox.critical(self, 'Error', exception.message)
         end

         @file_label.text = "FILE: " + filename
         
         end # if not outfile.nil?
      end # if @txt_window.toPlainText.size
    
   end # file_save()

   ##############################
   # 
   # display_file
   #
   def display_file (filename)
   
      begin 
         
         file_data = ""
         in_file = File.open(filename,'r')
         
         file_hdr = in_file.read(CFE_FILE_HDR_LEN)

         if file_hdr[0..3] == CFE_FILE_CONTENT_ID
        
            file_data = in_file.read
            #in_file.each { |line|
            #   file_data << line
            #}
         
            @txt_window.clear()
            @txt_window.append(file_data)
            @txt_window.moveCursor(Qt::TextCursor::End)
 
            @file_label.text = "FILE: " + filename

         else
         
            Qt::MessageBox.warning(self, 'Invalid file', 'File header does not contain cFE file identifier #{CFE_FILE_CONTENT_ID}') 

         end

         #in_file.close
         
      rescue => exception
         Qt::MessageBox.critical(self, 'Error', exception.message)
      end
         
   
   end # display_file()

   
   ##############################
   # 
   # clear_window_if_ok
   #
   def clear_window_if_ok
      
      if @txt_window.toPlainText.size() != 0 
         case Qt::MessageBox.question(self, 'Clear commands', 'Are you sure?', 
              Qt::MessageBox::Yes | Qt::MessageBox::No , # buttons
              Qt::MessageBox::No) # default button
         when Qt::MessageBox::Yes
            @txt_window.clear()
            @file_label.text = "FILE: "
            retval = true
         else
            retval = false
         end
      else
         retval = true
      end
   
      retval
   
   end # clear_window_if_ok()

   ##############################
   # 
   # clear_txt_window
   #
   def clear_txt_window
      clear_window_if_ok()
   end
     
   ##############################
   # 
   # run
   #
   def self.run (option_parser = nil, options = nil)
      
      Cosmos.catch_fatal_exception do
      
         unless option_parser && options
            option_parser, options = create_default_options()
            #options.width = 600
            #options.height = 425
            options.title = 'Text File Display'
            options.file = nil
            
            option_parser.on("-f", "--file FILE", "Open and display the specified file") do |arg|
               options.file = arg
            end
         end

         super(option_parser, options)

      end
      
   end # run()

end # class OskTxtFileViewer
end # module Cosmos
