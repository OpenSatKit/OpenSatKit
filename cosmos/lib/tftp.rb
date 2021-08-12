
###############################################################################
# TFTP Utilites
#
# Notes:
#   1. I had trouble creating a gem so I copied the class into the /lib
#      folder.
#
# License:
#   TFTP class written by Guillaume Marcais, and distributed under the Ruby
#   licensed. See TFTP protocol below.
#
###############################################################################

#! /usr/bin/ruby

#--
# Copyright (c) 2004, Guillaume Marcais (guillaume.marcais@free.fr)
# All rights reserved.
# This file is distributed under the Ruby license.
# http://net-tftp.rubyforge.org
#++
#
# == Description
# TFTP is used by many devices to upload or download their configuration,
# firmware or else. It is a very simple file transfer protocol built on top
# of UDP. It transmits data by chunck of 512 bytes. It waits for an ack after 
# each data packet but does not do any data integrety checks. 
# There is no authentication mechanism nor any way too list the content of 
# the remote directories. It just sends or retrieves files.
#
# == Usage
# Using TFTP is fairly trivial:
# <pre>
# <code>
# require 'net/tftp'
# t = Net::TFTP.new('localhost')
# t.getbinaryfile('remote_file', 'local_file')
# t.putbinaryfile('local_file', 'remote_file')
# </code>
# </pre>
#
# That's pretty much it. +getbinaryfile+ and +putbinaryfile+ can take a
# block which will be called every time a block is sent/received.
#
# == Known limitations
# * RFC 1350 mention a net-ascii mode. I am not quite sure what transformation
#   on the data should be done and it is not (yet) implemented.
# * None of the extensions of TFTP are implemented (RFC1782, RFC1783, RFC1784, 
#   RFC1785, RFC2347, RFC2348, RFC2349).
 
require 'socket'
require 'timeout'

##module Net # :nodoc:

  class TFTPError < StandardError; end
  class TFTPTimeout < TFTPError; end
  class TFTPProtocol < TFTPError
    attr_reader :code
    def initialize(msg, code)
      super(msg)
      @code = code
    end
  end

  
  class TFTP
    VERSION = "0.1.0"
    DEFAULTS = {
      :port => (Socket.getservbyname("tftp", "udp") rescue 69),
      :timeout => 5,
    }
    
    MINSIZE = 4
    MAXSIZE = 516
    DATABLOCK = 512
    
    # Errors
    ERROR_DESCRIPTION = [
      "Custom error",
      "File not found",
      "Access violation",
      "Disk full",
      "Illegal TFP operation",
      "Unknown transfer ID",
      "File already exists",
      "No such user",
    ]
    ERROR_UNDEF                 = 0
    ERROR_FILE_NOT_FOUND        = 1
    ERROR_ACCESS_VIOLATION      = 2
    ERROR_DISK_FULL             = 3
    ERROR_ILLEGAL_OPERATION     = 4
    ERROR_UNKNOWN_TRANSFER_ID   = 5
    ERROR_FILE_ALREADY_EXISTS   = 6
    ERROR_NO_SUCH_USER          = 7
    
    # Opcodes
    OP_RRQ   = 1
    OP_WRQ   = 2
    OP_DATA  = 3
    OP_ACK   = 4
    OP_ERROR = 5
    
    class << self
      # Alias for new
      def open(host)
        new(host)
      end

      # Return the number of blocks to send _size_ bytes.
      def size_in_blocks(size)
        s = size / DATABLOCK
        s += 1 unless (size % DATABLOCK) == 0
        s
      end
    end
    
    attr_accessor :timeout, :host

    # Create a TFTP connection object to a host. Note that no actual
    # network connection is made. This methods never fails.
    # Parameters:
    # [:port] The UDP port. See DEFAULTS
    # [:timeout] Timeout in second for each ack packet. See DEFAULTS
    def initialize(host, params = {})
      @host = host
      @port = params[:port] || DEFAULTS[:port]
      @timeout = params[:timeout] || DEFAULTS[:timeout]
    end

    # Retrieve a file using binary mode.
    # If the localfile name is omitted, it is set to the remotefile.
    # The optional block receives the data in the block and the sequence number
    # of the block starting at 1.
    def getbinaryfile(remotefile, localfile = nil, &block) # :yields: data, seq
      localfile ||= File.basename(remotefile)
      open(localfile, "w") do |f|
        getbinary(remotefile, f, &block)
      end
    end

    # Retrieve a file using binary mode and send content to an io object
    # The optional block receives the data in the block and the sequence number
    # of the block starting at 1.
    def getbinary(remotefile, io, &block) # :yields: data, seq
      s = UDPSocket.new
      begin
        peer_ip = IPSocket.getaddress(@host)
      rescue
        raise TFTPError, "Cannot find host '#{@host}'"
      end

      peer_tid = nil
      seq = 1
      from = nil
      data = nil

      # Initialize request
      s.send(rrq_packet(remotefile, "octet"), 0, peer_ip, @port)
      Timeout::timeout(@timeout, TFTPTimeout) do
        loop do
          packet, from = s.recvfrom(MAXSIZE, 0)
          next unless peer_ip == from[3]
          type, block, data = scan_packet(packet)
          break if (type == OP_DATA) && (block == seq)
        end
      end
      peer_tid = from[1]

      # Get and write data to io
      loop do
        io.write(data)
        s.send(ack_packet(seq), 0, peer_ip, peer_tid)
        yield(data, seq) if block_given?
        break if data.size < DATABLOCK
        
        seq += 1
        Timeout::timeout(@timeout, TFTPTimeout) do
          loop do
            packet, from = s.recvfrom(MAXSIZE, 0)
            next unless peer_ip == from[3]
            if peer_tid != from[1]
              s.send(error_packet(ERROR_UNKNOWN_TRANSFER_ID), 
                     0, from[3], from[1])
              next
            end
            type, block, data = scan_packet(packet)
            break if (type == OP_DATA) && (block == seq)
          end
        end
      end

      return true
    end

    # Send a file in binary mode. The name of the remotefile is set to
    # the name of the local file if omitted.
    # The optional block receives the data in the block and the sequence number
    # of the block starting at 1.
    def putbinaryfile(localfile, remotefile = nil, &block) # :yields: data, seq
      remotefile ||= File.basename(localfile)
      open(localfile) do |f|
        putbinary(remotefile, f, &block)
      end
    end

    # Send the content read from io to the remotefile.
    # The optional block receives the data in the block and the sequence number
    # of the block starting at 1.
    def putbinary(remotefile, io, &block) # :yields: data, seq
      s = UDPSocket.new
      peer_ip = IPSocket.getaddress(@host)
      
      peer_tid = nil
      seq = 0
      from = nil
      data = nil
      
      # Initialize request
      s.send(wrq_packet(remotefile, "octet"), 0, peer_ip, @port)
      Timeout::timeout(@timeout, TFTPTimeout) do
        loop do
          packet, from = s.recvfrom(MAXSIZE, 0)
          next unless peer_ip == from[3]
          type, block, data = scan_packet(packet)
          break if (type == OP_ACK) && (block == seq)
        end
      end
      peer_tid = from[1]

      loop do
        data = io.read(DATABLOCK) || ""
        seq += 1
        s.send(data_packet(seq, data), 0, peer_ip, peer_tid)
        
        Timeout::timeout(@timeout, TFTPTimeout) do
          loop do
            packet, from = s.recvfrom(MAXSIZE, 0)
            next unless peer_ip == from[3]
            if peer_tid != from[1]
              s.send(error_packet(ERROR_UNKNOWN_TRANSFER_ID), 
                     0, from[3], from[1])
              next
            end
            type, block, void = scan_packet(packet)
            break if (type == OP_ACK) && (block == seq)
          end
        end

        yield(data, seq) if block_given?
        break if data.size < DATABLOCK
      end
      
      return true
    end

    ####################
    # Private methods  #
    ####################
    private
    def rrq_packet(file, mode)
      [OP_RRQ, file, mode].pack("na#{file.size + 1}a#{mode.size + 1}")
    end

    def wrq_packet(file, mode)
      [OP_WRQ, file, mode].pack("na#{file.size + 1}a#{mode.size + 1}")
    end

    def data_packet(block, data)
      [OP_DATA, block, data].pack("nna*")
    end

    def ack_packet(block)
      [OP_ACK, block].pack("nn")
    end

    def error_packet(code, message = nil)
      message ||= ERROR_DESCRIPTION[code] || ""
      [OP_ERROR, code, message].pack("nna#{message.size + 1}")
    end

    # Check if the packet is malformed (unknown opcode, too big, etc.),
    # in which case it returns nil.
    # If it is an error packet, raise an TFTPProtocol error.
    # Returns scanned values otherwise.
    def scan_packet(packet)
      return nil if packet.size < MINSIZE || packet.size > MAXSIZE
      opcode, block_err, rest = packet.unpack("nna*")
      return nil if opcode.nil? || block_err.nil?
      case opcode
      when OP_RRQ, OP_WRQ
        return nil
      when OP_DATA
        return [opcode, block_err, rest]
      when OP_ACK
        return [opcode, block_err]
      when OP_ERROR
        err_msg = "%s: %s"
        err_msg %= [ERROR_DESCRIPTION[block_err] || "", rest.chomp("\000")]
        raise TFTPProtocol.new(err_msg, block_err)
      else
        return nil
      end
    end
  end
##end
