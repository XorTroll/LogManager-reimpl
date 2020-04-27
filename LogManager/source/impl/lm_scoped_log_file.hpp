
#pragma once
#include <stratosphere.hpp>

namespace ams::lm::impl {

    /* TODO: logs can make the log file quite big, consider splitting into various log files? (maybe one log file per process) */

    static constexpr const char LogFile[] = "sdmc:/atmosphere/lm-logs.log";

    /* Based on creport's ScopedFile. */

    class ScopedLogFile {
        NON_COPYABLE(ScopedLogFile);
        NON_MOVEABLE(ScopedLogFile);
        private:
            fs::FileHandle file;
            s64 offset;
            bool opened;
        public:
            ScopedLogFile() : file(), offset(), opened(false) {
                /* Ensure that the file exists. */
                fs::CreateFile(LogFile, 0);
                this->opened = R_SUCCEEDED(fs::OpenFile(std::addressof(this->file), LogFile, fs::OpenMode_Write | fs::OpenMode_AllowAppend));
                if(this->opened) {
                    /* Set current file size as offset to properly append the logs after each other. */
                    fs::GetFileSize(&this->offset, this->file);
                }
            }

            ~ScopedLogFile() {
                if(this->opened) {
                    fs::CloseFile(file);
                }
            }

            bool IsOpen() const {
                return this->opened;
            }

            void WriteString(const char *str);
            void WriteFormat(const char *fmt, ...) __attribute__((format(printf, 2, 3)));

            void Write(const void *data, size_t size);
    };

}