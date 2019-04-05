#ifndef NAIVESERV_SOCK_SOCK_H_
#define NAIVESERV_SOCK_SOCK_H_

#include <string>
#include <cstdint>
#include <cstddef>

#ifdef UTIL_OS_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>         // close
#include <sys/socket.h>     // socket
#include <arpa/inet.h>      // inet_ntoa
#include <netinet/in.h>     // sockaddr_in
#include <netdb.h>          // hostent
#endif

class Socket {
public:
    enum class Protocol {
        TCP, UDP
    };

    enum class Status {
        Closed, Open, Listening, Connected, Error
    };

    Socket() : proto_(Protocol::TCP), status_(Status::Closed) {
        InitSocket();
    }
    Socket(Protocol protocol) : proto_(protocol), status_(Status::Closed) {
        InitSocket();
    }
    ~Socket() { QuitSocket(); }

    void Reset() {
        QuitSocket();
        InitSocket();
    }

    bool Listen();
    bool Connect(const std::string &ip);
    bool Connect(std::uint32_t ip);
    bool Accept();
    bool Send(const std::uint8_t *data, std::size_t &len);
    bool Receive(std::uint8_t *data, std::size_t &len);
    bool Close();

    // setters
    void set_protocol(Protocol protocol) { proto_ = protocol; }
    void set_local_port(std::uint16_t local_port) {
        local_.sin_port = htons(local_port);
        Bind();
    }
    void set_remote_port(std::uint16_t remote_port) {
        remote_.sin_port = htons(remote_port);
    }

    // getters
    Protocol protocol() const { return proto_; }
    Status status() const { return status_; }
    std::uint16_t local_port() const { return ntohs(local_.sin_port); }
    std::uint16_t remote_port() const { return ntohs(remote_.sin_port); }
    std::uint32_t local_ip() const { return GetAddress(local_); }
    std::uint32_t remote_ip() const { return GetAddress(remote_); }
    const char *local_name() const {
        return inet_ntoa(local_.sin_addr);
    }
    const char *remote_name() const {
        return inet_ntoa(remote_.sin_addr);
    }

private:
#ifdef UTIL_OS_WINDOWS
    using SocketType = SOCKET;
#else
    using SocketType = int;
#endif

    void InitSocket();
    void QuitSocket();
    void InitAddress(sockaddr_in &addr);

    bool OpenSocket();
    bool Bind();
    bool CloseSocket(SocketType &socket);
    bool IsValidSocket(SocketType socket);
    void InvalidateSocket(SocketType &socket);

    bool SetError() {
        status_ = Status::Error;
        return false;
    }

    std::uint32_t &GetAddress(sockaddr_in &addr) const {
        return *reinterpret_cast<std::uint32_t *>(&addr.sin_addr);
    }
    const std::uint32_t &GetAddress(const sockaddr_in &addr) const {
        return *reinterpret_cast<const std::uint32_t *>(&addr.sin_addr);
    }

    Protocol proto_;
    Status status_;
    sockaddr_in local_, remote_;
    SocketType socket_, accepted_;
};

#endif // NAIVESERV_SOCK_SOCK_H_
