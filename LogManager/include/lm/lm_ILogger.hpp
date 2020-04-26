
#pragma once
#include <lm/log/log_LogFormat.hpp>
#include <lm/log/log_Logging.hpp>

namespace ams::lm {

    class ILogger : public sf::IServiceObject {

        private:
            enum class CommandId {
                Log = 0,
                SetDestination = 1,
            };

        private:
            u64 program_id;
            u64 process_id;
            log::LogDestination destination;

        public:
            ILogger(u64 program_id, u64 process_id) : program_id(program_id), process_id(process_id), destination(log::LogDestination::TMA) {}

        private:
            void Log(const sf::InAutoSelectBuffer &buf) {
                log::LogBuffer log_buf = {};
                log::ParseLogBuffer(buf.GetPointer(), buf.GetSize(), log_buf);
                const auto msg = log::FormatLogBuffer(log_buf, this->program_id, this->process_id, this->destination);
                /* Result is ignored, since this command must always succeed. */
                log::LogMessage(msg);
            }

            void SetDestination(log::LogDestination destination) {
                this->destination = destination;
            }

        public:
            DEFINE_SERVICE_DISPATCH_TABLE {
                MAKE_SERVICE_COMMAND_META(Log),
                MAKE_SERVICE_COMMAND_META(SetDestination, hos::Version_3_0_0),
            };

    };

}