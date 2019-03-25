# This file is a interface between CFDP Engine and Cosmos (not an external interface for Cosmos).
# It is used to send User Indication's messages to cmdtlmsrvr Logger.

require 'thread'
require 'cosmos'

module CFDP

	def CFDP_Indication(string)

		Cosmos::Logger.info string
	end

	module_function :CFDP_Indication
end

module Cosmos

	def Start_CFDP_Uplink(classType, destID, sourceFileName, destFileName)

		Thread.new do
			begin
				CFDP::CFDPEngine.instance.uplinkRequest(classType, destID, sourceFileName, destFileName)
			rescue Exception => err
				Cosmos::Logger.error ("UPLINK REQUEST ERROR: \"#{err.to_s}\"")
			end
		end
	end

	module_function :Start_CFDP_Uplink
end	# module Cosmos