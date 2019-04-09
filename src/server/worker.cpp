#include <server/worker.h>

#include <string>
#include <cassert>

#include <config/config.h>
#include <http/parser.h>
#include <server/responder/router.h>
#include <util/logger.h>

void Worker::LogError(const char *message) {
    ulog << "[Worker " << utime << "] ERROR (";
    ulog << "remote '" << socket_.remote_name() << "'):";
    ulog << message;
    ulog << std::endl;
}

void Worker::StartWork() {
    work_.detach();
}

std::string Worker::FetchData() {
    auto &config = ConfigReader::Instance();
    const int kDefaultBufferSize = config.sock_buffer_size();
    // initialize buffer
    std::string data;
    std::size_t len = kDefaultBufferSize, pos = 0;
    data.resize(len);
    auto buf = reinterpret_cast<uint8_t *>(data.data());
    // receive data from client
    do {
        socket_.Receive(buf + pos, len);
        assert(len <= kDefaultBufferSize);
        if (len == kDefaultBufferSize) {
            pos += kDefaultBufferSize;
            data.resize(kDefaultBufferSize + pos);
        }
    } while (len == kDefaultBufferSize);
    // resize data buffer
    data.resize(pos + len);
    return data;
}

void Worker::HandleConnection() {
    while (!done_) {
        // fetch data from client
        auto request = FetchData();
        // parse request
        HTTPParser parser(request);
        std::string response;
        if (!parser.is_error()) {
            // get responder via router
            auto &router = Router::Instance();
            auto responder = router.GetResponder(parser.url());
            // get response
            response = responder->AcceptRequest(parser).ToString();
        }
        else {
            // parser error
            response = HTTPResponse(500).ToString();
        }
        auto data = reinterpret_cast<std::uint8_t *>(response.data());
        auto len = response.size();
        socket_.Send(data, len);
        // check if need to close current connection
        if (parser.GetFieldValue("Connection") == "closed") {
            socket_.Close();
        }
        // check socket status
        switch (socket_.status()) {
            case Socket::Status::Error: {
                // log error
                LogError("socket error, connection closed");
                // fallthrough
            }
            case Socket::Status::Closed: {
                done_ = true;
                break;
            }
            default:;
        }
    }
}
