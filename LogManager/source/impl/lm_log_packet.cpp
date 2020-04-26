#include "lm_log_packet.hpp"
#include "lm_scoped_log_file.hpp"

namespace ams::lm::impl {

    void WriteLogPacket(LogPacket packet, u64 program_id) {
        ScopedLogFile log_file;
        log_file.WriteFormat("\n");
        log_file.WriteFormat("/----------------------------------------------------------------------------------------------------\\\n");
        log_file.WriteFormat("|\n");
        log_file.WriteFormat("|    Log data (0x%016lX)\n", program_id);
        log_file.WriteFormat("|\n");
        if(!packet.payload.file_name.IsEmpty()) {
            log_file.WriteFormat("|    At %s", packet.payload.file_name.value);
            if(!packet.payload.function_name.IsEmpty()) {
                /* If function name is present, line number is present too. */
                /* At <file-name>:<line-number> (<function-name>) */
                log_file.WriteFormat(":%d (%s)", packet.payload.line_number.value, packet.payload.function_name.value);
            }
            log_file.WriteFormat("\n");
        }
        if(!packet.payload.process_name.IsEmpty()) {
            log_file.WriteFormat("|    Process name: %s\n", packet.payload.process_name.value);
        }
        if(!packet.payload.module_name.IsEmpty()) {
            log_file.WriteFormat("|    Module name: %s\n", packet.payload.module_name.value);
        }
        if(!packet.payload.thread_name.IsEmpty()) {
            log_file.WriteFormat("|    Thread name: %s\n", packet.payload.thread_name.value);
        }
        /* TODO: more fields! */
        log_file.WriteFormat("|\n");
        log_file.WriteFormat("\\----------------------------------------------------------------------------------------------------/\n");
        if(!packet.payload.text_log.IsEmpty()) {
            /* There is a message, print it at the end of the log. */
            log_file.WriteFormat("%s\n", packet.payload.text_log.value);
        }
        
    }
}