#include <http/response.h>

namespace {

// status code -> status description
static const std::map<int, std::string> status_text_ = {
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

// TODO: put all of internal constants, variables or functions here,
//       or just delete this scope

} // namespace

void HTTPResponse::AutoFill() {
    // TODO: implement this method

    // fill date
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");

    field_info_["Date"] = ss.str();

    // fill Content-Length
    field_info_["Content-Length"] = "0";

    // fill Server
    field_info_["Server"] = kServerName;

    // fill Connection 
    // each connection is a new TCP connection
    field_info_["Connection"] = "Closed";

    // fill Content-Type
}

std::string HTTPResponse::ToString() {
    // TODO: implement this method

    std::ostringstream oss;

    // HTTP/1.1
    oss << "HTTP/" << kMajorVersion << "." << kMinorVersion << " ";

    // Status Code and text
    oss << status_code_ << " " << status_text_.find(status_code_)->second << "\n";

    // other fields
    for (auto i : field_info_){
        oss << i.first << ": " << i.second << "\n";
    }

    // response content
    oss << "Content: ";

    int size = data_.size();
    for (int i=0;i<size;i++) {
        oss << data_[i];
    }

    return oss.str();
}
