
#pragma once
#include <lm/log/log_Logging.hpp>
#include <vector>

namespace ams::lm::log {

    enum class LogDestination : u32 {
        TMA = 1,
        UART = 2,
        UARTSleeping = 3,
        All = UINT16_MAX
    };

    enum class LogStringId : u8 {
        Message = 2,
        SourceFile = 4,
        FunctionName = 5,
        ThreadName = 7,
        ModuleName = 10,
    };

    struct LogStringEntry {
        LogStringId id;
        u8 length;
        /* Max length wil be 0xFF, since it's sent as a single byte. */
        char str[UINT8_MAX + 1];

        inline constexpr bool IsEmpty() {
            return length == 0;
        }
        
    };

    struct LogBuffer {
        /* TODO: figure out what's this data. */
        u8 unk_data[0x28];
        std::vector<LogStringEntry> entries;
    };

    namespace impl {

        /* Utility. */
        inline std::string Format(const std::string &fmt, ...) {
            int size = fmt.size() * 2 + 50;
            std::string str;
            va_list ap;
            while(true) {
                str.resize(size);
                va_start(ap, fmt);
                auto n = vsnprintf(str.data(), size, fmt.c_str(), ap);
                va_end(ap);
                if((n > -1) && (n < size)) {
                    str.resize(n);
                    return str;
                }
                if(n > -1) {
                    size = n + 1;
                }
                else {
                    size *= 2;
                }
            }
            return str;
        }

    }

    inline void ParseLogBuffer(const void *buf, size_t buf_size, LogBuffer &out_buf) {
        __builtin_memcpy(out_buf.unk_data, buf, sizeof(out_buf.unk_data));
        auto offset = sizeof(out_buf.unk_data);
        auto buf8 = reinterpret_cast<const u8*>(buf);
        while(offset < buf_size) {
            auto buf_offset = buf8 + offset;
            LogStringEntry entry = {};
            entry.id = static_cast<LogStringId>(buf_offset[0]);
            entry.length = buf_offset[1];
            if(entry.length > 0) {
                __builtin_memcpy(entry.str, buf_offset + 2, entry.length);
            }
            out_buf.entries.push_back(entry);
            offset += entry.length + 2;
        }
    }

    /*
    
    Sample log:

    Log {
        Destination: TMA,
        Program ID: 0x0100000000001000,
        Process ID: 0x0,
        Module name: qlaunch,
        Thread name: MainThread,
        Source file: D:/sample/hello.cpp,
        Function name: sayHello,
    }
    ------------------------------
    Hello world!
    ------------------------------
    
    */

    inline std::string FormatLogDestination(LogDestination destination) {
        switch(destination) {
            case LogDestination::TMA:
                return "TMA";
            case LogDestination::UART:
                return "UART";
            case LogDestination::UARTSleeping:
                return "UART (when sleeping)";
            case LogDestination::All:
                return "All (TMA and UART)";
        }
        return "<unknown>";
    }

    inline std::string FormatLogBuffer(LogBuffer &buf, u64 program_id, u64 process_id, LogDestination destination) {
        std::string fmt = "Log {";
        fmt += "\n";
        LogStringEntry msg_entry = {};
        fmt += "    Destination: " + FormatLogDestination(destination) + ",";
        fmt += "\n";
        fmt += "    Program ID: " + impl::Format("0x%016lX", program_id) + ",";
        fmt += "\n";
        fmt += "    Process ID: " + impl::Format("0x%lX", process_id);
        bool last_entry_empty = false;
        for(auto &entry: buf.entries) {
            if(!last_entry_empty) {
                fmt += ",";
                fmt += "\n";
                last_entry_empty = true;
            }
            if(!entry.IsEmpty()) {
                std::string entry_name;
                switch(entry.id) {
                    case LogStringId::Message: {
                        msg_entry = entry;
                        break;
                    }
                    case LogStringId::SourceFile: {
                        entry_name = "Source file";
                        break;
                    }
                    case LogStringId::FunctionName: {
                        entry_name = "Function name";
                        break;
                    }
                    case LogStringId::ThreadName: {
                        entry_name = "Thread name";
                        break;
                    }
                    case LogStringId::ModuleName: {
                        entry_name = "Module name";
                        break;
                    }
                    default: {
                        entry_name = "Unknown(ID " + std::to_string(static_cast<u32>(entry.id)) + ")";
                        break;
                    }
                }
                last_entry_empty = entry_name.empty();
                if(!entry_name.empty()) {
                    fmt += "    " + entry_name + ": " + entry.str;
                }
            }
        }
        fmt += "}";
        fmt += "\n";
        if(msg_entry.IsEmpty()) {
            fmt += "<no message>";
        }
        else {
            /* Number of -'s which will make a simple box for the actual message. */
            const size_t char_no = 50;
            for(u32 i = 0; i < char_no; i++) {
                fmt += "-";
            }
            fmt += "\n";
            fmt += msg_entry.str;
            fmt += "\n";
            for(u32 i = 0; i < char_no; i++) {
                fmt += "-";
            }
        }
        fmt += "\n\n";
        return fmt;
    }

}