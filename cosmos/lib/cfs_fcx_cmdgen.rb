# encoding: ascii-8bit
# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt


require 'cosmos'
Cosmos.catch_fatal_exception do
  require 'cosmos/script'
  require 'cosmos/gui/qt_tool'
  require 'cosmos/gui/dialogs/splash'
  require 'cosmos/packets/structure'
  require 'cosmos/packets/commands'
end

module Cosmos

  #-----------------------------------------------------------
  #------------------ Class: CfsFcxCmdGen ---------------------
  #-----------------------------------------------------------
  class CfsFcxCmdGen < QtTool
    slots 'target_changed(const QString&)'
    slots 'click_callback(QTableWidgetItem*)'
    slots 'update_cmd_params(bool)'
    slots 'cmd_changed(const QString&)'
    slots 'add_button()'
    slots 'clear_button()'

    @valid_tagets = []
       
    # Contants 
    MANUALLY = "MANUALLY ENTERED"
    CFE_FS_FILE_CONTENT_ID  = "cFE1" # "cFE1"
    CFE_FS_FCX_SUB_TYPE     = 0x00004643   # "FC" - FCX file format
    CFE_FS_FED_SUB_TYPE     = 0x00004544   # "ED" - FCX-ED (embedded delay) file format
    CFE_FS_FILE_HDR_LEN     = 64
    CFE_FS_SC_ID            = 8
    CFE_FS_PROC_ID          = 1
    CFE_FS_APP_ID           = 0xC2
    CFE_FS_TIMESECS         = 0
    CFE_FS_TIMEUSECS        = 0
    CFE_FS_DESCR            = "This is a FCX file."
    CFE_FS_DESCR_LENGTH     = 32
    CFE_FS_HDR_LENGTH       = 64 # Total file hdr length in bytes

    # CCSDS pkt header len
    CCSDS_PKT_HEADER_LENGTH = 6

    # Size of delay field (in bytes)
    DELAY_SIZE              = 4

    # Max packet size
    MAX_CMD_PKT_LEN         = 4 * 1024  # totally arbitrary 

    def self.param_widgets
      @@param_widgets
    end

    ##############################
    # 
    # initialize
    #
    def initialize(options)
# MUST BE FIRST - All code before super is executed twice in RubyQt Based classes
      super(options)
      Cosmos.load_cosmos_icon("edit_tab.png")

      @filename = ""
      @@param_widgets = []
      @@table = nil

      @argv_targets = []
      if ARGV.length != 0
        ARGV.each { |i| @argv_targets << i}
      end

      @valid_targets = @argv_targets

      initialize_actions()
      initialize_menus()
      initialize_central_widget()
      complete_initialize() # defined in qt_tool

      # Bring up slash screen for long duration tasks after creation
      Splash.execute(self) do |splash|
        # Configure CosmosConfig to interact with splash screen
        ConfigParser.splash = splash

        System.commands
        Qt.execute_in_main_thread(true) do
          update_targets()
          @target_select.setCurrentText(options.packet[0]) if options.packet
          update_commands()
          @cmd_select.setCurrentText(options.packet[1]) if options.packet
          update_cmd_params()
        end

        # Unconfigure CosmosConfig to interact with splash screen
        ConfigParser.splash = nil
      end

    end # initialize

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

      # Config action
      @select_embedded_delay = Qt::Action.new('Embedded delay type',self)
      @select_embedded_delay.statusTip = 'Inter-command delay (ms) must be specified before each command'
      @select_embedded_delay.setCheckable(true)
      @select_embedded_delay.setChecked(false)
    end

    ##############################
    # 
    # initialize_menus
    #
    def initialize_menus
      # File Menu
      file_menu = menuBar.addMenu(tr('&File'))
      file_menu.addAction(@file_open)
      file_menu.addAction(@file_save)

      # Config menu
      config_menu = menuBar.addMenu(tr('&Config'))
      config_menu.addAction(@select_embedded_delay)
    end

    ##############################
    # 
    # initialize_central_widget
    #
    def initialize_central_widget
      # Create the central widget
      central_widget = Qt::Widget.new
      setCentralWidget(central_widget)

      # Create the top half of the splitter window
      sender = Qt::Widget.new

      # Create the top level vertical layout
      top_layout = Qt::VBoxLayout.new(sender)
      # Set the size constraint to always respect the minimum sizes of the child widgets
      # If this is not set then when we refresh the command parameters they'll all be squished
      top_layout.setSizeConstraint(Qt::Layout::SetMinimumSize)

      # Set the target combobox selection
      @target_select = Qt::ComboBox.new
      @target_select.setMaxVisibleItems(6)
      connect(@target_select, SIGNAL('activated(const QString&)'), self, SLOT('target_changed(const QString&)'))
      target_label = Qt::Label.new(tr("&Target:"))
      target_label.setBuddy(@target_select)

      # Set the comamnd combobox selection
      @cmd_select = Qt::ComboBox.new
      @cmd_select.setMaxVisibleItems(20)
      connect(@cmd_select, SIGNAL('activated(const QString&)'), self, SLOT('cmd_changed(const QString&)'))
      cmd_label = Qt::Label.new(tr("&Command:"))
      cmd_label.setBuddy(@cmd_select)

      # Button to add command
      add = Qt::PushButton.new("Add")
      connect(add, SIGNAL('clicked()'), self, SLOT('add_button()'))

      # Button to clear the window
      clear = Qt::PushButton.new("Clear")
      connect(clear, SIGNAL('clicked()'), self, SLOT('clear_button()'))

      # Layout the top level selection
      select_layout = Qt::HBoxLayout.new
      select_layout.addWidget(target_label)
      select_layout.addWidget(@target_select, 1)
      select_layout.addWidget(cmd_label)
      select_layout.addWidget(@cmd_select, 1)
      select_layout.addWidget(add)
      top_layout.addLayout(select_layout)

      # Separator Between Command Selection and Command Description
      sep1 = Qt::Frame.new(sender)
      sep1.setFrameStyle(Qt::Frame::HLine | Qt::Frame::Sunken)
      top_layout.addWidget(sep1)

      # Command Description Label
      dec_label = Qt::Label.new(tr("Description:"))
      @description = Qt::Label.new('')
      @description.setWordWrap(true)
      desc_layout = Qt::HBoxLayout.new
      desc_layout.addWidget(dec_label)
      desc_layout.addWidget(@description, 1)
      top_layout.addLayout(desc_layout)

      # Separator Between Description and parameters
      sep2 = Qt::Frame.new(sender)
      sep2.setFrameStyle(Qt::Frame::HLine | Qt::Frame::Sunken)
      top_layout.addWidget(sep2)

      # Parameters Label
      param_label = Qt::Label.new(tr("Parameters:"))
      top_layout.addWidget(param_label)

      # Grid Layout for Parameters
      @table_layout = Qt::VBoxLayout.new
      top_layout.addLayout(@table_layout, 500)

      # Add stretch to force everything to fit against the top of the window
      # otherwise the selection window, description, and parameters all try
      # to get equal space.
      top_layout.addStretch(1)

      # Create the text edit where previously issued commands go and where
      # commands can be manually typed in. 
      @cmd_window = Qt::TextEdit.new(statusBar)
      @cmd_window.setFocus()

      layout = Qt::VBoxLayout.new
      layout.setSpacing(1)
      layout.setContentsMargins(1, 1, 1, 1)
      layout.setSizeConstraint(Qt::Layout::SetMaximumSize)
      @comment = Qt::Label.new("")
      layout.addWidget(@comment)
      layout.addWidget(@cmd_window)
      history = Qt::Widget.new
      history.layout = layout

      # Create the scroll area
      scroll = Qt::ScrollArea.new
      scroll.setMinimumSize(500, 150)
      scroll.setWidgetResizable(true)
      scroll.setWidget(sender)

      splitter = Qt::Splitter.new(central_widget)
      splitter.setOrientation(Qt::Vertical)
      splitter.addWidget(scroll)
      splitter.addWidget(history)
      splitter.setStretchFactor(0,10)
      splitter.setStretchFactor(1,1)

      layout = Qt::VBoxLayout.new
      layout.setSpacing(1)
      layout.setContentsMargins(1, 1, 1, 1)
      layout.setSizeConstraint(Qt::Layout::SetMaximumSize)
      layout.addWidget(splitter)
      central_widget.layout = layout
      layout.addWidget(clear)

    end # init_central_widget
    
    ##############################
    # 
    # file_open
    #
    def file_open

      if not clear_window_if_ok()
        return
      end

      filename = Qt::FileDialog.getOpenFileName(self, "Select File")

      if filename

        @infile = File.open(filename, "r")

        # Read and Verify file format is CFE file
        file_hdr = @infile.read(CFE_FS_HDR_LENGTH)

        if file_hdr[0..3]  == CFE_FS_FILE_CONTENT_ID

          # Is it an embedded delay type of file?
          if file_hdr[4..7].unpack("L>") == [CFE_FS_FED_SUB_TYPE]
            file_is_ed_type = true
          elsif file_hdr[4..7].unpack("L>") == [CFE_FS_FCX_SUB_TYPE]
            file_is_ed_type = false
          else
            Qt::MessageBox.warning(self, 'Invalid file', 'Not a valid FCX file.') 
            return
          end

          @ccsds_hdr = CcsdsPacket.new(nil, nil, true)
          pkt_count = 0
          # Clear the input window
          @cmd_window.clear()

          # Read the packet header
          while true
            
            # Read the delay value, it preceeds the command
            if file_is_ed_type
              data = @infile.read(DELAY_SIZE)

              if not data
                break
              end

              output_string = "DELAY "
              output_string << "%d" % data.unpack("L>")

              @cmd_window.append(output_string)
              @cmd_window.moveCursor(Qt::TextCursor::End)
            end

            # Now read the command packet header
            data = @infile.read(CCSDS_PKT_HEADER_LENGTH)
            if not data 
              break  # file is finished
            else 
              if data.length != CCSDS_PKT_HEADER_LENGTH
                Qt::MessageBox.warning(self, 'Invalid command', 'Packet header error: %d' % data.length) 
                break
              end
            end
            
            @ccsds_hdr.buffer = data
#puts "Hdr: %s" % @ccsds_hdr.buffer.unpack("H*") 
#puts "Hdr len: %d" % @ccsds_hdr.buffer.length
#puts "Pkt len: %d" % @ccsds_hdr.read('CcsdsLength')

            # Now read the rest of the packet 
            pkt_body = @infile.read(@ccsds_hdr.read('CcsdsLength') + 1)
            if pkt_body.nil? 
              Qt::MessageBox.warning(self, 'Invalid command', '??')
              break
            elsif pkt_body.length != @ccsds_hdr.read('CcsdsLength') + 1
              Qt::MessageBox.warning(self, 'Invalid command', "Got: %d bytes," % pkt_body.length + "Expected: %d" % (@ccsds_hdr.read('CcsdsLength') + 1) )
              break
            end

            pkt_data = @ccsds_hdr.buffer 
            pkt_data << pkt_body

#          puts "CcsdsVersion: " + @ccsds_hdr.read('CcsdsVersion').to_s 
#          puts "CcsdsType: " + @ccsds_hdr.read('CcsdsType').to_s
#          puts "CcsdsApid: " + @ccsds_hdr.read('CcsdsApid').to_s 
#          puts "CcsdsShf: " + @ccsds_hdr.read('CcsdsShf').to_s 
#          puts "CcsdsSeqflags: " + @ccsds_hdr.read('CcsdsSeqflags').to_s 
#          puts "CcsdsSeqcnt: " + @ccsds_hdr.read('CcsdsSeqcnt').to_s 
#          puts "CcsdsLength: " + @ccsds_hdr.read('CcsdsLength').to_s

#puts pkt_data.unpack("H*") 
            # NOTE: Dont include targets other than CFE, etc. you may get
            # bogus results (i.e. wrong packet ID) 
            # Verify the command header
            packet = System.commands.identify(pkt_data, @valid_targets)
#puts packet.buffer.unpack("H*")
            if packet.nil?
              Qt::MessageBox.warning(self, 'Invalid command', "File: "+filename)
              @cmd_window.clear()
              filename = ""
              break
            end

            cmd_str =  System.commands.format(packet)
            #puts cmd_str
            cmd_strs = cmd_str.split("'")
            
#          puts cmd_strs[1]
            
            target_name,packet_name,params = extract_fields_from_cmd_text(cmd_strs[1])
#          puts t,c,params

            # ->  TARGET PACKET {CMD_PARAMS} 
            output_string = target_name
            output_string << " "
            output_string << packet_name
            output_string << " "
            output_string << params.inspect

#          puts output_string
            @cmd_window.append(output_string)
            @cmd_window.moveCursor(Qt::TextCursor::End)
          end 
        else
          Qt::MessageBox.warning(self, 'Invalid file', 'Not a valid FCX file.') 
        end

        @infile.close

        @comment.text = "FILE: "+filename
      end # if filename
    end

    ##############################
    # 
    # add_button
    #
    def add_button
      target_name = @target_select.text
      packet_name = @cmd_select.text

      output_string, params = view_as_script()

      # Discard the generated cmd string and build a new one:
      # ->  TARGET PACKET {CMD_PARAMS} 
      output_string = target_name
      output_string << " "
      output_string << packet_name
      output_string << " "
      output_string << params.inspect


      @cmd_window.append(output_string)
      @cmd_window.moveCursor(Qt::TextCursor::End)
      @cmd_window.ensureCursorVisible()

    end # add_button
    
    ##############################
    # 
    # file_save
    #
    def file_save

      filename = nil

      # Warn the user if the window is empty.
      if @cmd_window.toPlainText.size() == 0 
        Qt::MessageBox.information(self, 'Warning', 'No commands have been entered') 
      else

        filename = Qt::FileDialog.getSaveFileName(self, "Save file name");

        if not filename.nil? 
          begin 
            outfile = File.open(filename,'w')
          
            outfile.write(CFE_FS_FILE_CONTENT_ID)

            # File sub-type
            if @select_embedded_delay.checked?
              outfile.write([CFE_FS_FED_SUB_TYPE].pack("L>"))
            else
              outfile.write([CFE_FS_FCX_SUB_TYPE].pack("L>"))
            end
            outfile.write([CFE_FS_FILE_HDR_LEN].pack("L>"))
            outfile.write([CFE_FS_SC_ID].pack("L>"))
            outfile.write([CFE_FS_PROC_ID].pack("L>"))
            outfile.write([CFE_FS_APP_ID].pack("L>"))

            time = Time.new
            outfile.write([time.tv_sec].pack("L>"))   
            outfile.write([CFE_FS_TIMEUSECS].pack("L>"))
            outfile.write(File.basename(filename).ljust(CFE_FS_DESCR_LENGTH,' '))

            # For each command in the input window
            pkt_count = 0    # keep a local count
            @cmd_window.toPlainText.split("\n").each do |cmd_str|

              # IF the file format is ED, convert the delay value
              # every other line is a 'delay' command
              if @select_embedded_delay.checked? and (pkt_count % 2 == 0)
                delay_cmd, delay_val = cmd_str.split(" ",2)
                
                if delay_cmd.upcase == "DELAY"
                  outfile.write([delay_val.to_i].pack("L>"))
                else
                  break
                end
              else
                # Else the line is an actual FSW command
                target_name, packet_name, params = cmd_str.split(" ",3)
                cmd_params = eval(params)
             
                command = System.commands.build_cmd(target_name, packet_name, cmd_params)

                outfile << command.buffer
              end
              pkt_count += 1
            end
            outfile.close
          rescue => exception
            Qt::MessageBox.critical(self, 'Error', exception.message)
          end

          @comment.text = "FILE: "+filename
        end # if not outfile.nil?
      end # if @cmd_window.toPlainText.size
    end

    ##############################
    # 
    # clear_window_if_ok
    #
    def clear_window_if_ok
      if @cmd_window.toPlainText.size() != 0 
        case Qt::MessageBox.question(self, 'Clear commands', 'Are you sure?', 
            Qt::MessageBox::Yes | Qt::MessageBox::No , # buttons
            Qt::MessageBox::No) # default button
        when Qt::MessageBox::Yes
          @cmd_window.clear()
          @comment.text = ""
          retval = true
        else
          retval = false
        end
      else
        retval = true
      end
      retval
    end

    ##############################
    # 
    # clear_button
    #
    def clear_button
      clear_window_if_ok()
    end
     
    ##############################
    # 
    # view_as_script
    #
    def view_as_script
      params = {}

      @@param_widgets.each do |packet_item, value_item, state_value_item|
        text = value_item.text

        text = state_value_item.text if state_value_item and (text == MANUALLY )
        quotes_removed = text.remove_quotes
        if text == quotes_removed
          params[packet_item.name] = text.convert_to_value
        else
          params[packet_item.name] = quotes_removed
        end
        raise "#{packet_item.name} is required." if quotes_removed == '' and packet_item.required
      end
      statusBar.clearMessage()
      
      output_string = System.commands.build_cmd_output_string(@target_select.text, @cmd_select.text, params, false) 
      if output_string =~ /[\x00-\x08\x0B-\x0C\x0E-\x1F\x7F-\xFF]/
        output_string = output_string.inspect.remove_quotes
      end

      return output_string, params
    end

    ##############################
    # 
    # update_targets
    #
    def update_targets
      @target_select.clearItems()

      # IF the list of valid targets was specified on the command line 
      if @argv_targets.length > 0
        target_names = @argv_targets
      else
        target_names = System.commands.target_names
      end
      target_names_to_delete = []
      target_names.each do |target_name|
        found_non_hidden = false
        begin
          packets = System.commands.packets(target_name)
          packets.each do |packet_name, packet|
            found_non_hidden = true unless packet.hidden
          end
        rescue
          # Don't do anything
        end
        target_names_to_delete << target_name unless found_non_hidden
      end
      target_names_to_delete.each do |target_name|
        target_names.delete(target_name)
      end
      target_names.each do |target_name|
        @target_select.addItem(target_name)
      end
    end

    ##############################
    # 
    # target_changed
    #
    def target_changed(target)
      update_commands()
      update_cmd_params()
    end

    ##############################
    # 
    # cmd_changed
    #
    def cmd_changed(command)
      update_cmd_params()
    end

    ##############################
    # 
    # update_commands
    #
    def update_commands
      @cmd_select.clearItems()
      target_name = @target_select.text
      if target_name
        commands = System.commands.packets(@target_select.text)
        command_names = []
        commands.each do |command_name, command|
          command_names << command_name unless command.hidden
        end
        command_names.sort!
        command_names.each do |command_name|
          @cmd_select.addItem(command_name)
        end
      end
    end

    ##############################
    # 
    # update_cmd_params
    #
    def update_cmd_params(ignored_toggle = nil)
      old_params = {}
      if ignored_toggle.nil?
        ignored_toggle = false
      else
        ignored_toggle = true
        # Save parameter values
        @@param_widgets.each do |packet_item, value_item, state_value_item|
          text = value_item.text
          if state_value_item
            old_params[packet_item.name] = [text, state_value_item.text]
          else
            old_params[packet_item.name] = text
          end
        end
      end
      # Clear Status Bar
      statusBar.showMessage(tr(""))

      target_name = @target_select.text
      target = System.targets[target_name]
      packet_name = @cmd_select.text
      if target_name and packet_name
        packet = System.commands.packet(target_name, packet_name)
        packet_items = packet.sorted_items
        shown_packet_items = []
        packet_items.each do |packet_item|
          next if target and target.ignored_parameters.include?(packet_item.name) 
          shown_packet_items << packet_item
        end

        # Update Command Description
        @description.text = packet.description.to_s

        # Destroy the old table widget
        @@table.dispose if @@table
        @@table = nil

        # Update Parameters
        @@param_widgets = []
        drawn_header = false

        row = 0
        shown_packet_items.each do |packet_item|
          next if target and target.ignored_parameters.include?(packet_item.name) 
          value_item = nil
          state_value_item = nil

          unless drawn_header
            @@table = Qt::TableWidget.new()
            @@table.setSizePolicy(Qt::SizePolicy::Expanding, Qt::SizePolicy::Expanding)
            @@table.setWordWrap(true)
            @@table.setRowCount(shown_packet_items.length)
            @@table.setColumnCount(5)
            @@table.setHorizontalHeaderLabels(['Name', '         Value or State         ', '         ', 'Units', 'Description'])
            @@table.horizontalHeader.setStretchLastSection(true)
            @@table.verticalHeader.setVisible(false)
#            @@table.setItemDelegate(CmdSenderItemDelegate.new(@@table))
            @@table.setContextMenuPolicy(Qt::CustomContextMenu)
            @@table.verticalHeader.setResizeMode(Qt::HeaderView::ResizeToContents)
            @@table.setEditTriggers(Qt::AbstractItemView::AllEditTriggers)
            @@table.setSelectionMode(Qt::AbstractItemView::NoSelection)
#            connect(@@table, SIGNAL('customContextMenuRequested(const QPoint&)'), self, SLOT('context_menu(const QPoint&)'))
#            connect(@@table, SIGNAL('itemClicked(QTableWidgetItem*)'), self, SLOT('click_callback(QTableWidgetItem*)'))
            drawn_header = true
          end

          # Parameter Name
          item = Qt::TableWidgetItem.new("#{packet_item.name}:")
          item.setTextAlignment(Qt::AlignRight | Qt::AlignVCenter)
          item.setFlags(Qt::NoItemFlags | Qt::ItemIsSelectable | Qt::ItemIsEnabled)
          @@table.setItem(row, 0, item)

          if packet_item.states
            default_state = packet_item.states.key(packet_item.default)
            if old_params[packet_item.name]
              value_item = Qt::TableWidgetItem.new(old_params[packet_item.name][0])
            else
              if default_state
                value_item = Qt::TableWidgetItem.new(default_state.to_s)
              else
                value_item = Qt::TableWidgetItem.new(MANUALLY)
              end
            end
            value_item.setTextAlignment(Qt::AlignRight | Qt::AlignVCenter)
            value_item.setFlags(Qt::NoItemFlags | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable)
            @@table.setItem(row, 1, value_item)

            if old_params[packet_item.name]
              state_value_item = Qt::TableWidgetItem.new(old_params[packet_item.name][1])
            else
              if packet_item.default.kind_of?(Integer)
                state_value_item = Qt::TableWidgetItem.new(sprintf("0x%X", packet_item.default))
              else
                state_value_item = Qt::TableWidgetItem.new(packet_item.default.to_s)
              end
            end
            state_value_item.setTextAlignment(Qt::AlignRight | Qt::AlignVCenter)
            state_value_item.setFlags(Qt::NoItemFlags | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable)
            @@table.setItem(row, 2, state_value_item)

            # If the parameter is required set the combobox to MANUAL and
            # clear the value field so they have to choose something
            if packet_item.required and !old_params[packet_item.name]
              value_item.setText(MANUALLY)
              state_value_item.setText('')
            end
          else
            # Parameter Value
            if old_params[packet_item.name]
              value_item = Qt::TableWidgetItem.new(old_params[packet_item.name])
            else
              if packet_item.required
                value_item = Qt::TableWidgetItem.new('')
              else
                if packet_item.format_string
                  begin
                    value_item = Qt::TableWidgetItem.new(sprintf(packet_item.format_string, packet_item.default))
                  rescue
                    # Oh well - Don't use the format string
                    value_item = Qt::TableWidgetItem.new(packet_item.default.to_s)
                  end
                else
                  value_item = Qt::TableWidgetItem.new(packet_item.default.to_s)
                end
              end
            end
            value_item.setTextAlignment(Qt::AlignRight | Qt::AlignVCenter)
            value_item.setFlags(Qt::NoItemFlags | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable)
            @@table.setItem(row, 1, value_item)
            @@table.setSpan(row, 1, 1, 2)
          end

          # Units
          item = Qt::TableWidgetItem.new(packet_item.units.to_s)
          item.setTextAlignment(Qt::AlignRight | Qt::AlignVCenter)
          item.setFlags(Qt::NoItemFlags | Qt::ItemIsSelectable | Qt::ItemIsEnabled)
          @@table.setItem(row, 3, item)

          # Description
          item = Qt::TableWidgetItem.new(packet_item.description.to_s)
          item.setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter)
          item.setFlags(Qt::NoItemFlags | Qt::ItemIsSelectable | Qt::ItemIsEnabled)
          @@table.setItem(row, 4, item)

          @@param_widgets << [packet_item, value_item, state_value_item]
          row += 1
        end

        if @@table
          @@table.connect(SIGNAL('itemChanged(QTableWidgetItem*)')) do |item|
            packet_item, value_item, state_value_item = @@param_widgets[item.row]
            if item.column == 1
              if packet_item.states
                value = packet_item.states[value_item.text]
                @@table.blockSignals(true)
                if value.kind_of?(Integer)
                  state_value_item.setText(sprintf("0x%X", value))
                else
                  state_value_item.setText(value.to_s)
                end
                @@table.blockSignals(false)
              end
            elsif item.column == 2
              @@table.blockSignals(true)
              @@table.item(item.row, 1).setText(MANUALLY)
              @@table.blockSignals(false)
            end
          end
          @table_layout.addWidget(@@table, 500)
          @@table.resizeColumnsToContents()
          @@table.resizeRowsToContents()
        end
      end # if target_name and packet_name
    end # update_cmd_params

    def click_callback(item)
      @@table.editItem(item) if (item.flags & Qt::ItemIsEditable) != 0
    end

    def self.run (option_parser = nil, options = nil)
      puts"running fcx cmdgen"
      Cosmos.catch_fatal_exception do
        unless option_parser && options
          option_parser, options = create_default_options()
          #options.width = 600
          #options.height = 425
          options.title = 'FCX File Generator'
        end

        super(option_parser, options)

      end
    end

  end # class
end #module Cosmos
