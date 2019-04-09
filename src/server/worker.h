#ifndef NAIVESERV_SERVER_WORKER_H_
#define NAIVESERV_SERVER_WORKER_H_

#include <utility>
#include <thread>

#include <sock/sock.h>

class Worker {
public:
    Worker(Socket socket) : socket_(std::move(socket)),
            work_(&Worker::HandleConnection, this), done_(false) {
        StartWork();
    }
    Worker(const Worker &) = delete;
    Worker(Worker &&) = delete;

    const Worker &operator=(const Worker &) = delete;
    const Worker &operator=(Worker &&) = delete;

    bool done() const { return done_; }

private:
    void LogError(const char *message);
    void StartWork();
    std::string FetchData();
    void HandleConnection();

    Socket socket_;
    std::thread work_;
    // indicate if work is done
    bool done_;
};

#endif // NAIVESERV_SERVER_WORKER_H_
