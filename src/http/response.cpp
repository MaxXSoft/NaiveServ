#include <http/response.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <cassert>

namespace {

// status code -> status description
static const std::map<int, std::string> kStatusText = {
    {100, "Continue"},
    {101, "Switching Protocols"},
    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {203, "Non-Authoritative Information"},
    {204, "No Content"},
    {205, "Reset Content"},
    {206, "Partial Content"},
    {300, "Multiple Choices"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {303, "See Other"},
    {304, "Not Modified"},
    {305, "Use Proxy"},
    {306, "Unused"},
    {307, "Temporary Redirect"},
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {402, "Payment Required"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {406, "Not Acceptable"},
    {407, "Proxy Authentication Required"},
    {408, "Request Time-out"},
    {409, "Conflict"},
    {410, "Gone"},
    {411, "Length Required"},
    {412, "Precondition Failed"},
    {413, "Request Entity Too Large"},
    {414, "Request-URI Too Large"},
    {415, "Unsupported Media Type"},
    {416, "Requested range not satisfiable"},
    {417, "Expectation Failed"},
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Time-out"},
    {505, "HTTP Version not supported"},
};

static constexpr const char *kDateFormat = "%a, %d %b %y %T GMT";

} // namespace

void HTTPResponse::AutoFill() {
    std::ostringstream oss;
    // fill date
    auto now = std::chrono::system_clock::now();
    auto time_now = std::chrono::system_clock::to_time_t(now);
    oss << std::put_time(std::localtime(&time_now), kDateFormat);
    field_info_["Date"] = oss.str();
    // fill Content-Length
    oss.clear();
    oss.str("");
    oss << data_.size();
    field_info_["Content-Length"] = oss.str();
    // fill Server
    field_info_["Server"] = kServerName;
    // fill Connection
    // NOTE: each connection is a new TCP connection
    field_info_["Connection"] = "Closed";
}

std::string HTTPResponse::ToString() {
    std::ostringstream oss;
    // set HTTP response version
    oss << "HTTP/" << kMajorVersion << "." << kMinorVersion << " ";
    // status code and text
    auto it = kStatusText.find(status_code_);
    assert(it != kStatusText.end());
    oss << status_code_ << " " << it->second << "\r\n";
    // other fields
    AutoFill();
    for (const auto &i : field_info_) {
        oss << i.first << ": " << i.second << "\r\n";
    }
    // response content
    oss << "\r\n";
    for (const auto &i : data_) oss << i;
    return oss.str();
}
