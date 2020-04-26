#include <lm/log/log_Logging.hpp>
#include <mutex>

namespace ams::lm::log {

    namespace {

        /* Guard logs with a lock. */
        static inline ams::os::Mutex g_logging_lock(false);

    }

    static constexpr const char LogFileName[] = "sdmc:/atmosphere/lm.log";

    Result LogMessage(const std::string &msg) {
        std::scoped_lock lk(g_logging_lock);

        /* This ensures it's created. */
        fs::CreateFile(LogFileName, 0);

        fs::FileHandle log_file;
        R_TRY(fs::OpenFile(&log_file, LogFileName, fs::OpenMode_Write | fs::OpenMode_AllowAppend));
        ON_SCOPE_EXIT { fs::CloseFile(log_file); };

        s64 cur_file_size = 0; 
        R_TRY(fs::GetFileSize(&cur_file_size, log_file));
        R_TRY(fs::WriteFile(log_file, cur_file_size, msg.c_str(), msg.length(), fs::WriteOption::Flush));

        return ResultSuccess();
    }

}