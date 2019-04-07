#include <server/core.h>

#include <utility>

#include <util/logger.h>

namespace {

static constexpr int kHTTPLocalPort = 80;

} // namespace

void Core::InitSocket() {
    sock_.set_local_port(kHTTPLocalPort);
}

void Core::LogError(const char *message) {
    ulog << "[Core " << utime << "] ERROR: ";
    ulog << message << std::endl;
}

Core &Core::Instance() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    // initialize an static instance of 'Core'
    static Core core;
    return core;
}

void Core::StartListen() {
    sock_.Listen();
    for (;;) {
        // wait for new connection
        auto socket = sock_.AcceptNew();
        // check if an error has occurred
        if (sock_.status() == Socket::Status::Error) {
            // log error
            LogError("socket error, connection reset");
            // reset current socket
            sock_.Reset();
            InitSocket();
            sock_.Listen();
        }
        else {
            // create new worker to handle this connection
            workers_.emplace_front(std::move(socket));
        }
        // remove all of the workers which is done
        for (auto prev = workers_.before_begin(), it = workers_.begin();
                it != workers_.end(); ++it) {
            if (it->done()) prev = workers_.erase_after(prev);
        }
    }
}
