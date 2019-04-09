#include <daemon/daemon.h>

#include <string>
#include <cstring>
#include <cstdlib>

#include <util/logger.h>

namespace {

static constexpr const char *kStopDaemon = "stop-daemon";

}

bool RunDaemon(int argc, const char *argv[]) {
    // check need to stop
    for (int i = 1; i < argc; ++i) {
        if (!std::strcmp(argv[i], kStopDaemon)) return false;
    }
    // run self
    std::string cmd = argv[0];
    cmd = cmd + " " + kStopDaemon;
    for (;;) {
        int status = std::system(cmd.c_str());
        if (status >= 0 && WIFEXITED(status)) break;
        // log info
        ulog << "[Daemon " << utime;
        ulog << "] INFO: daemon restarted" << std::endl;
    }
    return true;
}
