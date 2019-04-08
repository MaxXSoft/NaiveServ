#include <server/core.h>

#include <utility>

#include <config/config.h>
#include <util/logger.h>

void Core::InitSocket() {
    sock_.set_local_port(ConfigReader::Instance().port());
}

void Core::LogError(const char *message) {
    ulog << "[Core " << utime << "] ERROR: ";
    ulog << message << std::endl;
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
