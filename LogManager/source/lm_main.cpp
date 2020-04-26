#include "lm_service.hpp"

extern "C" {

    extern u32 __start__;
    u32 __nx_applet_type = AppletType_None;
    u32 __nx_fs_num_sessions = 1;
    
    #define INNER_HEAP_SIZE 0x10000
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char nx_inner_heap[INNER_HEAP_SIZE];

    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);

}

namespace ams {

    ncm::ProgramId CurrentProgramId = ncm::SystemProgramId::LogManager;

    namespace result {

        bool CallFatalOnResultAssertion = true;

    }

}

using namespace ams;

void __libnx_initheap(void) {
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	/* Newlib */
	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end   = (char*)addr + size;
}

void __appInit(void) {
    hos::InitializeForStratosphere();

    sm::DoWithSession([&]() {
        R_ABORT_UNLESS(pminfoInitialize());
        R_ABORT_UNLESS(fsInitialize());
    });

    R_ABORT_UNLESS(fs::MountSdCard("sdmc"));
    ams::CheckApiVersion();
}

void __appExit(void) {
    /* Cleanup services. */
    fs::Unmount("sdmc");
    fsExit();
    pminfoExit();
}

namespace {

    struct ServerOptions {
        static constexpr size_t PointerBufferSize = 0;
        static constexpr size_t MaxDomains = 0x40;
        static constexpr size_t MaxDomainObjects = 0x200;
    };

    constexpr sm::ServiceName LmServiceName = sm::ServiceName::Encode("lm");
    constexpr size_t LmMaxSessions = 30;

    /* lm */
    constexpr size_t NumServers = 1;
    sf::hipc::ServerManager<NumServers, ServerOptions, LmMaxSessions> g_server_manager;

}

int main(int argc, char **argv) {
    /* Set thread name. */
    os::SetThreadNamePointer(os::GetCurrentThread(), "nn.lm.IpcServer");
    AMS_ASSERT(os::GetThreadPriority(os::GetCurrentThread()) == -18);

    /* Add service to manager. */
    R_ABORT_UNLESS(g_server_manager.RegisterServer<lm::LogService>(LmServiceName, LmMaxSessions));
 
    /* Loop forever, servicing our services. */
    g_server_manager.LoopProcess();
 
    return 0;
}