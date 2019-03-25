require "cosmos"
require "cosmos/tools/cmd_tlm_server/background_task"
require "cfdp_engine"
require "cfdp_vars"

module Cosmos

	class CfdpEngineTask < BackgroundTask

		CFDP_ENGINE_TASK_NAME = "CFDP Engine Task"
		attr_reader :pdu_received_counter
		attr_reader :pdu_sent_counter
		attr_reader :break

		def initialize_vars

			@pdu_received_counter = Hash.new
			@pdu_sent_counter = Hash.new

			CFDP::PDUCLASSES.each do |classe|
				@pdu_received_counter[classe] = 0
				@pdu_sent_counter[classe] = 0
			end			
		end

		def update_received_counter(pduClass)

			return unless CFDP::PDUCLASSES.include?(pduClass)
			@mutex.synchronize {@pdu_received_counter[pduClass]+=1}
		end

		def update_sent_counter(pduClass)

			return unless CFDP::PDUCLASSES.include?(pduClass)
			@mutex.synchronize {@pdu_sent_counter[pduClass]+=1}			
		end

		def call

			@break = false
			@name = CFDP_ENGINE_TASK_NAME
			@status = "Initializing"
			@mutex = Mutex.new

			begin

				@cfdp_engine = CFDP::CFDPEngine.instance
				initialize_vars
			rescue Exception => err

				Logger.error "Failed to initialize CFDP Engine.\n Error is #{err}. Backtrace is #{err.backtrace}"
				@status = "Failed to initialize"
				return
			ensure

				raise unless @cfdp_engine.is_a?(CFDP::CFDPEngine)
				raise if @pdu_received_counter.nil?
				raise if @pdu_sent_counter.nil?
			end

			@status = "Running"
			Logger.info "CFDP Engine has started."

			until @break

				CFDP::CFDPEngine.instance.cycleTimeout
				sleep(SLEEP_TIME_BTW_PDUS)

				begin

					pdu = CFDP::CFDPEngine.instance.pdu2beSent

					next if pdu.nil?
					pdu = pdu.pack

					# Transmissions errors are handled by the protocol itself
					cmd("#{PDU_TARGET_NAME} #{PDU_SEND_TARGET_PACKET} with CCSDS_LENGTH #{pdu.length+1}, PAYLOAD #{"0x"+pdu.pack('c*').unpack('H*')[0]}")
				rescue Exception => err

					Logger.error "Error while sending PDU. Error is #{err}.\n Backtrace is #{err.backtrace}"
					next
				end
			end

			ensure CFDP::CFDPEngine.instance.finalize
		end

		def stop

			@break = true
			Logger.info "CFDP Engine has ended."
			@status = "Stopped"
		end
	end
end