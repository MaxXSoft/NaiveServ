#ifndef NAIVESERV_SERVER_CORE_H_
#define NAIVESERV_SERVER_CORE_H_

#include <forward_list>

#include <sock/sock.h>
#include <server/worker.h>

// NOTE: class 'Core' is a singleton
class Core {
public:
    Core(const Core &) = delete;
    Core(Core &&) = delete;

    static Core &Instance() { return *core_instance_; }

    void StartListen();
    void Reset() {
        sock_.Reset();
        InitSocket();
    }

private:
    Core() { InitSocket(); }

    void InitSocket();
    void LogError(const char *message);

    // singleton
    static Core *core_instance_;
    // main socket
    Socket sock_;
    // pool of workers
    std::forward_list<Worker> workers_;
};

#endif // NAIVESERV_SERVER_CORE_H_
