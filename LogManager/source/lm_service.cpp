#include "lm_service.hpp"

namespace ams::lm {

    void Logger::Log(const sf::InAutoSelectBuffer &buf) {
        auto packet = impl::ParseLogPacket(buf.GetPointer(), buf.GetSize());
        impl::WriteLogPacket(packet, this->program_id);
    }

    /* TODO: use the destination value for something :P */
    void Logger::SetDestination(LogDestination destination) {
        this->destination = destination;
    }

    void LogService::OpenLogger(const sf::ClientProcessId &client_pid, sf::Out<std::shared_ptr<Logger>> out_logger) {
        u64 program_id = 0;
        /* TODO: shall we check the Result here? */
        pminfoGetProgramId(&program_id, static_cast<u64>(client_pid.GetValue()));
        auto logger = std::make_shared<Logger>(program_id);
        out_logger.SetValue(std::move(logger));
    }

}