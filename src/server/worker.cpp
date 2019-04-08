#include <server/worker.h>

#include <thread>
#include <string>
#include <cassert>

#include <http/parser.h>
#include <server/responder/responder.h>
#include <util/logger.h>

namespace {

static constexpr std::size_t kDefaultBufferSize = 1024;

} // namespace

void Worker::StartWork() {
    std::thread work(&Worker::HandleConnection, this);
    work.detach();
}

void Worker::HandleConnection() {
    // initialize buffer
    std::string data;
    auto buf = reinterpret_cast<uint8_t *>(data.data());
    // receive data from client
    std::size_t len = kDefaultBufferSize, pos = 0;
    data.resize(len);
    do {
        socket_.Receive(buf + pos, len);
        assert(len <= kDefaultBufferSize);
        if (len == kDefaultBufferSize) {
            pos += kDefaultBufferSize;
            data.resize(kDefaultBufferSize + pos);
        }
    } while (len == kDefaultBufferSize);
    // parse data
    data.resize(pos + len);
    HTTPParser parser(data);
    // send response
    // TODO
    // check error
    if (socket_.status() == Socket::Status::Error) {
        // TODO
    }
}
