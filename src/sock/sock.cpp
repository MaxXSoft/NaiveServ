#include <sock/sock.h>

#include <iostream>
#include <cassert>
#include <cstring>

namespace {

static constexpr int kListenQueueLen = 100;

#ifdef UTIL_OS_WINDOWS
using socklen_t = int;
#endif

} // namespace

void Socket::InitSocket() {
#ifdef UTIL_OS_WINDOWS
    WSADATA wsa;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    assert(!err);
#endif
    InvalidateSocket(accepted_);
    // initialize socket
    if (!OpenSocket()) return;
    // initialize local address
    InitAddress(local_);
    // initialize remote address
    InitAddress(remote_);
}

void Socket::QuitSocket() {
    CloseSocket(socket_);
#ifdef UTIL_OS_WINDOWS
    WSACleanup();
#endif
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

bool Socket::IsValidSocket(SocketType socket) {
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
    int len = sizeof(sockaddr_in);
    accepted_ = accept(socket_, addr, reinterpret_cast<socklen_t *>(&len));
    // check is valid
    if (!IsValidSocket(accepted_)) return SetError();
    status_ = Status::Connected;
    return true;
}

bool Socket::Send(const std::uint8_t *data, std::size_t &len) {
    if (status_ != Status::Connected) return false;
    if (proto_ == Protocol::TCP) {
        auto ret = send(accepted_, data, len, 0);
        if (ret < 0) return SetError();
        len = ret;
    }
    else {     // Protocol::UDP
        // TODO
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
        // TODO
    }
    return true;
}

bool Socket::Close() {
    if (status_ != Status::Connected) return false;
    if (IsValidSocket(accepted_)) {
        if (!CloseSocket(accepted_)) return false;
        status_ = Status::Listening;
        return true;
    }
    else {
        return CloseSocket(socket_);
    }
}
