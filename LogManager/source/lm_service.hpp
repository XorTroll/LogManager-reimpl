
#pragma once
#include "lm_types.hpp"
#include "impl/lm_log_packet.hpp"
#include "impl/lm_scoped_log_file.hpp"

namespace ams::lm {

    class Logger : public sf::IServiceObject {

        private:
            enum class CommandId {
                Log = 0,
                SetDestination = 1,
            };

        private:
            u64 program_id;
            LogDestination destination;

        public:
            Logger(u64 program_id) : program_id(program_id), destination(LogDestination::TMA) {}

        private:
            void Log(const sf::InAutoSelectBuffer &buf);
            void SetDestination(LogDestination destination);

        public:
            DEFINE_SERVICE_DISPATCH_TABLE {
                MAKE_SERVICE_COMMAND_META(Log),
                MAKE_SERVICE_COMMAND_META(SetDestination, hos::Version_3_0_0),
            };

    };

    class LogService : public sf::IServiceObject {
        private:
            enum class CommandId {
                OpenLogger = 0,
            };

        private:
            void OpenLogger(const sf::ClientProcessId &client_pid, sf::Out<std::shared_ptr<Logger>> out_logger);

        public:
            DEFINE_SERVICE_DISPATCH_TABLE {
                MAKE_SERVICE_COMMAND_META(OpenLogger),
            };

    };

}