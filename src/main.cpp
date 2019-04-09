#include <cstdlib>

#include <signal.h>

#include <server/core.h>
#include <daemon/daemon.h>

namespace {

void SignalHandler(int sig) {
    exit(0);
}

} // namespace

int main(int argc, const char *argv[]) {
    if (!RunDaemon(argc, argv)) {
        // register signal handler
        signal(SIGINT, SignalHandler);
        // run server core
        auto &core = Core::Instance();
        core.StartListen();
    }
    return 0;
}
