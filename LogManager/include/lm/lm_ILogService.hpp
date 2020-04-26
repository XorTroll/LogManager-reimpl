
#pragma once
#include <lm/lm_ILogger.hpp>

namespace ams::lm {

    class ILogService : public sf::IServiceObject {

        private:
            enum class CommandId {
                OpenLogger = 0,
            };

        private:
            void OpenLogger(const sf::ClientProcessId &client_pid, sf::Out<std::shared_ptr<ILogger>> out_logger) {
                u64 process_id = client_pid.GetValue().value;
                u64 program_id = 0;
                /* TODO: should we check the Result here? */
                pminfoGetProgramId(&program_id, process_id);
                auto logger = std::make_shared<ILogger>(program_id, process_id);
                out_logger.SetValue(std::move(logger));
            }

        public:
            DEFINE_SERVICE_DISPATCH_TABLE {
                MAKE_SERVICE_COMMAND_META(OpenLogger),
            };

    };

}