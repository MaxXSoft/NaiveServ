#include <iostream>
#include <sstream>
#include <cstring>
#include <signal.h>
#include <sock/sock.h>

using namespace std;

namespace {

static constexpr const char *kHost = "127.0.0.1";
static constexpr int kPort = 4321;

volatile bool quit_loop = false;

void SignalHandler(int sig) {
    quit_loop = true;
}

void PerformClient() {
    Socket sock;
    // setup socket
    sock.set_remote_port(kPort);
    // connect remote host
    sock.Connect(kHost);
    // receive data
    uint8_t buffer[1024];
    size_t len = sizeof(buffer);
    if (!sock.Receive(buffer, len)) {
        cerr << "socket error: failed to receive data" << endl;
        return;
    }
    // print data
    if (len > 0) {
        cout << buffer << endl;
    }
}

void PerformServer() {
    Socket sock;
    int visits = 0;
    // setup socket
    sock.set_local_port(kPort);
    // listen
    sock.Listen();
    // handle connections
    while (!quit_loop) {
        sock.Accept();
        cout << "connected client " << sock.remote_name() << endl;
        // prepare message
        ++visits;
        ostringstream oss;
        oss << "this server has been connected ";
        oss << visits << " time(s)" << endl;
        // send message
        auto msg = oss.str();
        auto data = reinterpret_cast<const uint8_t *>(msg.c_str());
        auto len = msg.size() + 1;
        sock.Send(data, len);
        // close current connection
        sock.Close();
    }
}

} // namespace

int main(int argc, const char *argv[]) {
    if (argc < 2 || !strcmp(argv[1], "-c")) {
        // client
        PerformClient();
    }
    else if (argc >= 2 && !strcmp(argv[1], "-s")) {
        // server
        signal(SIGINT, SignalHandler);
        PerformServer();
    }
    else {
        return 1;
    }
    return 0;
}
