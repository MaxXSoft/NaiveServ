#include <sock/sock.h>

#include <cassert>
#include <cstring>

namespace {

static constexpr int kListenQueueLen = 100;

#ifdef UTIL_OS_WINDOWS
using socklen_t = int;
#endif

} // namespace

int Socket::socket_counter_ = 0;
std::mutex Socket::counter_mutex_;

Socket::Socket(const Socket &socket, SocketType accepted) {
    IncreaseCounter();
    // copy properties
    proto_ = socket.proto_;
    status_ = Status::Connected;
    local_ = socket.local_;
    remote_ = socket.remote_;
    accepted_ = accepted;
    // invalidate current sockets
    InvalidateSocket(socket_);
}

void Socket::IncreaseCounter() {
    // make sure initialization of socket is thread-safe
    std::lock_guard<std::mutex> lock(counter_mutex_);
#ifdef UTIL_OS_WINDOWS
    if (!socket_counter_) {
        // make Windows socket happy
        WSADATA wsa;
        int err = WSAStartup(MAKEWORD(2, 2), &wsa);
        assert(!err);
    }
#endif
    ++socket_counter_;
}

void Socket::DecreaseCounter() {
    // make sure uninitialization of socket is thread-safe
    std::lock_guard<std::mutex> lock(counter_mutex_);
    --socket_counter_;
#ifdef UTIL_OS_WINDOWS
    // make Windows socket happy
    if (!socket_counter_) WSACleanup();
#endif
}

void Socket::InitSocket() {
    InvalidateSocket(accepted_);
     // initialize socket
    if (!OpenSocket()) return;
    // initialize local address
    InitAddress(local_);
    // initialize remote address
    InitAddress(remote_);
}

void Socket::QuitSocket() {
    if (IsValidSocket(accepted_)) CloseSocket(accepted_);
    CloseSocket(socket_);
}

void Socket::InitAddress(sockaddr_in &addr) {
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    GetAddress(addr) = INADDR_ANY;
}

bool Socket::OpenSocket() {
    if (status_ != Status::Closed) return false;
    // get protocol
    auto proto = proto_ == Protocol::TCP ? IPPROTO_TCP : IPPROTO_UDP;
    // open a new socket
    socket_ = socket(PF_INET, SOCK_STREAM, proto);
    // check is valid
    if (!IsValidSocket(socket_)) return SetError();
    // set socket option
    int on = 1;
    if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
        return SetError();
    }
    status_ = Status::Open;
    return true;
}

bool Socket::Bind() {
    if (status_ != Status::Open) return false;
    auto addr = reinterpret_cast<sockaddr *>(&local_);
    if (bind(socket_, addr, sizeof(sockaddr_in))) return SetError();
    return true;
}

bool Socket::CloseSocket(SocketType &socket) {
#ifdef UTIL_OS_WINDOWS
    if (closesocket(socket)) return SetError();
#else
    if (close(socket)) return SetError();
#endif
    InvalidateSocket(socket);
    status_ = Status::Closed;
    return true;
}

bool Socket::IsValidSocket(SocketType socket) const {
#ifdef UTIL_OS_WINDOWS
    return socket != INVALID_SOCKET;
#else
    return socket >= 0;
#endif
}

void Socket::InvalidateSocket(SocketType &socket) {
#ifdef UTIL_OS_WINDOWS
    socket = INVALID_SOCKET;
#else
    socket = -1;
#endif
}

Socket::Socket(Socket &&socket) noexcept {
    IncreaseCounter();
    // copy properties
    proto_ = socket.proto_;
    status_ = socket.status_;
    local_ = socket.local_;
    remote_ = socket.remote_;
    socket_ = socket.accepted_;
    accepted_ = socket.accepted_;
    // invalidate original sockets
    InvalidateSocket(socket.socket_);
    InvalidateSocket(socket.accepted_);
}

const Socket &Socket::operator=(Socket &&socket) noexcept {
    if (&socket != this) {
        QuitSocket();
        // copy properties
        proto_ = socket.proto_;
        status_ = socket.status_;
        local_ = socket.local_;
        remote_ = socket.remote_;
        socket_ = socket.accepted_;
        accepted_ = socket.accepted_;
        // invalidate original sockets
        InvalidateSocket(socket.socket_);
        InvalidateSocket(socket.accepted_);
    }
    return *this;
}

bool Socket::Listen() {
    if (status_ != Status::Open) return false;
    if (listen(socket_, kListenQueueLen)) return SetError();
    status_ = Status::Listening;
    return true;
}

bool Socket::Connect(const std::string &ip) {
    return Connect(inet_addr(ip.c_str()));
}

bool Socket::Connect(std::uint32_t ip) {
    if (status_ != Status::Open) return false;
    // initialize remote address
    remote_.sin_addr = *reinterpret_cast<in_addr *>(&ip);
    auto addr = reinterpret_cast<sockaddr *>(&remote_);
    // connect to address
    if (connect(socket_, addr, sizeof(sockaddr_in))) return SetError();
    status_ = Status::Connected;
    return true;
}

bool Socket::Accept() {
    if (status_ != Status::Listening) return false;
    // accept the request
    auto addr = reinterpret_cast<sockaddr *>(&remote_);
    auto len = sizeof(sockaddr_in);
    accepted_ = accept(socket_, addr, reinterpret_cast<socklen_t *>(&len));
    // check is valid
    if (!IsValidSocket(accepted_)) return SetError();
    status_ = Status::Connected;
    return true;
}

Socket Socket::AcceptNew() {
    if (status_ != Status::Listening) return nullptr;
    // accept the request
    auto addr = reinterpret_cast<sockaddr *>(&remote_);
    auto len = sizeof(sockaddr_in);
    accepted_ = accept(socket_, addr, reinterpret_cast<socklen_t *>(&len));
    // check is valid
    if (!IsValidSocket(accepted_)) {
        status_ = Status::Error;
        return nullptr;
    }
    else {
        // return new socket
        return Socket(*this, accepted_);
    }
}

bool Socket::Send(const std::uint8_t *data, std::size_t &len) {
    if (status_ != Status::Connected) return false;
    if (proto_ == Protocol::TCP) {
        auto ret = send(accepted_, data, len, 0);
        if (ret < 0) return SetError();
        len = ret;
    }
    else {     // Protocol::UDP
        // TODO: test
        auto addr = reinterpret_cast<sockaddr *>(&remote_);
        auto ret = sendto(socket_, data, len, 0,
                addr, sizeof(sockaddr_in));
        if (ret < 0) return SetError();
        len = ret;
    }
    return true;
}

bool Socket::Receive(std::uint8_t *data, std::size_t &len) {
    if (status_ != Status::Connected) return false;
    if (proto_ == Protocol::TCP) {
        auto ret = recv(socket_, data, len, 0);
        if (ret < 0) return SetError();
        len = ret;
    }
    else {     // Protocol::UDP
        // TODO: test
        auto addr = reinterpret_cast<sockaddr *>(&remote_);
        auto addr_len = static_cast<socklen_t>(sizeof(sockaddr_in));
        auto ret = recvfrom(socket_, data, len, 0, addr, &addr_len);
        if (ret < 0) return SetError();
        len = ret;
    }
    return true;
}

bool Socket::Close() {
    if (status_ != Status::Connected) return false;
    if (IsValidSocket(accepted_)) {
        if (!CloseSocket(accepted_)) return false;
        // switch status
        if (IsValidSocket(socket_)) status_ = Status::Listening;
        // else the status will be 'Status::Closed'
        return true;
    }
    else {
        return CloseSocket(socket_);
    }
}
