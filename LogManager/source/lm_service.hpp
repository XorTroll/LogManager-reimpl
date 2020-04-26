
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
            void Log(const sf::InAutoSelectBuffer &buf) {
                auto packet = impl::ParseLogPacket(buf.GetPointer(), buf.GetSize());
                impl::WriteLogPacket(packet, this->program_id);
            }

            /* TODO: use the destination value for something :P */
            void SetDestination(LogDestination destination) {
                this->destination = destination;
            }

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
            void OpenLogger(const sf::ClientProcessId &client_pid, sf::Out<std::shared_ptr<Logger>> out_logger) {
                u64 program_id = 0;
                /* TODO: shall we check the Result here? */
                pminfoGetProgramId(&program_id, static_cast<u64>(client_pid.GetValue()));
                auto logger = std::make_shared<Logger>(program_id);
                out_logger.SetValue(std::move(logger));
            }

        public:
            DEFINE_SERVICE_DISPATCH_TABLE {
                MAKE_SERVICE_COMMAND_META(OpenLogger),
            };

    };

}