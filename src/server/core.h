#ifndef NAIVESERV_SERVER_CORE_H_
#define NAIVESERV_SERVER_CORE_H_

#include <forward_list>

#include <sock/sock.h>
#include <server/worker.h>
#include <util/singleton.h>

class Core : public Singleton<Core> {
public:
    void StartListen();
    void Reset() {
        sock_.Reset();
        InitSocket();
    }

private:
    friend class Singleton<Core>;

    Core() { InitSocket(); }

    void InitSocket();
    void LogError(const char *message);

    // main socket
    Socket sock_;
    // pool of workers
    std::forward_list<Worker> workers_;
};

#endif // NAIVESERV_SERVER_CORE_H_
