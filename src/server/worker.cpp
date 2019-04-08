#include <server/worker.h>

#include <thread>
#include <string>
#include <cassert>

#include <config/config.h>
#include <http/parser.h>
#include <server/responder/responder.h>
#include <util/logger.h>

void Worker::LogError(const char *message) {
    ulog << "[Worker " << utime << "] ERROR (";
    ulog << "remote '" << socket_.remote_name() << "'):";
    ulog << message;
    ulog << std::endl;
}

void Worker::StartWork() {
    std::thread work(&Worker::HandleConnection, this);
    work.detach();
}

std::string Worker::FetchData() {
    auto &config = ConfigReader::Instance();
    const int kDefaultBufferSize = config.sock_buffer_size();
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
    // resize data buffer
    data.resize(pos + len);
    return data;
}

void Worker::HandleConnection() {
    while (!done) {
        // fetch data from client
        auto request = FetchData();
        // parse request
        HTTPParser parser(request);
        // get responder
        auto &factory = ResponderFactory::Instance();
        auto responder = factory.GetResponderByURL(parser.url());
        // send response
        auto response = responder->AcceptRequest(parser).ToString();
        auto data = reinterpret_cast<std::uint8_t *>(response.data());
        auto len = response.size();
        socket_.Send(data, len);
        // check error
        if (socket_.status() == Socket::Status::Error) {
            // log error
            LogError("socket error, connection closed");
        }
        // check if connection is closed
        if (parser.GetFieldValue("Connection") == "closed") {
            // close current socket
            socket_.Close();
            // set 'done' flag
            done_ = true;
        }
    }
}
