#include <util/logger.h>

#include <ctime>

#include <util/os.h>

// definition of global logger 'ulog'
Logger ulog;

// logger mutex
std::mutex Logger::logger_mutex_;

// print time to 'std::ostream'
std::ostream &utime(std::ostream &os) {
    // get current time to buffer
    char buffer[100];
    auto format = "%Y-%m-%d %T";
    auto now = std::time(nullptr);
    // print to stream
    if (std::strftime(buffer, 100, format, std::localtime(&now))) {
        os << buffer << ' ';
    }
    return os;
}
