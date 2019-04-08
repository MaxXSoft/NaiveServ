#ifndef NAIVESERV_HTTP_RESPONSE_H_
#define NAIVESERV_HTTP_RESPONSE_H_

#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstdint>
#include <cstddef>

#include <util/version.h>

// wrap HTTP response into string
class HTTPResponse {
public:
    using DataArray = std::vector<std::uint8_t>;

    HTTPResponse() {}
    HTTPResponse(int status_code) : status_code_(status_code) {}

    std::string ToString();

    // setter
    void set_status_code(int status_code) { status_code_ = status_code; }
    void set_field(const std::string &name, const std::string &value) {
        field_info_[name] = value;
    }
    void set_data(const DataArray &data) { data_ = data; }
    void set_data(const std::uint8_t *data, std::size_t len) {
        data_.resize(len);
        std::memcpy(data_.data(), data, len);
    }

private:
    // HTTP version in response message
    static constexpr int kMajorVersion = 1, kMinorVersion = 1;
    static constexpr const char *kServerName = APP_NAME "/" APP_VERSION;

    // auto fill 'Date', 'Content-Length', 'Server' fields to 'field_info_'
    void AutoFill();

    // HTTP status code (e.g. 200)
    int status_code_;
    // other fields
    std::map<std::string, std::string> field_info_;
    // response data
    DataArray data_;
};

#endif // NAIVESERV_HTTP_RESPONSE_H_
