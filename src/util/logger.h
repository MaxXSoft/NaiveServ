#ifndef NAIVESERV_UTIL_LOGGER_H_
#define NAIVESERV_UTIL_LOGGER_H_

#include <iostream>
#include <string>
#include <fstream>
#include <mutex>

// an implementation of thread-safe logger
class Logger {
public:
    Logger() : log_to_stderr_(true) {}
    Logger(const std::string &log_file)
            : log_to_stderr_(false), ofs_(log_file, std::ios::app) {}

    template <typename T>
    Logger &operator<<(const T &data) {
        Lock lock(logger_mutex_);
        if (log_to_stderr_) std::cerr << data;
        if (ofs_.is_open()) ofs_ << data;
        return *this;
    }

    Logger &operator<<(std::ostream &(*data)(std::ostream &)) {
        Lock lock(logger_mutex_);
        if (log_to_stderr_) std::cerr << data;
        if (ofs_.is_open()) ofs_ << data;
        return *this;
    }

    void set_log_to_stderr(bool enabled) {
        Lock lock(logger_mutex_);
        log_to_stderr_ = enabled;
    }
    bool set_log_file(const std::string &log_file) {
        Lock lock(logger_mutex_);
        ofs_.open(log_file, std::ios::app);
        return ofs_.is_open();
    }

    bool log_to_stderr() const { return log_to_stderr_; }

private:
    using Lock = std::lock_guard<std::mutex>;

    bool log_to_stderr_;
    std::ofstream ofs_;
    // mutex for all loggers
    static std::mutex logger_mutex_;
};

// declarations for global logger 'ulog'
extern Logger ulog;
std::ostream &utime(std::ostream &os);

#endif // NAIVESERV_UTIL_LOGGER_H_
