#include "lm_log_packet.hpp"
#include "lm_scoped_log_file.hpp"

namespace ams::lm::impl {

    namespace {

        bool can_access_fs = true;
        os::Mutex fs_access_lock(false);

    }

    void SetCanAccessFs(bool can_access) {
        std::scoped_lock lk(fs_access_lock);
        can_access_fs = can_access;
    }

    void WriteLogPackets(std::vector<LogPacket> &packet_list, u64 program_id) {
        std::scoped_lock lk(fs_access_lock);
        if(!can_access_fs) {
            return;
        }
        if(packet_list.empty()) {
            /* This shouldn't happen... */
            return;
        }
        /* For everything except the text log, use the first/head packet. */
        auto &head_packet = packet_list.front();
        ScopedLogFile log_file;
        log_file.WriteFormat("\n");
        log_file.WriteFormat("/----------------------------------------------------------------------------------------------------\\\n");
        log_file.WriteFormat("|\n");
        log_file.WriteFormat("|    Log data (0x%016lX)\n", program_id);
        log_file.WriteFormat("|\n");
        if(!head_packet.payload.file_name.IsEmpty()) {
            log_file.WriteFormat("|    At %s", head_packet.payload.file_name.value);
            if(!head_packet.payload.function_name.IsEmpty()) {
                /* If function name is present, line number is present too. */
                /* At <file-name>:<line-number> (<function-name>) */
                log_file.WriteFormat(":%d (%s)", head_packet.payload.line_number.value, head_packet.payload.function_name.value);
            }
            log_file.WriteFormat("\n");
        }
        if(!head_packet.payload.process_name.IsEmpty()) {
            log_file.WriteFormat("|    Process name: %s\n", head_packet.payload.process_name.value);
        }
        if(!head_packet.payload.module_name.IsEmpty()) {
            log_file.WriteFormat("|    Module name: %s\n", head_packet.payload.module_name.value);
        }
        if(!head_packet.payload.thread_name.IsEmpty()) {
            log_file.WriteFormat("|    Thread name: %s\n", head_packet.payload.thread_name.value);
        }

        /* TODO: more fields! */

        log_file.WriteFormat("|\n");
        log_file.WriteFormat("\\----------------------------------------------------------------------------------------------------/\n");

        if(!head_packet.payload.text_log.IsEmpty()) {
            /* Concatenate all the packets' messages. */
            for(auto &packet: packet_list) {
                log_file.WriteFormat("%s", packet.payload.text_log.value);
            }
            log_file.WriteFormat("\n");
        }
    }
}